#include "camera_impl.h"

#include <dlfcn.h>
#include <unistd.h>

#include "base/log.h"

namespace mav {

const std::string kCameraDisplayModeName = "CAM_DISPLAY_MODE";
const std::string kCameraModeName = "CAM_MODE";
const std::string kWhitebalanceModeName = "CAM_WBMODE";
const std::string kStreamingUrl = "127.0.0.1:8554";

#define QCOM_CAMERA_LIBERAY "libqcom_camera.so"
typedef mav_camera::MavCamera *(*create_qcom_camera_fun)();

CameraImpl::CameraImpl() {
    _current_mode = Camera::Mode::Unknown;

    _total_storage_mib = 2 * 1024 * 1024;
    _available_storage_mib = 1 * 1024 * 1024;
}

CameraImpl::~CameraImpl() {}

Camera::Result CameraImpl::prepare() {
    _plugin_handle = dlopen(QCOM_CAMERA_LIBERAY, RTLD_NOW);
    if (_plugin_handle == NULL) {
        char const *err_str = dlerror();
        base::LogError() << "load module " << QCOM_CAMERA_LIBERAY << " failed "
                         << (err_str != NULL ? err_str : "unknown");
        return Camera::Result::Error;
    }

    create_qcom_camera_fun create_camera_fun =
        (create_qcom_camera_fun)dlsym(_plugin_handle, "create_qcom_camera");
    if (create_camera_fun == NULL) {
        base::LogError() << "cannot find symbol create_qcom_camera";
        dlclose(_plugin_handle);
        _plugin_handle = NULL;
        return Camera::Result::Error;
    }

    _mav_camera = create_camera_fun();
    if (_mav_camera == nullptr) {
        base::LogError() << "cannot create mav camera instance";
        dlclose(_plugin_handle);
        _plugin_handle = NULL;
        return Camera::Result::Error;
    }

    _mav_camera->set_timestamp(1724721206027);

    mav_camera::Options options;
    options.preview_drm_output = false;
    options.preview_v4l2_output = false;
    options.preview_weston_output = true;

    options.preview_width = 1920;
    options.preview_height = 1080;
    options.snapshot_width = 4624;
    options.snapshot_height = 3472;
    options.video_width = 1920;
    options.video_height = 1080;

    options.infrared_camera_path = "/dev/video2";
    options.framerate = 60;
    options.debug_calc_fps = false;
    options.init_mode = mav_camera::Mode::Video;

    mav_camera::Result result = _mav_camera->open(options);
    if (result == mav_camera::Result::Success) {
        base::LogDebug() << "open qcom camera success";
    }

    if (options.init_mode == mav_camera::Mode::Photo) {
        _settings.emplace_back(build_setting(kCameraModeName, "0"));
    } else {
        _settings.emplace_back(build_setting(kCameraModeName, "1"));
    }

    _mav_camera->start_streaming(kStreamingUrl);

    // get all settings
    auto display_mode = get_camera_display_mode();
    _settings.emplace_back(build_setting(kCameraDisplayModeName, display_mode));
    std::string wb_mode = get_whitebalance_mode();
    _settings.emplace_back(build_setting(kWhitebalanceModeName, wb_mode));
    _settings.emplace_back(build_setting("CAM_EXPMODE", "0"));
    _settings.emplace_back(build_setting("CAM_EV", "0"));
    _settings.emplace_back(build_setting("CAM_ISO", "100"));
    _settings.emplace_back(build_setting("CAM_SHUTTERSPD", "0.01"));
    _settings.emplace_back(build_setting("CAM_VIDFMT", "1"));
    _settings.emplace_back(build_setting("CAM_VIDRES", "0"));

    return Camera::Result::Success;
}

Camera::Result CameraImpl::take_photo() {
    _mav_camera->take_photo();
    return Camera::Result::Success;
}

Camera::Result CameraImpl::start_photo_interval(float interval_s) {
    base::LogDebug() << "call start photo interval " << interval_s;
    return Camera::Result::ProtocolUnsupported;
}

Camera::Result CameraImpl::stop_photo_interval() {
    base::LogDebug() << "call stop photo interval";
    return Camera::Result::ProtocolUnsupported;
}

Camera::Result CameraImpl::start_video() {
    base::LogDebug() << "call start video";
    _status.video_on = true;
    _start_video_time = std::chrono::steady_clock::now();
    _mav_camera->start_video();
    return Camera::Result::Success;
}

Camera::Result CameraImpl::stop_video() {
    base::LogDebug() << "call stop video";
    _status.video_on = false;
    _mav_camera->stop_video();
    return Camera::Result::Success;
}

Camera::Result CameraImpl::start_video_streaming(int32_t stream_id) {
    base::LogDebug() << "call start video streaming " << stream_id;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::stop_video_streaming(int32_t stream_id) {
    base::LogDebug() << "call stop video streaming " << stream_id;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::set_mode(Camera::Mode mode) {
    if (_current_mode == mode) {
        // same mode do not change again
        return Camera::Result::Success;
    }

    base::LogDebug() << "call set camera to mode " << mode;
    _current_mode = mode;
    // also need change mode in settings
    if (_current_mode == Camera::Mode::Photo) {
        auto setting = build_setting(kCameraModeName, "0");
        set_setting(setting);

        _mav_camera->set_mode(mav_camera::Mode::Photo);
    } else {
        auto setting = build_setting(kCameraModeName, "1");
        set_setting(setting);

        _mav_camera->set_mode(mav_camera::Mode::Video);
    }

    return Camera::Result::Success;
}

std::pair<Camera::Result, std::vector<Camera::CaptureInfo>> CameraImpl::list_photos(
    Camera::PhotosRange photos_range) {
    base::LogDebug() << "call list_photos " << photos_range;

    return {Camera::Result::ProtocolUnsupported, {}};
}

void CameraImpl::mode_async(const Camera::ModeCallback &callback) {
    base::LogDebug() << "call mode_async";
    callback(_current_mode);
}

Camera::Mode CameraImpl::mode() const {
    return _current_mode;
}

void CameraImpl::information_async(const Camera::InformationCallback &callback) {
    base::LogDebug() << "call information_async";
    callback(information());
}

Camera::Information CameraImpl::information() const {
    Camera::Information out_info;

    mav_camera::Information in_info;
    auto result = _mav_camera->get_information(in_info);
    if (result == mav_camera::Result::Success) {
        out_info.vendor_name = in_info.vendor_name;
        out_info.model_name = in_info.model_name;
        out_info.firmware_version = in_info.firmware_version;
        out_info.focal_length_mm = in_info.focal_length_mm;
        out_info.horizontal_sensor_size_mm = in_info.horizontal_sensor_size_mm;
        out_info.vertical_sensor_size_mm = in_info.vertical_sensor_size_mm;
        out_info.horizontal_resolution_px = in_info.horizontal_resolution_px;
        out_info.vertical_resolution_px = in_info.vertical_resolution_px;
        out_info.lens_id = in_info.lens_id;
        out_info.definition_file_version = in_info.definition_file_version;
        out_info.definition_file_uri = "mftp://definition/" + in_info.definition_file_name;

    } else {
        out_info.vendor_name = "Unknown";
        out_info.model_name = "Unknown";
        out_info.firmware_version = "0.0.0";
        out_info.focal_length_mm = 0;
        out_info.horizontal_sensor_size_mm = 0;
        out_info.vertical_sensor_size_mm = 0;
        out_info.horizontal_resolution_px = 0;
        out_info.vertical_resolution_px = 0;
        out_info.lens_id = 0;
        out_info.definition_file_version = 0;
        out_info.definition_file_uri = "";
    }

    out_info.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::CaptureImage);
    out_info.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::CaptureVideo);
    out_info.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::HasModes);
    out_info.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::HasVideoStream);

    return out_info;
}

void CameraImpl::video_stream_info_async(const Camera::VideoStreamInfoCallback &callback) {
    base::LogDebug() << "call video_stream_info_async";
    callback(video_stream_info());
}

std::vector<Camera::VideoStreamInfo> CameraImpl::video_stream_info() const {
    mav::Camera::VideoStreamInfo normal_video_stream;
    normal_video_stream.stream_id = 1;

    normal_video_stream.settings.frame_rate_hz = 60.0;
    normal_video_stream.settings.horizontal_resolution_pix = 1920;
    normal_video_stream.settings.vertical_resolution_pix = 1080;
    normal_video_stream.settings.bit_rate_b_s = 5000000;
    normal_video_stream.settings.rotation_deg = 0;
    normal_video_stream.settings.uri = "rtsp://169.254.254.1/live";
    normal_video_stream.settings.horizontal_fov_deg = 0;
    normal_video_stream.status = mav::Camera::VideoStreamInfo::VideoStreamStatus::InProgress;
    normal_video_stream.spectrum = mav::Camera::VideoStreamInfo::VideoStreamSpectrum::VisibleLight;

    return {normal_video_stream};
}

void CameraImpl::capture_info_async(const Camera::CaptureInfoCallback &callback) {
    base::LogDebug() << "call capture_info_async";
    _capture_info_callback = callback;
}

void CameraImpl::status_async(const Camera::StatusCallback &callback) {
    // base::LogDebug() << "call status_async";
    callback(status());
}

Camera::Status CameraImpl::status() const {
    // TODO just demo
    _status.available_storage_mib = _available_storage_mib;
    _status.total_storage_mib = _total_storage_mib;
    _status.storage_id = 1;
    _status.storage_status = Camera::Status::StorageStatus::Formatted;
    _status.storage_type = Camera::Status::StorageType::Sd;
    if (_status.video_on) {
        auto current_time = std::chrono::steady_clock::now();
        _status.recording_time_s =
            std::chrono::duration_cast<std::chrono::seconds>(current_time - _start_video_time)
                .count();
    } else {
        _status.recording_time_s = 0;
    }
    return _status;
}

void CameraImpl::current_settings_async(const Camera::CurrentSettingsCallback &callback) {
    base::LogDebug() << "call current_settings_async";
    callback(_settings);
}

void CameraImpl::possible_setting_options_async(
    const Camera::PossibleSettingOptionsCallback &callback) {
    // TODO :)
}

std::vector<Camera::SettingOptions> CameraImpl::possible_setting_options() const {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::set_setting(Camera::Setting setting) {
    base::LogDebug() << "call set " << setting.setting_id << " to value "
                     << setting.option.option_id;
    bool set_success = false;
    //camera mode settings
    if (setting.setting_id == kCameraModeName) {
        if (setting.option.option_id == "0") {
            set_mode(Camera::Mode::Photo);
        } else {
            set_mode(Camera::Mode::Video);
        }
    } else if (setting.setting_id == kCameraDisplayModeName) {
        set_success = set_camera_display_mode(setting.option.option_id);
    } else if (setting.setting_id == kWhitebalanceModeName) {  // whitebalance mode
        set_success = set_whitebalance_mode(setting.option.option_id);
    } else {  // other settings
    }
    if (set_success) {  // update current setting
        for (auto &it : _settings) {
            if (it.setting_id == setting.setting_id) {
                it.option.option_id = setting.option.option_id;
                it.option.option_description = setting.option.option_description;
            }
        }
    }
    return Camera::Result::Success;
}

std::pair<Camera::Result, Camera::Setting> CameraImpl::get_setting(Camera::Setting setting) {
    base::LogDebug() << "call get_setting " << setting.setting_id;
    for (auto &it : _settings) {
        if (it.setting_id == setting.setting_id) {
            setting.option.option_id = it.option.option_id;
            setting.option.option_description = it.option.option_description;
            return {Camera::Result::Success, setting};
        }
    }
    return {Camera::Result::WrongArgument, setting};
}

Camera::Result CameraImpl::format_storage(int32_t storage_id) {
    _available_storage_mib = _total_storage_mib.load();
    base::LogDebug() << "call format storage " << storage_id;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::select_camera(int32_t camera_id) {
    base::LogDebug() << "call select_camera";
    return Camera::Result::ProtocolUnsupported;
}

Camera::Result CameraImpl::reset_settings() {
    base::LogDebug() << "call reset settings";
    // reset all value to default value
    set_mode(Camera::Mode::Photo);
    set_camera_display_mode("0");
    set_setting(build_setting(kCameraDisplayModeName, "0"));

    set_whitebalance_mode("0");
    set_setting(build_setting(kWhitebalanceModeName, "0"));

    set_setting(build_setting("CAM_EXPMODE", "0"));
    set_setting(build_setting("CAM_EV", "0"));
    set_setting(build_setting("CAM_ISO", "100"));
    set_setting(build_setting("CAM_SHUTTERSPD", "0.01"));
    set_setting(build_setting("CAM_VIDFMT", "1"));
    set_setting(build_setting("CAM_VIDRES", "0"));
    return Camera::Result::Success;
}

Camera::Result CameraImpl::set_definition_data(std::string definition_data) {
    base::LogDebug() << "call set_definition_data";
    return Camera::Result::ProtocolUnsupported;
}

Camera::Setting CameraImpl::build_setting(std::string name, std::string value) {
    Camera::Setting setting;
    setting.setting_id = name;
    setting.option.option_id = value;
    return setting;
}

bool CameraImpl::set_camera_display_mode(std::string mode) {
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

std::string CameraImpl::get_camera_display_mode() {
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
bool CameraImpl::set_whitebalance_mode(std::string mode) {
    mav_camera::Result result;
    if (mode == "0") {
        result = _mav_camera->set_white_balance(mav_camera::kAutoWhitebalanceValue);
    } else if (mode == "1") {
        result = _mav_camera->set_white_balance(2200);
    } else if (mode == "2") {
        result = _mav_camera->set_white_balance(2500);
    } else if (mode == "3") {
        result = _mav_camera->set_white_balance(3000);
    } else if (mode == "4") {
        result = _mav_camera->set_white_balance(5200);
    } else if (mode == "5") {
        result = _mav_camera->set_white_balance(6200);
    } else if (mode == "7") {
        result = _mav_camera->set_white_balance(4000);
    }
    base::LogDebug() << "set whitebalance mode to " << mode << " result " << (int)result;

    return result == mav_camera::Result::Success;
}

std::string CameraImpl::get_whitebalance_mode() {
    auto [result, value] = _mav_camera->get_white_balance();
    if (result != mav_camera::Result::Success) {
        return "0";
    }
    if (value == mav_camera::kAutoWhitebalanceValue) {
        return "0";
    } else if (value == 2200) {
        return "1";
    } else if (value == 2500) {
        return "2";
    } else if (value == 3000) {
        return "3";
    } else if (value == 5200) {
        return "4";
    } else if (value == 6200) {
        return "5";
    } else if (value == 4000) {
        return "7";
    }
    base::LogWarn() << "invalid white balance value " << value;
    return "0";
}

}  // namespace mav