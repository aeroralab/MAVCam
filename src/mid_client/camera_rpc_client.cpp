#include "camera_rpc_client.h"

#include <string>

#include "base/log.h"
#include "camera/camera.pb.h"

namespace mid {

static mavsdk::CameraServer::Result translateFromRpcResult(
    const mavsdk::rpc::camera::CameraResult_Result result);
static mavsdk::rpc::camera::Mode translateFromCameraServerMode(
    const mavsdk::CameraServer::Mode server_mode);

static void fillInformation(const mavsdk::rpc::camera::Information &input,
                            mavsdk::CameraServer::Information &output);
static void fillStorageInformation(const mavsdk::rpc::camera::Status &input,
                                   mavsdk::CameraServer::StorageInformation &output);
static void fillCaptureStatus(const mavsdk::rpc::camera::Status &input,
                              mavsdk::CameraServer::CaptureStatus &output);
mavsdk::Camera::Setting buildSettings(std::string name, std::string value);

std::unique_ptr<mavsdk::rpc::camera::Setting> createRPCSetting(
    const std::string &setting_id, const std::string &setting_description,
    const std::string &option_id, const std::string &option_description);

CameraRpcClient::CameraRpcClient() {}

CameraRpcClient::~CameraRpcClient() {
    stop();
}

bool CameraRpcClient::Init(int rpc_port) {
    std::string target = "0.0.0.0:" + std::to_string(rpc_port);
    //the channel isn't authenticated
    _channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    _stub = mavsdk::rpc::camera::CameraService::NewStub(_channel);

    _init_information = false;

    _should_exit = false;
    _work_thread = new std::thread(work_thread, this);
    return true;
}

mavsdk::CameraServer::Result CameraRpcClient::take_photo(int index) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call take photo " << index;
    _is_capture_in_progress = true;

    mavsdk::rpc::camera::TakePhotoRequest request;
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;
    mavsdk::rpc::camera::TakePhotoResponse response;
    grpc::Status status = _stub->TakePhoto(&context, request, &response);
    _is_capture_in_progress = false;
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Take photo result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::start_video() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call start video ";
    _is_recording_video = true;
    _start_video_time = std::chrono::steady_clock::now();

    mavsdk::rpc::camera::StartVideoRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::StartVideoResponse response;
    grpc::Status status = _stub->StartVideo(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Start video result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call stop video ";
    _is_recording_video = false;

    mavsdk::rpc::camera::StopVideoRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::StopVideoResponse response;
    grpc::Status status = _stub->StopVideo(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Stop video result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::start_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call start video streaming " << stream_id;

    mavsdk::rpc::camera::StartVideoStreamingRequest request;
    request.set_stream_id(stream_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::StartVideoStreamingResponse response;
    grpc::Status status = _stub->StartVideoStreaming(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Start video streaming result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call stop video streaming " << stream_id;

    mavsdk::rpc::camera::StopVideoStreamingRequest request;
    request.set_stream_id(stream_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::StopVideoStreamingResponse response;
    grpc::Status status = _stub->StopVideoStreaming(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Stop video streaming result : " << response.camera_result().result_str();
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
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Set mode result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::format_storage(int storage_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call format storage " << storage_id;

    mavsdk::rpc::camera::FormatStorageRequest request;
    request.set_storage_id(storage_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::FormatStorageResponse response;
    grpc::Status status = _stub->FormatStorage(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << "Format storage result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::reset_settings() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call reset settings";

    mavsdk::rpc::camera::ResetSettingsRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::ResetSettingsResponse response;
    grpc::Status status = _stub->ResetSettings(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << "Reset settings result : " << response.camera_result().result_str();
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
    if (!_init_current_settings) {
        mavsdk::rpc::camera::SubscribeCurrentSettingsRequest request;
        grpc::ClientContext context;
        _current_settings_reader = _stub->SubscribeCurrentSettings(&context, request);

        mavsdk::rpc::camera::CurrentSettingsResponse response;
        if (_current_settings_reader->Read(&response)) {
            for (auto &setting : response.current_settings()) {
                LogDebug() << "setitng " << setting.setting_id() << " value "
                           << setting.option().option_id();
                _settings[setting.setting_id()] = setting.option().option_id();
                settings.emplace_back(
                    buildSettings(setting.setting_id(), setting.option().option_id()));
            }
        }
        _current_settings_reader->Finish();
        _init_current_settings = true;
    }
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraRpcClient::set_setting(mavsdk::Camera::Setting setting) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call set " << setting.setting_id << " to " << setting.option.option_id;

    mavsdk::rpc::camera::SetSettingRequest request;

    request.set_allocated_setting(
        createRPCSetting(setting.setting_id, "", setting.option.option_id, "").release());

    grpc::ClientContext context;
    mavsdk::rpc::camera::SetSettingResponse response;
    grpc::Status status = _stub->SetSetting(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << "Reset settings result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());

    return mavsdk::CameraServer::Result::Success;
}

std::pair<mavsdk::CameraServer::Result, mavsdk::Camera::Setting> CameraRpcClient::get_setting(
    mavsdk::Camera::Setting setting) const {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call get setting " << setting.setting_id;

    mavsdk::rpc::camera::GetSettingRequest request;
    grpc::ClientContext context;

    mavsdk::rpc::camera::GetSettingResponse response;
    grpc::Status status = _stub->GetSetting(&context, request, &response);
    if (!status.ok()) {
        LogError() << "Grpc status error message : " << status.error_message();
        return {mavsdk::CameraServer::Result::NoSystem, setting};
    }
    LogDebug() << "Reset settings result : " << response.camera_result().result_str();

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
        }
        status_reader->Finish();
    }
}

static mavsdk::CameraServer::Result translateFromRpcResult(
    const mavsdk::rpc::camera::CameraResult_Result result) {
    switch (result) {
        default:
            LogError() << "Unknown result enum value: " << static_cast<int>(result);
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
        case mavsdk::CameraServer::Mode::Photo:
            return mavsdk::rpc::camera::Mode::MODE_PHOTO;
        case mavsdk::CameraServer::Mode::Video:
            return mavsdk::rpc::camera::Mode::MODE_VIDEO;
        case mavsdk::CameraServer::Mode::Unknown:
            return mavsdk::rpc::camera::Mode::MODE_UNKNOWN;
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
}

static void fillStorageInformation(const mavsdk::rpc::camera::Status &input,
                                   mavsdk::CameraServer::StorageInformation &output) {
    output.used_storage_mib = input.used_storage_mib();
    output.available_storage_mib = input.available_storage_mib();
    output.total_storage_mib = input.total_storage_mib();
    // output.storage_status =
    output.storage_id = input.storage_id();
    // output.storage_type =
}

static void fillCaptureStatus(const mavsdk::rpc::camera::Status &input,
                              mavsdk::CameraServer::CaptureStatus &output) {
    output.recording_time_s = input.recording_time_s();
    output.available_capacity = input.available_storage_mib();
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

}  // namespace mid