#include "camera_local_client.h"

#include "base/log.h"

namespace mid {

CameraLocalClient::CameraLocalClient() {
    _is_capture_in_progress = false;
    _image_count = 0;
    _is_recording_video = false;

    // TODO just demo for settings
    _settings["CAM_MODE"] = "1";
    _settings["CAM_WBMODE"] = "0";
    _settings["CAM_EXPMODE"] = "0";
    _settings["CAM_EV"] = "0";
    _settings["CAM_ISO"] = "100";
    _settings["CAM_SHUTTERSPD"] = "0.01";
    _settings["CAM_VIDFMT"] = "1";
    _settings["CAM_VIDRES"] = "0";
    _settings["CAM_PHOTORATIO"] = "1";
}

CameraLocalClient::~CameraLocalClient() {}

mavsdk::CameraServer::Result CameraLocalClient::take_photo(int index) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call take photo " << index;
    _is_capture_in_progress = true;
    auto result = mavsdk::CameraServer::Result::Success;
    _is_capture_in_progress = false;
    _image_count++;
    return result;
}

mavsdk::CameraServer::Result CameraLocalClient::start_video() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call start video";
    _is_recording_video = true;
    _start_video_time = std::chrono::steady_clock::now();
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::stop_video() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call stop video";
    _is_recording_video = false;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::start_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call start video streaming";
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::stop_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call stop video streaming";
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::set_mode(mavsdk::CameraServer::Mode mode) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call set mode" << mode;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::format_storage(int storage_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call format storage " << storage_id;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::reset_settings() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "locally call reset settings";
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::fill_information(
    mavsdk::CameraServer::Information &information) {
    information.vendor_name = "GoerLabs";
    information.model_name = "Simple Model";
    information.firmware_version = "0.0.1";
    information.focal_length_mm = 3.0;
    information.horizontal_sensor_size_mm = 3.68;
    information.vertical_sensor_size_mm = 2.76;
    information.horizontal_resolution_px = 3280;
    information.vertical_resolution_px = 2464;
    information.lens_id = 0;
    information.definition_file_version = 1;
    information.definition_file_uri = "ftp://C10.xml";  //TODO just demo
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::fill_storage_information(
    mavsdk::CameraServer::StorageInformation &storage_information) {
    constexpr int kTotalStorage = 4 * 1024 * 1024;
    storage_information.total_storage_mib = kTotalStorage;
    storage_information.used_storage_mib = 100;
    storage_information.available_storage_mib =
        kTotalStorage - storage_information.used_storage_mib;
    storage_information.storage_status =
        mavsdk::CameraServer::StorageInformation::StorageStatus::Formatted;
    storage_information.storage_type =
        mavsdk::CameraServer::StorageInformation::StorageType::Microsd;
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::fill_capture_status(
    mavsdk::CameraServer::CaptureStatus &capture_status) {
    // not need lock guard
    capture_status.image_count = _image_count;
    capture_status.image_status =
        _is_capture_in_progress
            ? mavsdk::CameraServer::CaptureStatus::ImageStatus::CaptureInProgress
            : mavsdk::CameraServer::CaptureStatus::ImageStatus::Idle;
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

mavsdk::CameraServer::Result CameraLocalClient::retrieve_current_settings(
    std::vector<mavsdk::Camera::Setting> &settings) {
    for (auto &it : _settings) {
        settings.emplace_back(build_setting(it.first, it.second));
    }

    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraLocalClient::set_setting(mavsdk::Camera::Setting setting) {
    LogDebug() << "change " << setting.setting_id << " to " << setting.option.option_id;
    if (_settings.count(setting.setting_id) == 0) {
        LogError() << "Unsupport setting " << setting.setting_id;
        return mavsdk::CameraServer::Result::WrongArgument;
    }
    _settings[setting.setting_id] = setting.option.option_id;
    return mavsdk::CameraServer::Result::Success;
}

std::pair<mavsdk::CameraServer::Result, mavsdk::Camera::Setting> CameraLocalClient::get_setting(
    mavsdk::Camera::Setting setting) const {
    if (_settings.count(setting.setting_id) == 0) {
        return {mavsdk::CameraServer::Result::WrongArgument, setting};
    }
    mavsdk::Camera::Setting out_setting = setting;
    out_setting.option.option_id = _settings[setting.setting_id];
    return {mavsdk::CameraServer::Result::Success, out_setting};
}

mavsdk::Camera::Setting CameraLocalClient::build_setting(std::string name, std::string value) {
    mavsdk::Camera::Setting setting;
    setting.setting_id = name;
    setting.option.option_id = value;
    return setting;
}

}  // namespace mid