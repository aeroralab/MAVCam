#include "plugins/camera/camera_impl.h"

#include <chrono>

#include "base/log.h"

namespace mid {

CameraImpl::CameraImpl() {
    start();

    // TODO just demo for settings
    _settings.emplace_back(build_setting("CAM_MODE", "1"));
    _settings.emplace_back(build_setting("CAM_WBMODE", "0"));
    _settings.emplace_back(build_setting("CAM_EXPMODE", "0"));
    _settings.emplace_back(build_setting("CAM_EV", "0"));
    _settings.emplace_back(build_setting("CAM_ISO", "100"));
    _settings.emplace_back(build_setting("CAM_SHUTTERSPD", "0.01"));
    _settings.emplace_back(build_setting("CAM_VIDFMT", "1"));
    _settings.emplace_back(build_setting("CAM_VIDRES", "0"));
    _settings.emplace_back(build_setting("CAM_PHOTORATIO", "1"));
}

CameraImpl::~CameraImpl() {
    stop();
}

Camera::Result CameraImpl::prepare() const {
    LogDebug() << "call prepare";
    return Camera::Result::Success;
}

Camera::Result CameraImpl::take_photo() const {
    LogDebug() << "call take photo";
    return Camera::Result::Success;
}

Camera::Result CameraImpl::start_photo_interval(float interval_s) const {
    LogDebug() << "call start photo interval " << interval_s;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::stop_photo_interval() const {
    LogDebug() << "call stop photo interval";
    return Camera::Result::Success;
}

Camera::Result CameraImpl::start_video() const {
    LogDebug() << "call start video";
    _status.video_on = true;
    _start_video_time = std::chrono::steady_clock::now();
    return Camera::Result::Success;
}

Camera::Result CameraImpl::stop_video() const {
    LogDebug() << "call stop video";
    _status.video_on = false;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::start_video_streaming(int32_t stream_id) const {
    LogDebug() << "call start video streaming " << stream_id;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::stop_video_streaming(int32_t stream_id) const {
    LogDebug() << "call stop video streaming " << stream_id;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::set_mode(Camera::Mode mode) const {
    LogDebug() << "call set mode " << mode;
    return Camera::Result::Success;
}

std::pair<Camera::Result, std::vector<Camera::CaptureInfo>> CameraImpl::list_photos(
    Camera::PhotosRange photos_range) const {
    LogWarn() << "unsupport function";
}

void CameraImpl::subscribe_mode(const Camera::ModeCallback &callback) {
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _camera_mode_callback = callback;
}

Camera::Mode CameraImpl::mode() const {
    return Camera::Mode::Unknown;
}

void CameraImpl::subscribe_information(const Camera::InformationCallback &callback) {
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _need_update_camera_information = true;
    _camera_information_callback = callback;
}

Camera::Information CameraImpl::information() const {
    return {
        // clang-format off
        .vendor_name = "GoerLabs",
        .model_name = "C10",
         .firmware_version = "0.0.1",
        .focal_length_mm = 3.0,
        .horizontal_sensor_size_mm = 3.68,
        .vertical_sensor_size_mm = 2.76,
        .horizontal_resolution_px = 3280,
        .vertical_resolution_px = 2464,
        .lens_id = 0,
        .definition_file_version = 1,
        .definition_file_uri = "ftp://C10.xml",  //TODO just demo
        // clang-format on
    };
}

void CameraImpl::subscribe_video_stream_info(const Camera::VideoStreamInfoCallback &callback) {}

Camera::VideoStreamInfo CameraImpl::video_stream_info() const {}

void CameraImpl::subscribe_capture_info(const Camera::CaptureInfoCallback &callback) {}

void CameraImpl::subscribe_status(const Camera::StatusCallback &callback) {
    std::lock_guard<std::mutex> lock(_callback_mutex);
    _status_callback = callback;
}

Camera::Status CameraImpl::status() const {
    //TODO just demo
    _status.available_storage_mib = 1024 * 1024;
    _status.total_storage_mib = 4 * 1024 * 1024;
    _status.storage_id = 1;
    _status.storage_status = Status::StorageStatus::Formatted;
    _status.storage_type = Status::StorageType::Sd;
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

void CameraImpl::subscribe_current_settings(const Camera::CurrentSettingsCallback &callback) {
    callback(_settings);
}

void CameraImpl::subscribe_possible_setting_options(
    const Camera::PossibleSettingOptionsCallback &callback) {}

std::vector<Camera::SettingOptions> CameraImpl::possible_setting_options() const {}

Camera::Result CameraImpl::set_setting(Setting setting) const {
    LogDebug() << "call set " << setting.setting_id << " to value " << setting.option.option_id;
    for (auto &it : _settings) {
        if (it.setting_id == setting.setting_id) {
            it.option.option_id = setting.option.option_id;
            it.option.option_description = setting.option.option_description;
        }
    }
    return Camera::Result::Success;
}

std::pair<Camera::Result, Camera::Setting> CameraImpl::get_setting(Camera::Setting setting) const {
    for (auto &it : _settings) {
        if (it.setting_id == setting.setting_id) {
            setting.option.option_id = it.option.option_id;
            setting.option.option_description = it.option.option_description;
            return {Camera::Result::Success, setting};
        }
    }
    return {Camera::Result::WrongArgument, setting};
}

Camera::Result CameraImpl::format_storage(int32_t storage_id) const {
    LogDebug() << "call format storage " << storage_id;
    return Camera::Result::Success;
}

Camera::Result CameraImpl::select_camera(int32_t camera_id) const {
    LogWarn() << "unsupport function";
    return Camera::Result::ProtocolUnsupported;
}

Camera::Result CameraImpl::reset_settings() const {
    LogDebug() << "call reset settings";
    return Camera::Result::Success;
}

Camera::Result CameraImpl::set_definition_data(std::string definition_data) const {
    LogWarn() << "unsupport function";
    return Camera::Result::ProtocolUnsupported;
}

void CameraImpl::start() {
    _should_exit = false;
    _work_thread = new std::thread(work_thread, this);
}

void CameraImpl::stop() {
    _should_exit = true;
    if (_work_thread != nullptr) {
        _work_thread->join();
        delete _work_thread;
        _work_thread = nullptr;
    }
}

void CameraImpl::work_thread(CameraImpl *self) {
    while (!self->_should_exit) {
        {
            std::lock_guard<std::mutex> lock(self->_callback_mutex);
            if (self->_need_update_camera_information) {
                LogDebug() << "retrive camera information";
                self->_camera_information_callback(self->information());
                self->_need_update_camera_information = false;
            }
            // send status
            if (self->_status_callback != nullptr) {
                self->_status_callback(self->status());
                self->_status_callback = nullptr;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
}

Camera::Setting CameraImpl::build_setting(std::string name, std::string value) {
    Camera::Setting setting;
    setting.setting_id = name;
    setting.option.option_id = value;
    return setting;
}

}  // namespace mid