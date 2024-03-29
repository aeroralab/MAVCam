#include "camera_impl.h"

#include "base/log.h"

namespace mav {

CameraImpl::CameraImpl() {
    _current_mode = Camera::Mode::Photo;

    // TODO just demo for settings
    // CAM_MODE is define in definition file, photo mode as 0, video mode as 1
    if (_current_mode == Camera::Mode::Photo) {
        _settings.emplace_back(build_setting("CAM_MODE", "0"));
    } else {
        _settings.emplace_back(build_setting("CAM_MODE", "1"));
    }
    _settings.emplace_back(build_setting("CAM_WBMODE", "1"));
    _settings.emplace_back(build_setting("CAM_EXPMODE", "0"));
    _settings.emplace_back(build_setting("CAM_EV", "0"));
    _settings.emplace_back(build_setting("CAM_ISO", "100"));
    _settings.emplace_back(build_setting("CAM_SHUTTERSPD", "0.01"));
    _settings.emplace_back(build_setting("CAM_VIDFMT", "1"));
    _settings.emplace_back(build_setting("CAM_VIDRES", "0"));
    _settings.emplace_back(build_setting("CAM_PHOTORATIO", "1"));

    _total_storage_mib = 2 * 1024 * 1024;
    _available_storage_mib = 1 * 1024 * 1024;
}

CameraImpl::~CameraImpl() {}

Camera::Result CameraImpl::prepare() {
    base::LogDebug() << "call prepare";
    return Camera::Result::Success;
}

Camera::Result CameraImpl::take_photo() {
    base::LogDebug() << "call take photo";
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
    return Camera::Result::Success;
}

Camera::Result CameraImpl::stop_video() {
    base::LogDebug() << "call stop video";
    _status.video_on = false;
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
    base::LogDebug() << "call set mode " << mode;
    _current_mode = mode;
    // also need change mode in settings
    if (_current_mode == Camera::Mode::Photo) {
        auto setting = build_setting("CAM_MODE", "0");
        set_setting(setting);
    } else {
        auto setting = build_setting("CAM_MODE", "1");
        set_setting(setting);
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
    Camera::Information information;
    information.vendor_name = "GoerLabs";
    information.model_name = "C10";
    information.firmware_version = "0.0.1";
    information.focal_length_mm = 3.0;
    information.horizontal_sensor_size_mm = 3.68;
    information.vertical_sensor_size_mm = 2.76;
    information.horizontal_resolution_px = 3280;
    information.vertical_resolution_px = 2464;
    information.lens_id = 0;
    information.definition_file_version = 1;
    information.definition_file_uri = "mftp://definition/C10.xml";  // TODO just demo
    information.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::CaptureImage);
    information.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::CaptureVideo);
    information.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::HasModes);
    information.camera_cap_flags.emplace_back(Camera::Information::CameraCapFlags::HasVideoStream);

    return information;
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
    normal_video_stream.settings.bit_rate_b_s = 4 * 1024 * 1024;
    normal_video_stream.settings.rotation_deg = 0;
    normal_video_stream.settings.uri = "rtsp://10.0.0.11/live";
    normal_video_stream.settings.horizontal_fov_deg = 0;
    normal_video_stream.status = mav::Camera::VideoStreamInfo::VideoStreamStatus::InProgress;
    normal_video_stream.spectrum = mav::Camera::VideoStreamInfo::VideoStreamSpectrum::VisibleLight;

    mav::Camera::VideoStreamInfo infrared_video_stream;
    infrared_video_stream.stream_id = 2;

    infrared_video_stream.settings.frame_rate_hz = 24.0;
    infrared_video_stream.settings.horizontal_resolution_pix = 1280;
    infrared_video_stream.settings.vertical_resolution_pix = 720;
    infrared_video_stream.settings.bit_rate_b_s = 4 * 1024;
    infrared_video_stream.settings.rotation_deg = 0;
    infrared_video_stream.settings.uri = "rtsp://10.0.0.11/live2";
    infrared_video_stream.settings.horizontal_fov_deg = 0;
    infrared_video_stream.status = mav::Camera::VideoStreamInfo::VideoStreamStatus::InProgress;
    infrared_video_stream.spectrum = mav::Camera::VideoStreamInfo::VideoStreamSpectrum::Infrared;

    return {normal_video_stream, infrared_video_stream};
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
    for (auto &it : _settings) {
        if (it.setting_id == setting.setting_id) {
            it.option.option_id = setting.option.option_id;
            it.option.option_description = setting.option.option_description;
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
    _current_mode = Camera::Mode::Photo;
    _settings.clear();
    _settings.emplace_back(build_setting("CAM_MODE", "0"));
    _settings.emplace_back(build_setting("CAM_WBMODE", "0"));
    _settings.emplace_back(build_setting("CAM_EXPMODE", "0"));
    _settings.emplace_back(build_setting("CAM_EV", "0"));
    _settings.emplace_back(build_setting("CAM_ISO", "100"));
    _settings.emplace_back(build_setting("CAM_SHUTTERSPD", "0.01"));
    _settings.emplace_back(build_setting("CAM_VIDFMT", "1"));
    _settings.emplace_back(build_setting("CAM_VIDRES", "0"));
    _settings.emplace_back(build_setting("CAM_PHOTORATIO", "1"));
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

}  // namespace mav