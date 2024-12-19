#include "camera_local_client.h"

#include <dlfcn.h>
#include <string.h>
#include <unistd.h>

#include <chrono>
#include <iomanip>  // for std::setprecision
#include <regex>
#include <thread>

#include "base/log.h"

namespace mavcam {

const std::string kCameraModeName = "CAM_MODE";
const std::string kCameraDisplayModeName = "CAM_DIS_MODE";
const std::string kPhotoResolution = "CAM_PHOTO_RES";
const std::string kPhotoQuality = "CAM_PHOTO_QC";
const std::string kVideoResolution = "CAM_VIDRES";
const std::string kVideoFormat = "CAM_VIDFMT";
const std::string kWhitebalanceModeName = "CAM_WBMODE";
const std::string kExposureMode = "CAM_EXPMODE";
const std::string kEVName = "CAM_EV";
const std::string kISOName = "CAM_ISO";
const std::string kShutterSpeedName = "CAM_SHUTTERSPD";
const std::string kMeteringModeName = "CAM_METER";

const std::string kIrCamPalette = "IRCAM_PALETTE";
const std::string kIrCamFFC = "IRCAM_FFC";

static const int32_t kPreviewWidth = 1920;
static const int32_t kPreviewPhotoHeight = 1440;
static const int32_t kPreviewVideoHeight = 1080;

static int32_t kSnapshotWidth = 1920;
static int32_t kSnapshotHeight = 1440;
static int32_t kSnapshotHalfWidth = 1920;
static int32_t kSnapshotHalfHeight = 1440;
static const int32_t kVideoWidth = 3840;
static const int32_t kVideoHeight = 2160;

#define QCOM_CAMERA_LIBERAY "libqcom_camera.so"
#define IR_CAMERA_LIBRARY "libirextension.so"

typedef mav_camera::MavCamera *(*create_qcom_camera_fun)();

CameraLocalClient::CameraLocalClient() {
    _current_mode = mavsdk::CameraServer::Mode::Unknown;
    _framerate = 30;
    _image_count = 0;
    _is_recording_video = false;
}

CameraLocalClient::~CameraLocalClient() {
    deinit();
}

mavsdk::CameraServer::Result CameraLocalClient::take_photo(int index) {
    base::LogDebug() << "locally call take photo";
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    auto result = _mav_camera->take_photo();
    auto convert_result = convert_camera_result_to_mav_server_result(result);
    if (convert_result == mavsdk::CameraServer::Result::Success) {
        _image_count++;
    }
    return convert_result;
}

mavsdk::CameraServer::Result CameraLocalClient::start_video() {
    base::LogDebug() << "locally call start video";
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    auto result = _mav_camera->start_video();
    auto mav_result = convert_camera_result_to_mav_server_result(result);
    if (mav_result == mavsdk::CameraServer::Result::Success) {
        _is_recording_video = true;
        _start_video_time = std::chrono::steady_clock::now();
    }
    return mav_result;
}

mavsdk::CameraServer::Result CameraLocalClient::stop_video() {
    base::LogDebug() << "locally call stop video";
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_is_recording_video) {
        base::LogWarn() << "call stop video without video is recording";
        return mavsdk::CameraServer::Result::Success;
    }
    auto result = _mav_camera->stop_video();
    auto mav_result = convert_camera_result_to_mav_server_result(result);
    if (mav_result == mavsdk::CameraServer::Result::Success) {
        _is_recording_video = false;
    }
    return mav_result;
}

mavsdk::CameraServer::Result CameraLocalClient::start_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "locally call start video streaming";
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::stop_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    base::LogDebug() << "locally call stop video streaming";
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::set_mode(mavsdk::CameraServer::Mode mode) {
    base::LogDebug() << "locally call set mode " << mode;
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    if (_current_mode == mode) {
        // same mode do not change again
        return mavsdk::CameraServer::Result::Success;
    }
    mav_camera::Result result = mav_camera::Result::Unknown;
    std::string setting_mode = "0";
    if (mode == mavsdk::CameraServer::Mode::Photo) {
        setting_mode = "0";
    } else {
        setting_mode = "1";
    }
    // use set setting to change camera mode
    auto setting = build_setting(kCameraModeName, setting_mode);
    return set_setting(setting);
}

mavsdk::CameraServer::Result CameraLocalClient::format_storage(int storage_id) {
    base::LogDebug() << "locally call format storage " << storage_id;
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    auto result = _mav_camera->format_storage(storage_id);
    auto convert_result = convert_camera_result_to_mav_server_result(result);
    if (convert_result == mavsdk::CameraServer::Result::Success) {
        // clear image count
        _image_count = 0;
    }
    return convert_result;
}

mavsdk::CameraServer::Result CameraLocalClient::reset_settings() {
    base::LogDebug() << "locally call reset settings";
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    std::lock_guard<std::mutex> lock(_mutex);

    auto result = _mav_camera->reset_settings();
    if (result == mav_camera::Result::Success) {
        // reset settings value
        _settings[kCameraModeName] = "0";
        _settings[kCameraDisplayModeName] = "3";
        _settings[kPhotoQuality] = "0";
        _settings[kWhitebalanceModeName] = "0";
        _settings[kExposureMode] = "0";
        _settings[kEVName] = "0";
        _settings[kISOName] = "125";
        _settings[kShutterSpeedName] = "0.01";
        _settings[kVideoFormat] = "1";
        _settings[kMeteringModeName] = "0";
    }
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::set_timestamp(int64_t time_unix_msec) {
    base::LogDebug() << "local call set timestamp " << time_unix_msec;
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    auto result = _mav_camera->set_timestamp(time_unix_msec);
    return convert_camera_result_to_mav_server_result(result);
}

mavsdk::CameraServer::Result CameraLocalClient::set_zoom_range(float range) {
    base::LogDebug() << "local call set zoom range " << range;
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    std::lock_guard<std::mutex> lock(_mutex);
    auto result = _mav_camera->set_zoom(range);
    return convert_camera_result_to_mav_server_result(result);
}

mavsdk::CameraServer::Result CameraLocalClient::fill_information(
    mavsdk::CameraServer::Information &information) {
    mav_camera::Information in_info;
    mav_camera::Result result = mav_camera::Result::NoSystem;
    if (_mav_camera != nullptr) {
        result = _mav_camera->get_information(in_info);
    }
    if (result == mav_camera::Result::Success) {
        information.vendor_name = "Aeroratech";
        information.model_name = "D64TR";
        information.firmware_version = "0.7.0";
        information.focal_length_mm = in_info.focal_length_mm;
        information.horizontal_sensor_size_mm = in_info.horizontal_sensor_size_mm;
        information.vertical_sensor_size_mm = in_info.vertical_sensor_size_mm;
        information.horizontal_resolution_px = in_info.horizontal_resolution_px;
        information.vertical_resolution_px = in_info.vertical_resolution_px;
        information.lens_id = in_info.lens_id;
        //TODO (Thomas) : hard code
        information.definition_file_version = 10;
        information.definition_file_uri = "mftp://definition/D64TR.xml";

    } else {
        information.vendor_name = "Unknown";
        information.model_name = "Unknown";
        information.firmware_version = "0.0.0";
        information.focal_length_mm = 0;
        information.horizontal_sensor_size_mm = 0;
        information.vertical_sensor_size_mm = 0;
        information.horizontal_resolution_px = 0;
        information.vertical_resolution_px = 0;
        information.lens_id = 0;
        information.definition_file_version = 0;
        information.definition_file_uri = "";
        return mavsdk::CameraServer::Result::NoSystem;
    }

    information.camera_cap_flags.emplace_back(
        mavsdk::CameraServer::Information::CameraCapFlags::CaptureImage);
    information.camera_cap_flags.emplace_back(
        mavsdk::CameraServer::Information::CameraCapFlags::CaptureVideo);
    information.camera_cap_flags.emplace_back(
        mavsdk::CameraServer::Information::CameraCapFlags::HasModes);
    information.camera_cap_flags.emplace_back(
        mavsdk::CameraServer::Information::CameraCapFlags::HasVideoStream);
    information.camera_cap_flags.emplace_back(
        mavsdk::CameraServer::Information::CameraCapFlags::HasBasicZoom);
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::fill_video_stream_info(
    std::vector<mavsdk::CameraServer::VideoStreamInfo> &video_stream_infos) {
    video_stream_infos.clear();

    mavsdk::CameraServer::VideoStreamInfo normal_video_stream;
    normal_video_stream.stream_id = 1;

    normal_video_stream.settings.frame_rate_hz = 60.0;
    normal_video_stream.settings.horizontal_resolution_pix = 1920;
    normal_video_stream.settings.vertical_resolution_pix = 1080;
    normal_video_stream.settings.bit_rate_b_s = 4 * 1024 * 1024;
    normal_video_stream.settings.rotation_deg = 0;
    normal_video_stream.settings.uri = "rtsp://192.168.251.1/live";
    normal_video_stream.settings.horizontal_fov_deg = 0;
    normal_video_stream.status =
        mavsdk::CameraServer::VideoStreamInfo::VideoStreamStatus::InProgress;
    normal_video_stream.spectrum =
        mavsdk::CameraServer::VideoStreamInfo::VideoStreamSpectrum::VisibleLight;

    video_stream_infos.emplace_back(normal_video_stream);

    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::fill_storage_information(
    mavsdk::CameraServer::StorageInformation &storage_information) {
    std::lock_guard<std::mutex> lock(_storage_information_mutex);
    storage_information.total_storage_mib = _current_storage_information.total_storage_mib;
    storage_information.used_storage_mib = _current_storage_information.used_storage_mib;
    storage_information.available_storage_mib = _current_storage_information.available_storage_mib;

    switch (_current_storage_information.storage_status) {
        case mav_camera::StorageInformation::StorageStatus::Formatted:
            storage_information.storage_status =
                mavsdk::CameraServer::StorageInformation::StorageStatus::Formatted;
            break;
        case mav_camera::StorageInformation::StorageStatus::Unformatted:
            storage_information.storage_status =
                mavsdk::CameraServer::StorageInformation::StorageStatus::Unformatted;
            break;
        case mav_camera::StorageInformation::StorageStatus::NotAvailable:
            storage_information.storage_status =
                mavsdk::CameraServer::StorageInformation::StorageStatus::NotAvailable;
            break;
        case mav_camera::StorageInformation::StorageStatus::NotSupported:
            storage_information.storage_status =
                mavsdk::CameraServer::StorageInformation::StorageStatus::NotSupported;
            break;
    }

    switch (_current_storage_information.storage_type) {
        case mav_camera::StorageType::Hd:
            storage_information.storage_type =
                mavsdk::CameraServer::StorageInformation::StorageType::Hd;
            break;
        case mav_camera::StorageType::Microsd:
            storage_information.storage_type =
                mavsdk::CameraServer::StorageInformation::StorageType::Microsd;
            break;
        case mav_camera::StorageType::Other:
            storage_information.storage_type =
                mavsdk::CameraServer::StorageInformation::StorageType::Other;
            break;
        case mav_camera::StorageType::Sd:
            storage_information.storage_type =
                mavsdk::CameraServer::StorageInformation::StorageType::Sd;
            break;
        case mav_camera::StorageType::Unknown:
            storage_information.storage_type =
                mavsdk::CameraServer::StorageInformation::StorageType::Unknown;
            break;
        case mav_camera::StorageType::UsbStick:
            storage_information.storage_type =
                mavsdk::CameraServer::StorageInformation::StorageType::UsbStick;
            break;
    }
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::fill_capture_status(
    mavsdk::CameraServer::CaptureStatus &capture_status) {
    // not need lock guard
    capture_status.available_capacity_mib = _current_storage_information.available_storage_mib;
    capture_status.image_count = _image_count;
    capture_status.image_status = mavsdk::CameraServer::CaptureStatus::ImageStatus::Idle;
    capture_status.video_status =
        _is_recording_video ? mavsdk::CameraServer::CaptureStatus::VideoStatus::CaptureInProgress
                            : mavsdk::CameraServer::CaptureStatus::VideoStatus::Idle;
    if (_is_recording_video) {
        auto current_time = std::chrono::steady_clock::now();
        capture_status.recording_time_s =
            std::chrono::duration_cast<std::chrono::seconds>(current_time - _start_video_time)
                .count();
    } else {
        capture_status.recording_time_s = 0;
    }
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::fill_settings(
    mavsdk::CameraServer::Settings &settings) {
    base::LogDebug() << "locally call fill settings ";
    if (_settings[kCameraModeName] == "0") {
        settings.mode = mavsdk::CameraServer::Mode::Photo;
    } else {
        settings.mode = mavsdk::CameraServer::Mode::Video;
    }
    settings.zoom_level = 0;
    settings.focus_level = 0;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::retrieve_current_settings(
    std::vector<mavsdk::Camera::Setting> &settings) {
    settings.clear();
    for (auto &it : _settings) {
        settings.emplace_back(build_setting(it.first, it.second));
    }

    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::set_setting(mavsdk::Camera::Setting setting) {
    if (_mav_camera == nullptr) {
        return mavsdk::CameraServer::Result::NoSystem;
    }
    base::LogDebug() << "change " << setting.setting_id << " to " << setting.option.option_id;
    if (_settings.count(setting.setting_id) == 0) {
        base::LogError() << "Unsupport setting " << setting.setting_id;
        return mavsdk::CameraServer::Result::WrongArgument;
    }
    bool set_success = false;
    if (setting.setting_id == kCameraModeName) {
        mav_camera::Mode set_mode = mav_camera::Mode::Unknown;
        if (setting.option.option_id == "0") {
            set_mode = mav_camera::Mode::Photo;
        } else {
            set_mode = mav_camera::Mode::Video;
        }
        auto result = _mav_camera->set_mode(set_mode);
        set_success = result == mav_camera::Result::Success;
        if (set_success) {
            if (set_mode == mav_camera::Mode::Photo) {
                _current_mode = mavsdk::CameraServer::Mode::Photo;
            } else {
                _current_mode = mavsdk::CameraServer::Mode::Video;
            }
        }
    } else if (setting.setting_id == kCameraDisplayModeName) {
        set_success = set_camera_display_mode(setting.option.option_id);
    } else if (setting.setting_id == kPhotoResolution) {
        if (setting.option.option_id == "0") {
            auto result = _mav_camera->set_snapshot_resolution(kSnapshotWidth, kSnapshotHeight);
            set_success = result == mav_camera::Result::Success;
        } else if (setting.option.option_id == "1") {
            auto result =
                _mav_camera->set_snapshot_resolution(kSnapshotHalfWidth, kSnapshotHalfHeight);
            set_success = result == mav_camera::Result::Success;
        }
    } else if (setting.setting_id == kPhotoQuality) {
        mav_camera::JpegQuality jpeg_quality;
        if (setting.option.option_id == "0") {
            jpeg_quality = mav_camera::JpegQuality::SuperFine;
        } else if (setting.option.option_id == "1") {
            jpeg_quality = mav_camera::JpegQuality::Fine;
        } else if (setting.option.option_id == "2") {
            jpeg_quality = mav_camera::JpegQuality::Normal;
        }
        auto result = _mav_camera->set_jpeg_quality(jpeg_quality);
        set_success = result == mav_camera::Result::Success;
    } else if (setting.setting_id == kWhitebalanceModeName) {  // whitebalance mode
        set_success = set_whitebalance_mode(setting.option.option_id);
    } else if (setting.setting_id == kExposureMode) {
        // exposure mode not set to camera implement
        set_success = true;
    } else if (setting.setting_id == kEVName) {  // exposure value
        auto result = _mav_camera->set_exposure_value(std::stof(setting.option.option_id));
        set_success = result == mav_camera::Result::Success;
    } else if (setting.setting_id == kISOName) {
        auto result = _mav_camera->set_iso(std::stoi(setting.option.option_id));
        set_success = result == mav_camera::Result::Success;
    } else if (setting.setting_id == kShutterSpeedName) {
        auto result = _mav_camera->set_shutter_speed(setting.option.option_id);
        set_success = result == mav_camera::Result::Success;
    } else if (setting.setting_id == kVideoResolution) {
        set_success = set_video_resolution(setting.option.option_id);
    } else if (setting.setting_id == kMeteringModeName) {
        set_success = set_metering_mode(setting.option.option_id);
    } else if (setting.setting_id == kIrCamPalette) {
        set_success = set_ir_palette(setting.option.option_id);
    } else if (setting.setting_id == kIrCamFFC) {
        set_success = set_ir_FFC(setting.option.option_id);
    } else {
        base::LogError() << "Not implement setting" << setting.setting_id;
        set_success = false;
    }

    // when set success update the settings value
    if (set_success) {
        _settings[setting.setting_id] = setting.option.option_id;
    }
    return mavsdk::CameraServer::Result::Success;
}

std::pair<mavsdk::CameraServer::Result, mavsdk::Camera::Setting> CameraLocalClient::get_setting(
    mavsdk::Camera::Setting setting) const {
    base::LogDebug() << "call get_setting " << setting.setting_id;
    if (_settings.count(setting.setting_id) == 0) {
        return {mavsdk::CameraServer::Result::WrongArgument, setting};
    }
    setting.option.option_id = _settings[setting.setting_id];
    base::LogDebug() << "get " << setting.setting_id << " return " << setting.option.option_id;
    return {mavsdk::CameraServer::Result::Success, setting};
}

bool CameraLocalClient::init() {
    if (_mav_camera != nullptr) {
        return true;
    }

    //init ir camera first for ir stream function
    auto ir_result = init_ir_camera();
    if (ir_result) {
        int color_mode = get_ir_palette();
        _settings[kIrCamPalette] = std::to_string(color_mode);
        _settings[kIrCamFFC] = "0";
    }

    _plugin_handle = dlopen(QCOM_CAMERA_LIBERAY, RTLD_NOW);
    if (_plugin_handle == NULL) {
        char const *err_str = dlerror();
        base::LogError() << "load module " << QCOM_CAMERA_LIBERAY << " failed "
                         << (err_str != NULL ? err_str : "unknown");
        return false;
    }

    create_qcom_camera_fun create_camera_fun =
        (create_qcom_camera_fun)dlsym(_plugin_handle, "create_qcom_camera");
    if (create_camera_fun == NULL) {
        base::LogError() << "cannot find symbol create_qcom_camera";
        dlclose(_plugin_handle);
        _plugin_handle = NULL;
        return false;
    }

    _mav_camera = create_camera_fun();
    if (_mav_camera == nullptr) {
        base::LogError() << "cannot create mav camera instance";
        dlclose(_plugin_handle);
        _plugin_handle = NULL;
        return false;
    }

    _mav_camera->set_log_path("/data/camera/qcom_cam.log");
    mav_camera::Result result = _mav_camera->prepare();
    if (result != mav_camera::Result::Success) {
        base::LogDebug() << "cannot find qcom camera";
        return false;
    }

    mav_camera::Options options;
    options.preview_drm_output = false;
    options.preview_v4l2_output = false;
    options.preview_weston_output = true;

    auto camera_mode = mav_camera::Mode::Photo;
    const char *init_camera_mode = getenv("MAVCAM_INIT_CAMERA_MODE");
    if (init_camera_mode != NULL) {
        if (strncmp(init_camera_mode, "0", 1) == 0) {
            camera_mode = mav_camera::Mode::Photo;
            base::LogInfo() << "Manually init camera to photo mode";
        } else if (strncmp(init_camera_mode, "1", 1) == 0) {
            camera_mode = mav_camera::Mode::Video;
            base::LogInfo() << "Manually init camera to video mode";
        }
    }
    options.init_mode = camera_mode;

    const char *init_snapshot_resoltuion = getenv("MAVCAM_INIT_SNAPSHOT_RES");
    if (init_snapshot_resoltuion != NULL) {
        std::regex resolutionRegex(R"(^(\d+)x(\d+)$)");
        std::smatch match;
        const auto str_snapshot_resoltuion = std::string(init_snapshot_resoltuion);
        if (std::regex_match(str_snapshot_resoltuion, match, resolutionRegex)) {
            // Extract width and height from the match results
            kSnapshotWidth = std::stoi(match[1].str());
            kSnapshotHeight = std::stoi(match[2].str());

            kSnapshotHalfWidth = kSnapshotWidth / 2;
            kSnapshotHalfHeight = kSnapshotHeight / 2;

            // for manually set snapshot resolution, not use half snapshot resolution
            options.snapshot_width = kSnapshotWidth;
            options.snapshot_height = kSnapshotHeight;
            _settings[kPhotoResolution] = "0";
        }
    } else {
        int32_t snapshot_width = 0;
        int32_t snpashot_height = 0;
        std::tie(result, kSnapshotWidth, kSnapshotHeight) = _mav_camera->get_snapshot_resolution();
        kSnapshotHalfWidth = kSnapshotWidth / 2;
        kSnapshotHalfHeight = kSnapshotHeight / 2;
        if (kSnapshotWidth > 8000) {  // for 64M mode, use half width and height
            options.snapshot_width = kSnapshotHalfWidth;
            options.snapshot_height = kSnapshotHalfHeight;
            _settings[kPhotoResolution] = "1";  // 1 for 1/4 resolution
        } else {
            options.snapshot_width = kSnapshotWidth;
            options.snapshot_height = kSnapshotHeight;
            _settings[kPhotoResolution] = "0";
        }
    }

    options.jpeg_quality = mav_camera::JpegQuality::SuperFine;
    _settings[kPhotoQuality] = "0";  // 0 for jpeg super fine

    if (options.init_mode == mav_camera::Mode::Photo) {
        options.preview_width = kPreviewWidth;
        options.preview_height = kPreviewPhotoHeight;
    } else {
        options.preview_width = kPreviewWidth;
        options.preview_height = kPreviewPhotoHeight;
    }

    options.video_width = kVideoWidth;
    options.video_height = kVideoHeight;

    options.framerate = _framerate;
    options.debug_calc_fps = false;

    const char *store_prefix = getenv("MAVCAM_DEFAULT_STORE_PREFIX");
    if (store_prefix == NULL) {
        base::LogWarn() << "No store prefix found";
    } else {
        options.store_prefix = store_prefix;
        base::LogInfo() << "Set store prefix to " << options.store_prefix;
    }

    result = _mav_camera->open(options);
    if (result == mav_camera::Result::Success) {
        base::LogDebug() << "open qcom camera success";
    }

    if (options.init_mode == mav_camera::Mode::Photo) {
        _settings[kCameraModeName] = "0";
    } else {
        _settings[kCameraModeName] = "1";
    }

    _mav_camera->subscribe_storage_information(
        [&](mav_camera::Result result, mav_camera::StorageInformation storage_information) {
            std::lock_guard<std::mutex> lock(_storage_information_mutex);
            _current_storage_information = storage_information;
        });

    // init all settings
    auto display_mode = get_camera_display_mode();
    _settings[kCameraDisplayModeName] = display_mode;
    std::string wb_mode = get_whitebalance_mode();
    _settings[kWhitebalanceModeName] = wb_mode;
    // 0 for auto exposure mode
    _settings[kExposureMode] = "0";
    std::string ev_value = get_ev_value();
    _settings[kEVName] = ev_value;
    std::string iso_value = get_iso_value();
    _settings[kISOName] = iso_value;
    std::string shutter_speed_value = get_shutter_speed_value();
    _settings[kShutterSpeedName] = shutter_speed_value;
    _settings[kVideoFormat] = "1";
    std::string video_resolution = get_video_resolution();
    _settings[kVideoResolution] = video_resolution;
    _settings[kMeteringModeName] = "0";

    base::LogDebug() << "Init settings :";
    for (const auto &setting : _settings) {
        base::LogDebug() << "  - " << setting.first << " : " << setting.second;
    }
    return true;
}

void CameraLocalClient::deinit() {
    if (_mav_camera != nullptr) {
        _mav_camera->close();
        delete _mav_camera;
        _mav_camera = nullptr;
    }
    if (_plugin_handle != NULL) {
        dlclose(_plugin_handle);
        _plugin_handle = NULL;
    }

    free_ir_camera();
}

bool CameraLocalClient::set_camera_display_mode(std::string mode) {
    mav_camera::Result result = mav_camera::Result::Unknown;
    if (mode == "0") {
        result = _mav_camera->set_preview_stream_output_type(
            mav_camera::PreivewStreamOutputType::RGBStreamOnly);
    } else if (mode == "1") {
        result = _mav_camera->set_preview_stream_output_type(
            mav_camera::PreivewStreamOutputType::InfraredStreamOnly);
    } else if (mode == "2") {
        result = _mav_camera->set_preview_stream_output_type(
            mav_camera::PreivewStreamOutputType::MixSideBySide);
    } else if (mode == "3") {
        result = _mav_camera->set_preview_stream_output_type(
            mav_camera::PreivewStreamOutputType::MixPIP);
    }
    base::LogDebug() << "set camera display mode to " << mode << " result " << int(result);
    return result == mav_camera::Result::Success;
}

std::string CameraLocalClient::get_camera_display_mode() {
    mav_camera::Result result;
    mav_camera::PreivewStreamOutputType preview_type;
    std::tie(result, preview_type) = _mav_camera->get_preview_stream_output_type();
    if (result == mav_camera::Result::Success) {
        switch (preview_type) {
            case mav_camera::PreivewStreamOutputType::RGBStreamOnly:
                return "0";
                break;
            case mav_camera::PreivewStreamOutputType::InfraredStreamOnly:
                return "1";
                break;
            case mav_camera::PreivewStreamOutputType::MixSideBySide:
                return "2";
                break;
            case mav_camera::PreivewStreamOutputType::MixPIP:
                return "3";
                break;
        }
    }
    return 0;
}

/**
    <option name="Auto" value="0" />
    <option name="Incandescent" value="1" />
    <option name="Sunrise" value="2" />
    <option name="Sunset" value="3" />
    <option name="Sunny" value="4" />
    <option name="Cloudy" value="5" />
    <option name="Fluorescent" value="7" />
*/
bool CameraLocalClient::set_whitebalance_mode(std::string mode) {
    mav_camera::Result result;
    if (mode == "0") {  // Auto
        result = _mav_camera->set_white_balance(mav_camera::kAutoWhitebalanceValue);
    } else if (mode == "1") {  // Daylight
        result = _mav_camera->set_white_balance(5500);
    } else if (mode == "2") {  // Cloudy
        result = _mav_camera->set_white_balance(6500);
    } else if (mode == "3") {  // Shady
        result = _mav_camera->set_white_balance(7500);
    } else if (mode == "4") {  // Incandescent
        result = _mav_camera->set_white_balance(2700);
    } else if (mode == "5") {  // Fluorescent
        result = _mav_camera->set_white_balance(4000);
    }
    base::LogDebug() << "set whitebalance mode to " << mode << " result " << (int)result;

    return result == mav_camera::Result::Success;
}

std::string CameraLocalClient::get_whitebalance_mode() {
    auto [result, value] = _mav_camera->get_white_balance();
    if (result != mav_camera::Result::Success) {
        base::LogError() << "Cannot get whitebalance mode"
                         << convert_camera_result_to_mav_server_result(result);
        return "0";
    }
    if (value == mav_camera::kAutoWhitebalanceValue) {
        return "0";
    } else if (value == 5500) {
        return "1";
    } else if (value == 6500) {
        return "2";
    } else if (value == 7500) {
        return "3";
    } else if (value == 2700) {
        return "4";
    } else if (value == 4000) {
        return "5";
    }
    base::LogWarn() << "invalid white balance value " << value;
    return "0";
}

std::string CameraLocalClient::get_ev_value() {
    auto [result, value] = _mav_camera->get_exposure_value();
    if (result != mav_camera::Result::Success) {
        base::LogError() << "Cannot get exposure value"
                         << convert_camera_result_to_mav_server_result(result);
        return "0.0";
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value;
    std::string ev_value = oss.str();
    return ev_value;
}

std::string CameraLocalClient::get_iso_value() {
    auto [result, value] = _mav_camera->get_iso();
    if (result != mav_camera::Result::Success) {
        base::LogError() << "Cannot get iso value"
                         << convert_camera_result_to_mav_server_result(result);
        return "100";
    }
    return std::to_string(value);
}

std::string CameraLocalClient::get_shutter_speed_value() {
    auto [result, value] = _mav_camera->get_shutter_speed();
    if (result != mav_camera::Result::Success) {
        base::LogDebug() << "Cannot get shutterspeed "
                         << convert_camera_result_to_mav_server_result(result);
        return "0.01";  // default value
    }
    std::size_t pos = value.find('/');
    if (pos != std::string::npos) {
        // Split the string at '/'
        std::string num_str = value.substr(0, pos);
        std::string den_str = value.substr(pos + 1);

        // Convert to float
        float numerator = std::stof(num_str);
        float denominator = std::stof(den_str);

        // Perform the division
        auto convert_result = std::to_string(numerator / denominator);
        base::LogDebug() << "current shutter speed is : " << convert_result;
        return convert_result;
    } else {
        // If there is no '/', assume it's a whole number
        return value;
    }
}

std::string CameraLocalClient::get_video_resolution() {
    auto [result, width, height] = _mav_camera->get_video_resolution();
    if (result != mav_camera::Result::Success) {
        base::LogError() << "Cannot get video resolution "
                         << convert_camera_result_to_mav_server_result(result);
        return "0";
    }
    auto [result2, framerate] = _mav_camera->get_framerate();
    if (result2 != mav_camera::Result::Success) {
        base::LogError() << "Cannot get framerate"
                         << convert_camera_result_to_mav_server_result(result);
        return "0";
    }
    base::LogDebug() << "Current video resolution is " << width << "x" << height << "@"
                     << framerate;
    if (width == 3840 && height == 2160 && framerate == 60) {
        return "0";
    } else if (width == 3840 && height == 2160 && framerate == 30) {
        return "1";
    } else if (width == 1920 && height == 1080 && framerate == 60) {
        return "2";
    } else if (width == 1920 && height == 1080 && framerate == 30) {
        return "3";
    } else {
        base::LogError() << "Not found match resolution : " << width << "x" << height << "@"
                         << framerate;
        return "0";
    }
}

bool CameraLocalClient::set_video_resolution(std::string value) {
    int set_width = 0;
    int set_height = 0;
    int set_framerate = 0;
    if (value == "0") {
        set_width = 3840;
        set_height = 2160;
        set_framerate = 60;
    } else if (value == "1") {
        set_width = 3840;
        set_height = 2160;
        set_framerate = 30;
    } else if (value == "2") {
        set_width = 1920;
        set_height = 1080;
        set_framerate = 60;
    } else if (value == "3") {
        set_width = 1920;
        set_height = 1080;
        set_framerate = 30;
    }
    base::LogDebug() << "Set video resolution to " << set_width << "x" << set_height << "@"
                     << set_framerate;
    auto result = _mav_camera->set_video_resolution(set_width, set_height);
    if (result != mav_camera::Result::Success) {
        base::LogError() << "Failed to set video resolution : " << set_width << "x" << set_height;
    }
    result = _mav_camera->set_framerate(set_framerate);
    if (result != mav_camera::Result::Success) {
        base::LogError() << "Failed to set video framerate : " << set_framerate;
    }
    return result == mav_camera::Result::Success;
}

bool CameraLocalClient::set_metering_mode(std::string value) {
    int32_t metering_mode = std::stoi(value);
    if (metering_mode < 0 || metering_mode > 4) {
        base::LogError() << "Invalid metering mode";
        return false;
    }
    auto result = _mav_camera->set_metering_mode(metering_mode);
    if (result != mav_camera::Result::Success) {
        base::LogError() << "Failed to set metering mode : " << metering_mode;
    }
    return result == mav_camera::Result::Success;
}

bool CameraLocalClient::init_ir_camera() {
    if (_ir_camera != nullptr) {
        return true;
    }
    typedef struct ir_extension_api *(*create_ir_extension_api_fun)();
    _ir_camera_handle = dlopen(IR_CAMERA_LIBRARY, RTLD_NOW);
    if (_ir_camera_handle == NULL) {
        char const *err_str = dlerror();
        base::LogError() << "Load module " << IR_CAMERA_LIBRARY << " failed "
                         << (err_str != NULL ? err_str : "unknown");
        return false;
    }

    create_ir_extension_api_fun create_ir_extension_api =
        (create_ir_extension_api_fun)dlsym(_ir_camera_handle, "create_ir_extension_api");
    if (create_ir_extension_api == NULL) {
        base::LogError() << "Cannot find symbol create_ir_extension_api";
        return false;
    }

    _ir_camera = create_ir_extension_api();
    if (_ir_camera == nullptr) {
        base::LogError() << "Cannot create ir camera instance";
        return false;
    }

    if (_ir_camera->exec(TYPE_CAMERA_INIT) == 0) {
        base::LogInfo() << "Init ir camera success.";
    } else {
        base::LogError() << "Failed to init ir camera.";
        return false;
    }

    uint32_t camera_sn;
    if (_ir_camera->get(TYPE_CAMERA_SN, &camera_sn) == 0) {
        base::LogInfo() << "TYPE_CAMERA_SN: " << camera_sn;
    } else {
        base::LogError() << "Failed to TYPE_CAMERA_SN.";
    }

    uint32_t version;
    if (_ir_camera->get(TYPE_CAMERA_VERSION, &version) == 0) {
        printf("version: %u\n", version);
        uint32_t major = (version >> 24) & 0xFF;
        uint32_t minor = (version >> 16) & 0xFF;
        uint32_t patch = version & 0xFFFF;
        base::LogInfo() << "TYPE_CAMERA_VERSION: " << major << "." << minor << "." << patch;
    } else {
        base::LogError() << "Failed to TYPE_CAMERA_VERSION.";
    }

    uint32_t camera_type;
    if (_ir_camera->get(TYPE_CAMERA_TYPE, &camera_type) == 0) {
        base::LogInfo() << "TYPE_CAMERA_TYPE: 0x00000000 -- 0x" << std::setw(8) << std::setfill('0')
                        << std::hex << std::uppercase << camera_type;
    } else {
        base::LogError() << "Failed to TYPE_CAMERA_TYPE.";
    }

    char camera_pn[64];
    if (_ir_camera->get(TYPE_CAMERA_PN, &camera_pn) == 0) {
        base::LogInfo() << "TYPE_CAMERA_PN: " << camera_pn;
    } else {
        base::LogError() << "Failed to TYPE_CAMERA_PN.";
    }

    base::LogInfo() << "Load ir camera success";
    return true;
}

void CameraLocalClient::free_ir_camera() {
    if (_ir_camera != nullptr) {
        if (_ir_camera->exec(TYPE_CAMERA_CLOSE) == 0) {
            base::LogDebug() << "close ir camera success.";
        } else {
            base::LogDebug() << "Failed to close ir camera.";
        }
        _ir_camera = nullptr;
    }
    if (_ir_camera_handle != NULL) {
        dlclose(_ir_camera_handle);
        _ir_camera_handle = NULL;
    }
}

int CameraLocalClient::get_ir_palette() {
    uint32_t color_mode = 0;
    if (_ir_camera != nullptr) {
        _ir_camera->get(TYPE_CAMERA_COLOR_MODE, &color_mode);
        base::LogDebug() << "Current ir palette is " << color_mode;
    }
    return color_mode;
}

bool CameraLocalClient::set_ir_palette(std::string color_mode) {
    uint32_t convert_mode = std::stoul(color_mode);
    if (_ir_camera != nullptr) {
        auto result = _ir_camera->set(TYPE_CAMERA_COLOR_MODE, &convert_mode);
        return result == 0;
    }
    return false;
}

bool CameraLocalClient::set_ir_FFC(std::string /*ignore*/) {
    if (_ir_camera != nullptr) {
        auto result = _ir_camera->exec(TYPE_CAMERA_FFC);
        return result == 0;
    }
    return false;
}

mavsdk::Camera::Setting CameraLocalClient::build_setting(std::string name, std::string value) {
    mavsdk::Camera::Setting setting;
    setting.setting_id = name;
    setting.option.option_id = value;
    return setting;
}

mavsdk::CameraServer::Result CameraLocalClient::convert_camera_result_to_mav_server_result(
    mav_camera::Result input_result) {
    mavsdk::CameraServer::Result output_result = mavsdk::CameraServer::Result::Unknown;
    switch (input_result) {
        case mav_camera::Result::Success:
            output_result = mavsdk::CameraServer::Result::Success;
            break;
        case mav_camera::Result::Denied:
            output_result = mavsdk::CameraServer::Result::Denied;
            break;
        case mav_camera::Result::Busy:
            output_result = mavsdk::CameraServer::Result::Busy;
            break;
        case mav_camera::Result::Error:
            output_result = mavsdk::CameraServer::Result::Error;
            break;
        case mav_camera::Result::InProgress:
            output_result = mavsdk::CameraServer::Result::InProgress;
            break;
        case mav_camera::Result::NoSystem:
            output_result = mavsdk::CameraServer::Result::NoSystem;
            break;
        case mav_camera::Result::Timeout:
            output_result = mavsdk::CameraServer::Result::Timeout;
            break;
        case mav_camera::Result::Unknown:
            output_result = mavsdk::CameraServer::Result::Unknown;
            break;
        case mav_camera::Result::WrongArgument:
            output_result = mavsdk::CameraServer::Result::WrongArgument;
            break;
    }
    return output_result;
}

}  // namespace mavcam
