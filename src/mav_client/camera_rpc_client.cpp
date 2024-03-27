#include "camera_rpc_client.h"

#include <string>

#include "base/log.h"
#include "camera/camera.pb.h"

namespace mav {

static mavsdk::CameraServer::Result translateFromRpcResult(
    const mavsdk::rpc::camera::CameraResult_Result result);
static mavsdk::rpc::camera::Mode translateFromCameraServerMode(
    const mavsdk::CameraServer::Mode server_mode);

static void fillInformation(const mavsdk::rpc::camera::Information &input,
                            mavsdk::CameraServer::Information &output);
static void fillVideoStreamInfos(
    const ::google::protobuf::RepeatedPtrField<::mavsdk::rpc::camera::VideoStreamInfo> &input,
    std::vector<mavsdk::CameraServer::VideoStreamInfo> &output);
static void fillStorageInformation(const mavsdk::rpc::camera::Status &input,
                                   mavsdk::CameraServer::StorageInformation &output);
static void fillCaptureStatus(const mavsdk::rpc::camera::Status &input,
                              mavsdk::CameraServer::CaptureStatus &output);
mavsdk::Camera::Setting buildSettings(std::string name, std::string value);

std::unique_ptr<mavsdk::rpc::camera::Setting> createRPCSetting(
    const std::string &setting_id, const std::string &setting_description,
    const std::string &option_id, const std::string &option_description);

static mavsdk::CameraServer::VideoStreamInfo::VideoStreamStatus translateFromRpcVideoStreamStatus(
    const mavsdk::rpc::camera::VideoStreamInfo::VideoStreamStatus video_stream_status);
static mavsdk::CameraServer::VideoStreamInfo::VideoStreamSpectrum
translateFromRpcVideoStreamSpectrum(
    const mavsdk::rpc::camera::VideoStreamInfo::VideoStreamSpectrum video_stream_spectrum);
static mavsdk::CameraServer::StorageInformation::StorageStatus translateFromRpcStorageStatus(
    const mavsdk::rpc::camera::Status::StorageStatus storage_status);
static mavsdk::CameraServer::StorageInformation::StorageType translateFromRpcStorageType(
    const mavsdk::rpc::camera::Status::StorageType storage_type);

CameraRpcClient::CameraRpcClient() {}

CameraRpcClient::~CameraRpcClient() {
    stop();
}

bool CameraRpcClient::init(int rpc_port) {
    std::string target = "0.0.0.0:" + std::to_string(rpc_port);
    // the channel isn't authenticated
    _channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    _stub = mavsdk::rpc::camera::CameraService::NewStub(_channel);

    // call prepare to init mav camera
    mavsdk::rpc::camera::PrepareRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::PrepareResponse response;
    grpc::Status status = _stub->Prepare(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "Call rpc prepare failed with errorcode: " << status.error_code();
        return false;
    }

    _init_information = false;
    _image_count = 0;
    _should_exit = false;
    _work_thread = new std::thread(work_thread, this);
    return true;
}

mavsdk::CameraServer::Result CameraRpcClient::take_photo(int index) {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call take photo " << index;
    _is_capture_in_progress = true;

    mavsdk::rpc::camera::TakePhotoRequest request;
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;
    mavsdk::rpc::camera::TakePhotoResponse response;
    grpc::Status status = _stub->TakePhoto(&context, request, &response);
    _is_capture_in_progress = false;
    if (!status.ok()) {
        base::LogError() << "call rpc take_photo failed with errorcode: " << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << " Take photo result : " << response.camera_result().result_str();
    auto result = translateFromRpcResult(response.camera_result().result());
    if (result == mavsdk::CameraServer::Result::Success) {
        _image_count++;
    }
    return result;
}

mavsdk::CameraServer::Result CameraRpcClient::start_video() {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call start video ";
    _start_video_time = std::chrono::steady_clock::now();

    mavsdk::rpc::camera::StartVideoRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::StartVideoResponse response;
    grpc::Status status = _stub->StartVideo(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "call rpc start_video failed with errorcode : " << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << " Start video result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video() {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call stop video ";

    mavsdk::rpc::camera::StopVideoRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::StopVideoResponse response;
    grpc::Status status = _stub->StopVideo(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "call rpc stop_video failed with errorcode : " << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << " Stop video result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::start_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call start video streaming " << stream_id;

    mavsdk::rpc::camera::StartVideoStreamingRequest request;
    request.set_stream_id(stream_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::StartVideoStreamingResponse response;
    grpc::Status status = _stub->StartVideoStreaming(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "call rpc start_video_streaming failed with errorcode : "
                         << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << " Start video streaming result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call stop video streaming " << stream_id;

    mavsdk::rpc::camera::StopVideoStreamingRequest request;
    request.set_stream_id(stream_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::StopVideoStreamingResponse response;
    grpc::Status status = _stub->StopVideoStreaming(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "call rpc stop_video_streaming failed with errorcode : "
                         << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << " Stop video streaming result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::set_mode(mavsdk::CameraServer::Mode mode) {
    std::lock_guard<std::mutex> lock(_mutex);

    mavsdk::rpc::camera::SetModeRequest request;
    request.set_mode(translateFromCameraServerMode(mode));
    grpc::ClientContext context;
    mavsdk::rpc::camera::SetModeResponse response;
    grpc::Status status = _stub->SetMode(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "call rpc set_mode failed with errorcode: " << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << " Set mode result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::format_storage(int storage_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call format storage " << storage_id;

    mavsdk::rpc::camera::FormatStorageRequest request;
    request.set_storage_id(storage_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::FormatStorageResponse response;
    grpc::Status status = _stub->FormatStorage(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "call rpc format_storage failed with errorcode: "
                         << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << "Format storage result : " << response.camera_result().result_str();
    auto result = translateFromRpcResult(response.camera_result().result());
    if (result == mavsdk::CameraServer::Result::Success) {
        _image_count = 0;
    }
    return result;
}

mavsdk::CameraServer::Result CameraRpcClient::reset_settings() {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call reset settings";

    mavsdk::rpc::camera::ResetSettingsRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::ResetSettingsResponse response;
    grpc::Status status = _stub->ResetSettings(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "call rpc reset_settings failed with errorcode: "
                         << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << "Reset settings result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::fill_information(
    mavsdk::CameraServer::Information &information) {
    if (!_init_information) {
        mavsdk::rpc::camera::SubscribeInformationRequest request;
        grpc::ClientContext context;
        auto information_reader = _stub->SubscribeInformation(&context, request);

        mavsdk::rpc::camera::InformationResponse response;
        if (information_reader->Read(&response)) {
            fillInformation(response.information(), _information);
        }
        information_reader->Finish();
        _init_information = true;
    }

    information = _information;
    base::LogDebug() << "got rpc information " << _information;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraRpcClient::fill_video_stream_info(
    std::vector<mavsdk::CameraServer::VideoStreamInfo> &video_stream_infos) {
    if (!_init_video_stream_info) {
        mavsdk::rpc::camera::SubscribeVideoStreamInfoRequest request;
        grpc::ClientContext context;
        auto video_stream_info_reader = _stub->SubscribeVideoStreamInfo(&context, request);
        mavsdk::rpc::camera::VideoStreamInfoResponse response;
        if (video_stream_info_reader->Read(&response)) {
            fillVideoStreamInfos(response.video_stream_infos(), _video_stream_infos);
        }
        video_stream_info_reader->Finish();

        _init_video_stream_info = true;
    }
    video_stream_infos = _video_stream_infos;
    base::LogDebug() << "got rpc video stream infos ";
    for (auto &it : _video_stream_infos) {
        base::LogDebug() << it;
    }
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraRpcClient::fill_storage_information(
    mavsdk::CameraServer::StorageInformation &storage_information) {
    storage_information = _storage_information;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraRpcClient::fill_capture_status(
    mavsdk::CameraServer::CaptureStatus &capture_status) {
    capture_status = _capture_status;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraRpcClient::retrieve_current_settings(
    std::vector<mavsdk::Camera::Setting> &settings) {
    std::lock_guard<std::mutex> lock(_mutex);
    mavsdk::rpc::camera::SubscribeCurrentSettingsRequest request;
    grpc::ClientContext context;
    _current_settings_reader = _stub->SubscribeCurrentSettings(&context, request);

    mavsdk::rpc::camera::CurrentSettingsResponse response;
    if (_current_settings_reader->Read(&response)) {
        for (auto &setting : response.current_settings()) {
            base::LogDebug() << "setitng " << setting.setting_id() << " value "
                             << setting.option().option_id();
            settings.emplace_back(
                buildSettings(setting.setting_id(), setting.option().option_id()));
        }
    }
    _current_settings_reader->Finish();
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraRpcClient::set_setting(mavsdk::Camera::Setting setting) {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call set " << setting.setting_id << " to " << setting.option.option_id;

    mavsdk::rpc::camera::SetSettingRequest request;

    request.set_allocated_setting(
        createRPCSetting(setting.setting_id, "", setting.option.option_id, "").release());

    grpc::ClientContext context;
    mavsdk::rpc::camera::SetSettingResponse response;
    grpc::Status status = _stub->SetSetting(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "Grpc status errorcode: " << status.error_code();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << "Set settings result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());

    return mavsdk::CameraServer::Result::Success;
}

std::pair<mavsdk::CameraServer::Result, mavsdk::Camera::Setting> CameraRpcClient::get_setting(
    mavsdk::Camera::Setting setting) const {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "rpc call get setting " << setting.setting_id;

    mavsdk::rpc::camera::GetSettingRequest request;
    grpc::ClientContext context;

    mavsdk::rpc::camera::GetSettingResponse response;
    grpc::Status status = _stub->GetSetting(&context, request, &response);
    if (!status.ok()) {
        base::LogError() << "Grpc status errorcode : " << status.error_code();
        return {mavsdk::CameraServer::Result::NoSystem, setting};
    }
    base::LogDebug() << "Get settings result : " << response.camera_result().result_str();

    setting.setting_id = response.setting().setting_id();
    setting.setting_description = response.setting().setting_description();
    setting.option.option_id = response.setting().option().option_id();
    setting.option.option_description = response.setting().option().option_description();
    setting.is_range = response.setting().is_range();

    return {mavsdk::CameraServer::Result::Success, setting};
}

void CameraRpcClient::stop() {
    _should_exit = true;
    if (_work_thread != nullptr) {
        _work_thread->join();
        delete _work_thread;
        _work_thread = nullptr;
    }
}

void CameraRpcClient::work_thread(CameraRpcClient *self) {
    while (!self->_should_exit) {
        mavsdk::rpc::camera::SubscribeStatusRequest request;
        grpc::ClientContext context;
        auto status_reader = self->_stub->SubscribeStatus(&context, request);

        mavsdk::rpc::camera::StatusResponse response;
        if (status_reader->Read(&response)) {
            fillStorageInformation(response.camera_status(), self->_storage_information);
            fillCaptureStatus(response.camera_status(), self->_capture_status);
            // TODO need change
            self->_capture_status.image_count = self->_image_count;
        }
        status_reader->Finish();
    }
}

static mavsdk::CameraServer::Result translateFromRpcResult(
    const mavsdk::rpc::camera::CameraResult_Result result) {
    switch (result) {
        default:
            base::LogError() << "Unknown result enum value: " << static_cast<int>(result);
        // FALLTHROUGH
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_UNKNOWN:
            return mavsdk::CameraServer::Result::Unknown;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS:
            return mavsdk::CameraServer::Result::Success;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_IN_PROGRESS:
            return mavsdk::CameraServer::Result::InProgress;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_BUSY:
            return mavsdk::CameraServer::Result::Busy;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_DENIED:
            return mavsdk::CameraServer::Result::Denied;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_ERROR:
            return mavsdk::CameraServer::Result::Error;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_TIMEOUT:
            return mavsdk::CameraServer::Result::Timeout;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_WRONG_ARGUMENT:
            return mavsdk::CameraServer::Result::WrongArgument;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_NO_SYSTEM:
            return mavsdk::CameraServer::Result::NoSystem;
    }
}

static mavsdk::rpc::camera::Mode translateFromCameraServerMode(
    const mavsdk::CameraServer::Mode server_mode) {
    switch (server_mode) {
        default:
            base::LogError() << "Unknown enum value: " << static_cast<int>(server_mode);
        // FALLTHROUGH
        case mavsdk::CameraServer::Mode::Photo:
            return mavsdk::rpc::camera::Mode::MODE_PHOTO;
        case mavsdk::CameraServer::Mode::Video:
            return mavsdk::rpc::camera::Mode::MODE_VIDEO;
        case mavsdk::CameraServer::Mode::Unknown:
            return mavsdk::rpc::camera::Mode::MODE_UNKNOWN;
    }
}

static mavsdk::CameraServer::Information::CameraCapFlags translateFromRpcCameraCapFlags(
    const mavsdk::rpc::camera::Information::CameraCapFlags camera_cap_flags) {
    switch (camera_cap_flags) {
        default:
            base::LogError() << "Unknown camera_cap_flags enum value: "
                             << static_cast<int>(camera_cap_flags);
        // FALLTHROUGH
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_CAPTURE_VIDEO:
            return mavsdk::CameraServer::Information::CameraCapFlags::CaptureVideo;
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_CAPTURE_IMAGE:
            return mavsdk::CameraServer::Information::CameraCapFlags::CaptureImage;
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_MODES:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasModes;
        case mavsdk::rpc::camera::
            Information_CameraCapFlags_CAMERA_CAP_FLAGS_CAN_CAPTURE_IMAGE_IN_VIDEO_MODE:
            return mavsdk::CameraServer::Information::CameraCapFlags::CanCaptureImageInVideoMode;
        case mavsdk::rpc::camera::
            Information_CameraCapFlags_CAMERA_CAP_FLAGS_CAN_CAPTURE_VIDEO_IN_IMAGE_MODE:
            return mavsdk::CameraServer::Information::CameraCapFlags::CanCaptureVideoInImageMode;
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_IMAGE_SURVEY_MODE:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasImageSurveyMode;
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_BASIC_ZOOM:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasBasicZoom;
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_BASIC_FOCUS:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasBasicFocus;
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_VIDEO_STREAM:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasVideoStream;
        case mavsdk::rpc::camera::Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_TRACKING_POINT:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasTrackingPoint;
        case mavsdk::rpc::camera::
            Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_TRACKING_RECTANGLE:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasTrackingRectangle;
        case mavsdk::rpc::camera::
            Information_CameraCapFlags_CAMERA_CAP_FLAGS_HAS_TRACKING_GEO_STATUS:
            return mavsdk::CameraServer::Information::CameraCapFlags::HasTrackingGeoStatus;
    }
}

static void fillInformation(const mavsdk::rpc::camera::Information &input,
                            mavsdk::CameraServer::Information &output) {
    output.vendor_name = input.vendor_name();
    output.model_name = input.model_name();
    output.firmware_version = input.firmware_version();
    output.focal_length_mm = input.focal_length_mm();
    output.horizontal_sensor_size_mm = input.horizontal_sensor_size_mm();
    output.vertical_sensor_size_mm = input.vertical_sensor_size_mm();
    output.horizontal_resolution_px = input.horizontal_resolution_px();
    output.vertical_resolution_px = input.vertical_resolution_px();
    output.lens_id = input.lens_id();
    output.definition_file_version = input.definition_file_version();
    output.definition_file_uri = input.definition_file_uri();
    for (int i = 0; i < input.camera_cap_flags_size(); i++) {
        mavsdk::rpc::camera::Information::CameraCapFlags camera_cap_flag =
            input.camera_cap_flags(i);
        output.camera_cap_flags.emplace_back(translateFromRpcCameraCapFlags(camera_cap_flag));
    }
}

static void fillVideoStreamInfos(
    const ::google::protobuf::RepeatedPtrField<::mavsdk::rpc::camera::VideoStreamInfo> &input,
    std::vector<mavsdk::CameraServer::VideoStreamInfo> &output) {
    for (auto &it : input) {
        mavsdk::CameraServer::VideoStreamInfo video_stream_info;
        video_stream_info.stream_id = it.stream_id();

        video_stream_info.settings.frame_rate_hz = it.settings().frame_rate_hz();
        video_stream_info.settings.horizontal_resolution_pix =
            it.settings().horizontal_resolution_pix();
        video_stream_info.settings.vertical_resolution_pix =
            it.settings().vertical_resolution_pix();
        video_stream_info.settings.bit_rate_b_s = it.settings().bit_rate_b_s();
        video_stream_info.settings.rotation_deg = it.settings().rotation_deg();
        video_stream_info.settings.uri = it.settings().uri();
        video_stream_info.settings.horizontal_fov_deg = it.settings().horizontal_fov_deg();

        video_stream_info.status = translateFromRpcVideoStreamStatus(it.status());
        video_stream_info.spectrum = translateFromRpcVideoStreamSpectrum(it.spectrum());

        base::LogDebug() << video_stream_info;
        output.emplace_back(video_stream_info);
    }
    return;
}

static void fillStorageInformation(const mavsdk::rpc::camera::Status &input,
                                   mavsdk::CameraServer::StorageInformation &output) {
    output.used_storage_mib = input.used_storage_mib();
    output.available_storage_mib = input.available_storage_mib();
    output.total_storage_mib = input.total_storage_mib();
    output.storage_status = translateFromRpcStorageStatus(input.storage_status());
    output.storage_id = input.storage_id();
    output.storage_type = translateFromRpcStorageType(input.storage_type());
}

static void fillCaptureStatus(const mavsdk::rpc::camera::Status &input,
                              mavsdk::CameraServer::CaptureStatus &output) {
    output.recording_time_s = input.recording_time_s();
    output.available_capacity_mib = input.available_storage_mib();
    output.video_status = input.video_on()
                            ? mavsdk::CameraServer::CaptureStatus::VideoStatus::CaptureInProgress
                            : mavsdk::CameraServer::CaptureStatus::VideoStatus::Idle;
}

mavsdk::Camera::Setting buildSettings(std::string name, std::string value) {
    mavsdk::Camera::Setting setting;
    setting.setting_id = name;
    setting.option.option_id = value;
    return setting;
}

std::unique_ptr<mavsdk::rpc::camera::Setting> createRPCSetting(
    const std::string &setting_id, const std::string &setting_description,
    const std::string &option_id, const std::string &option_description) {
    auto setting = std::make_unique<mavsdk::rpc::camera::Setting>();
    setting->set_setting_id(setting_id);
    setting->set_setting_description(setting_description);

    auto option = std::make_unique<mavsdk::rpc::camera::Option>();
    option->set_option_id(option_id);
    option->set_option_description(option_description);
    setting->set_allocated_option(option.release());

    return setting;
}

static mavsdk::CameraServer::VideoStreamInfo::VideoStreamStatus translateFromRpcVideoStreamStatus(
    const mavsdk::rpc::camera::VideoStreamInfo::VideoStreamStatus video_stream_status) {
    switch (video_stream_status) {
        default:
            base::LogError() << "Unknown video_stream_status enum value: "
                             << static_cast<int>(video_stream_status);
        // FALLTHROUGH
        case mavsdk::rpc::camera::VideoStreamInfo_VideoStreamStatus_VIDEO_STREAM_STATUS_NOT_RUNNING:
            return mavsdk::CameraServer::VideoStreamInfo::VideoStreamStatus::NotRunning;
        case mavsdk::rpc::camera::VideoStreamInfo_VideoStreamStatus_VIDEO_STREAM_STATUS_IN_PROGRESS:
            return mavsdk::CameraServer::VideoStreamInfo::VideoStreamStatus::InProgress;
    }
}

static mavsdk::CameraServer::VideoStreamInfo::VideoStreamSpectrum
translateFromRpcVideoStreamSpectrum(
    const mavsdk::rpc::camera::VideoStreamInfo::VideoStreamSpectrum video_stream_spectrum) {
    switch (video_stream_spectrum) {
        default:
            base::LogError() << "Unknown video_stream_spectrum enum value: "
                             << static_cast<int>(video_stream_spectrum);
        // FALLTHROUGH
        case mavsdk::rpc::camera::VideoStreamInfo_VideoStreamSpectrum_VIDEO_STREAM_SPECTRUM_UNKNOWN:
            return mavsdk::CameraServer::VideoStreamInfo::VideoStreamSpectrum::Unknown;
        case mavsdk::rpc::camera::
            VideoStreamInfo_VideoStreamSpectrum_VIDEO_STREAM_SPECTRUM_VISIBLE_LIGHT:
            return mavsdk::CameraServer::VideoStreamInfo::VideoStreamSpectrum::VisibleLight;
        case mavsdk::rpc::camera::
            VideoStreamInfo_VideoStreamSpectrum_VIDEO_STREAM_SPECTRUM_INFRARED:
            return mavsdk::CameraServer::VideoStreamInfo::VideoStreamSpectrum::Infrared;
    }
}

static mavsdk::CameraServer::StorageInformation::StorageStatus translateFromRpcStorageStatus(
    const mavsdk::rpc::camera::Status::StorageStatus storage_status) {
    switch (storage_status) {
        default:
            base::LogError() << "Unknown storage_status enum value: "
                             << static_cast<int>(storage_status);
        // FALLTHROUGH
        case mavsdk::rpc::camera::Status_StorageStatus_STORAGE_STATUS_NOT_AVAILABLE:
            return mavsdk::CameraServer::StorageInformation::StorageStatus::NotAvailable;
        case mavsdk::rpc::camera::Status_StorageStatus_STORAGE_STATUS_UNFORMATTED:
            return mavsdk::CameraServer::StorageInformation::StorageStatus::Unformatted;
        case mavsdk::rpc::camera::Status_StorageStatus_STORAGE_STATUS_FORMATTED:
            return mavsdk::CameraServer::StorageInformation::StorageStatus::Formatted;
        case mavsdk::rpc::camera::Status_StorageStatus_STORAGE_STATUS_NOT_SUPPORTED:
            return mavsdk::CameraServer::StorageInformation::StorageStatus::NotSupported;
    }
}

static mavsdk::CameraServer::StorageInformation::StorageType translateFromRpcStorageType(
    const mavsdk::rpc::camera::Status::StorageType storage_type) {
    switch (storage_type) {
        default:
            base::LogError() << "Unknown storage_type enum value: "
                             << static_cast<int>(storage_type);
        // FALLTHROUGH
        case mavsdk::rpc::camera::Status_StorageType_STORAGE_TYPE_UNKNOWN:
            return mavsdk::CameraServer::StorageInformation::StorageType::Unknown;
        case mavsdk::rpc::camera::Status_StorageType_STORAGE_TYPE_USB_STICK:
            return mavsdk::CameraServer::StorageInformation::StorageType::UsbStick;
        case mavsdk::rpc::camera::Status_StorageType_STORAGE_TYPE_SD:
            return mavsdk::CameraServer::StorageInformation::StorageType::Sd;
        case mavsdk::rpc::camera::Status_StorageType_STORAGE_TYPE_MICROSD:
            return mavsdk::CameraServer::StorageInformation::StorageType::Microsd;
        case mavsdk::rpc::camera::Status_StorageType_STORAGE_TYPE_HD:
            return mavsdk::CameraServer::StorageInformation::StorageType::Hd;
        case mavsdk::rpc::camera::Status_StorageType_STORAGE_TYPE_OTHER:
            return mavsdk::CameraServer::StorageInformation::StorageType::Other;
    }
}

}  // namespace mav