#include "plugins/camera/camera.h"

#include <iomanip>

namespace mid {
using Position = Camera::Position;
using Quaternion = Camera::Quaternion;
using EulerAngle = Camera::EulerAngle;
using CaptureInfo = Camera::CaptureInfo;
using VideoStreamSettings = Camera::VideoStreamSettings;
using VideoStreamInfo = Camera::VideoStreamInfo;
using Status = Camera::Status;
using Option = Camera::Option;
using Setting = Camera::Setting;
using SettingOptions = Camera::SettingOptions;
using Information = Camera::Information;

Camera::Camera() {}

Camera::~Camera() {}

std::ostream &operator<<(std::ostream &str, Camera::Mode const &mode) {
    switch (mode) {
        case Camera::Mode::Unknown:
            return str << "Unknown";
        case Camera::Mode::Photo:
            return str << "Photo";
        case Camera::Mode::Video:
            return str << "Video";
        default:
            return str << "Unknown";
    }
}

std::ostream &operator<<(std::ostream &str, Camera::PhotosRange const &photos_range) {
    switch (photos_range) {
        case Camera::PhotosRange::All:
            return str << "All";
        case Camera::PhotosRange::SinceConnection:
            return str << "Since Connection";
        default:
            return str << "Unknown";
    }
}

std::ostream &operator<<(std::ostream &str, Camera::Result const &result) {
    switch (result) {
        case Camera::Result::Unknown:
            return str << "Unknown";
        case Camera::Result::Success:
            return str << "Success";
        case Camera::Result::InProgress:
            return str << "In Progress";
        case Camera::Result::Busy:
            return str << "Busy";
        case Camera::Result::Denied:
            return str << "Denied";
        case Camera::Result::Error:
            return str << "Error";
        case Camera::Result::Timeout:
            return str << "Timeout";
        case Camera::Result::WrongArgument:
            return str << "Wrong Argument";
        case Camera::Result::NoSystem:
            return str << "No System";
        case Camera::Result::ProtocolUnsupported:
            return str << "Protocol Unsupported";
        default:
            return str << "Unknown";
    }
}

bool operator==(const Camera::Position &lhs, const Camera::Position &rhs) {
    return ((std::isnan(rhs.latitude_deg) && std::isnan(lhs.latitude_deg)) ||
            rhs.latitude_deg == lhs.latitude_deg) &&
           ((std::isnan(rhs.longitude_deg) && std::isnan(lhs.longitude_deg)) ||
            rhs.longitude_deg == lhs.longitude_deg) &&
           ((std::isnan(rhs.absolute_altitude_m) && std::isnan(lhs.absolute_altitude_m)) ||
            rhs.absolute_altitude_m == lhs.absolute_altitude_m) &&
           ((std::isnan(rhs.relative_altitude_m) && std::isnan(lhs.relative_altitude_m)) ||
            rhs.relative_altitude_m == lhs.relative_altitude_m);
}

std::ostream &operator<<(std::ostream &str, Camera::Position const &position) {
    str << std::setprecision(15);
    str << "position:" << '\n' << "{\n";
    str << "    latitude_deg: " << position.latitude_deg << '\n';
    str << "    longitude_deg: " << position.longitude_deg << '\n';
    str << "    absolute_altitude_m: " << position.absolute_altitude_m << '\n';
    str << "    relative_altitude_m: " << position.relative_altitude_m << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::Quaternion &lhs, const Camera::Quaternion &rhs) {
    return ((std::isnan(rhs.w) && std::isnan(lhs.w)) || rhs.w == lhs.w) &&
           ((std::isnan(rhs.x) && std::isnan(lhs.x)) || rhs.x == lhs.x) &&
           ((std::isnan(rhs.y) && std::isnan(lhs.y)) || rhs.y == lhs.y) &&
           ((std::isnan(rhs.z) && std::isnan(lhs.z)) || rhs.z == lhs.z);
}

std::ostream &operator<<(std::ostream &str, Camera::Quaternion const &quaternion) {
    str << std::setprecision(15);
    str << "quaternion:" << '\n' << "{\n";
    str << "    w: " << quaternion.w << '\n';
    str << "    x: " << quaternion.x << '\n';
    str << "    y: " << quaternion.y << '\n';
    str << "    z: " << quaternion.z << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::EulerAngle &lhs, const Camera::EulerAngle &rhs) {
    return ((std::isnan(rhs.roll_deg) && std::isnan(lhs.roll_deg)) ||
            rhs.roll_deg == lhs.roll_deg) &&
           ((std::isnan(rhs.pitch_deg) && std::isnan(lhs.pitch_deg)) ||
            rhs.pitch_deg == lhs.pitch_deg) &&
           ((std::isnan(rhs.yaw_deg) && std::isnan(lhs.yaw_deg)) || rhs.yaw_deg == lhs.yaw_deg);
}

std::ostream &operator<<(std::ostream &str, Camera::EulerAngle const &euler_angle) {
    str << std::setprecision(15);
    str << "euler_angle:" << '\n' << "{\n";
    str << "    roll_deg: " << euler_angle.roll_deg << '\n';
    str << "    pitch_deg: " << euler_angle.pitch_deg << '\n';
    str << "    yaw_deg: " << euler_angle.yaw_deg << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::CaptureInfo &lhs, const Camera::CaptureInfo &rhs) {
    return (rhs.position == lhs.position) && (rhs.attitude_quaternion == lhs.attitude_quaternion) &&
           (rhs.attitude_euler_angle == lhs.attitude_euler_angle) &&
           (rhs.time_utc_us == lhs.time_utc_us) && (rhs.is_success == lhs.is_success) &&
           (rhs.index == lhs.index) && (rhs.file_url == lhs.file_url);
}

std::ostream &operator<<(std::ostream &str, Camera::CaptureInfo const &capture_info) {
    str << std::setprecision(15);
    str << "capture_info:" << '\n' << "{\n";
    str << "    position: " << capture_info.position << '\n';
    str << "    attitude_quaternion: " << capture_info.attitude_quaternion << '\n';
    str << "    attitude_euler_angle: " << capture_info.attitude_euler_angle << '\n';
    str << "    time_utc_us: " << capture_info.time_utc_us << '\n';
    str << "    is_success: " << capture_info.is_success << '\n';
    str << "    index: " << capture_info.index << '\n';
    str << "    file_url: " << capture_info.file_url << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::VideoStreamSettings &lhs, const Camera::VideoStreamSettings &rhs) {
    return ((std::isnan(rhs.frame_rate_hz) && std::isnan(lhs.frame_rate_hz)) ||
            rhs.frame_rate_hz == lhs.frame_rate_hz) &&
           (rhs.horizontal_resolution_pix == lhs.horizontal_resolution_pix) &&
           (rhs.vertical_resolution_pix == lhs.vertical_resolution_pix) &&
           (rhs.bit_rate_b_s == lhs.bit_rate_b_s) && (rhs.rotation_deg == lhs.rotation_deg) &&
           (rhs.uri == lhs.uri) &&
           ((std::isnan(rhs.horizontal_fov_deg) && std::isnan(lhs.horizontal_fov_deg)) ||
            rhs.horizontal_fov_deg == lhs.horizontal_fov_deg);
}

std::ostream &operator<<(std::ostream &str,
                         Camera::VideoStreamSettings const &video_stream_settings) {
    str << std::setprecision(15);
    str << "video_stream_settings:" << '\n' << "{\n";
    str << "    frame_rate_hz: " << video_stream_settings.frame_rate_hz << '\n';
    str << "    horizontal_resolution_pix: " << video_stream_settings.horizontal_resolution_pix
        << '\n';
    str << "    vertical_resolution_pix: " << video_stream_settings.vertical_resolution_pix << '\n';
    str << "    bit_rate_b_s: " << video_stream_settings.bit_rate_b_s << '\n';
    str << "    rotation_deg: " << video_stream_settings.rotation_deg << '\n';
    str << "    uri: " << video_stream_settings.uri << '\n';
    str << "    horizontal_fov_deg: " << video_stream_settings.horizontal_fov_deg << '\n';
    str << '}';
    return str;
}

std::ostream &operator<<(std::ostream &str,
                         Camera::VideoStreamInfo::VideoStreamStatus const &video_stream_status) {
    switch (video_stream_status) {
        case Camera::VideoStreamInfo::VideoStreamStatus::NotRunning:
            return str << "Not Running";
        case Camera::VideoStreamInfo::VideoStreamStatus::InProgress:
            return str << "In Progress";
        default:
            return str << "Unknown";
    }
}

std::ostream &operator<<(
    std::ostream &str, Camera::VideoStreamInfo::VideoStreamSpectrum const &video_stream_spectrum) {
    switch (video_stream_spectrum) {
        case Camera::VideoStreamInfo::VideoStreamSpectrum::Unknown:
            return str << "Unknown";
        case Camera::VideoStreamInfo::VideoStreamSpectrum::VisibleLight:
            return str << "Visible Light";
        case Camera::VideoStreamInfo::VideoStreamSpectrum::Infrared:
            return str << "Infrared";
        default:
            return str << "Unknown";
    }
}
bool operator==(const Camera::VideoStreamInfo &lhs, const Camera::VideoStreamInfo &rhs) {
    return (rhs.stream_id == lhs.stream_id) && (rhs.settings == lhs.settings) &&
           (rhs.status == lhs.status) && (rhs.spectrum == lhs.spectrum);
}

std::ostream &operator<<(std::ostream &str, Camera::VideoStreamInfo const &video_stream_info) {
    str << std::setprecision(15);
    str << "video_stream_info:" << '\n' << "{\n";
    str << "    stream_id: " << video_stream_info.stream_id << '\n';
    str << "    settings: " << video_stream_info.settings << '\n';
    str << "    status: " << video_stream_info.status << '\n';
    str << "    spectrum: " << video_stream_info.spectrum << '\n';
    str << '}';
    return str;
}

std::ostream &operator<<(std::ostream &str, Camera::Status::StorageStatus const &storage_status) {
    switch (storage_status) {
        case Camera::Status::StorageStatus::NotAvailable:
            return str << "Not Available";
        case Camera::Status::StorageStatus::Unformatted:
            return str << "Unformatted";
        case Camera::Status::StorageStatus::Formatted:
            return str << "Formatted";
        case Camera::Status::StorageStatus::NotSupported:
            return str << "Not Supported";
        default:
            return str << "Unknown";
    }
}

std::ostream &operator<<(std::ostream &str, Camera::Status::StorageType const &storage_type) {
    switch (storage_type) {
        case Camera::Status::StorageType::Unknown:
            return str << "Unknown";
        case Camera::Status::StorageType::UsbStick:
            return str << "Usb Stick";
        case Camera::Status::StorageType::Sd:
            return str << "Sd";
        case Camera::Status::StorageType::Microsd:
            return str << "Microsd";
        case Camera::Status::StorageType::Hd:
            return str << "Hd";
        case Camera::Status::StorageType::Other:
            return str << "Other";
        default:
            return str << "Unknown";
    }
}
bool operator==(const Camera::Status &lhs, const Camera::Status &rhs) {
    return (rhs.video_on == lhs.video_on) && (rhs.photo_interval_on == lhs.photo_interval_on) &&
           ((std::isnan(rhs.used_storage_mib) && std::isnan(lhs.used_storage_mib)) ||
            rhs.used_storage_mib == lhs.used_storage_mib) &&
           ((std::isnan(rhs.available_storage_mib) && std::isnan(lhs.available_storage_mib)) ||
            rhs.available_storage_mib == lhs.available_storage_mib) &&
           ((std::isnan(rhs.total_storage_mib) && std::isnan(lhs.total_storage_mib)) ||
            rhs.total_storage_mib == lhs.total_storage_mib) &&
           ((std::isnan(rhs.recording_time_s) && std::isnan(lhs.recording_time_s)) ||
            rhs.recording_time_s == lhs.recording_time_s) &&
           (rhs.media_folder_name == lhs.media_folder_name) &&
           (rhs.storage_status == lhs.storage_status) && (rhs.storage_id == lhs.storage_id) &&
           (rhs.storage_type == lhs.storage_type);
}

std::ostream &operator<<(std::ostream &str, Camera::Status const &status) {
    str << std::setprecision(15);
    str << "status:" << '\n' << "{\n";
    str << "    video_on: " << status.video_on << '\n';
    str << "    photo_interval_on: " << status.photo_interval_on << '\n';
    str << "    used_storage_mib: " << status.used_storage_mib << '\n';
    str << "    available_storage_mib: " << status.available_storage_mib << '\n';
    str << "    total_storage_mib: " << status.total_storage_mib << '\n';
    str << "    recording_time_s: " << status.recording_time_s << '\n';
    str << "    media_folder_name: " << status.media_folder_name << '\n';
    str << "    storage_status: " << status.storage_status << '\n';
    str << "    storage_id: " << status.storage_id << '\n';
    str << "    storage_type: " << status.storage_type << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::Option &lhs, const Camera::Option &rhs) {
    return (rhs.option_id == lhs.option_id) && (rhs.option_description == lhs.option_description);
}

std::ostream &operator<<(std::ostream &str, Camera::Option const &option) {
    str << std::setprecision(15);
    str << "option:" << '\n' << "{\n";
    str << "    option_id: " << option.option_id << '\n';
    str << "    option_description: " << option.option_description << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::Setting &lhs, const Camera::Setting &rhs) {
    return (rhs.setting_id == lhs.setting_id) &&
           (rhs.setting_description == lhs.setting_description) && (rhs.option == lhs.option) &&
           (rhs.is_range == lhs.is_range);
}

std::ostream &operator<<(std::ostream &str, Camera::Setting const &setting) {
    str << std::setprecision(15);
    str << "setting:" << '\n' << "{\n";
    str << "    setting_id: " << setting.setting_id << '\n';
    str << "    setting_description: " << setting.setting_description << '\n';
    str << "    option: " << setting.option << '\n';
    str << "    is_range: " << setting.is_range << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::SettingOptions &lhs, const Camera::SettingOptions &rhs) {
    return (rhs.setting_id == lhs.setting_id) &&
           (rhs.setting_description == lhs.setting_description) && (rhs.options == lhs.options) &&
           (rhs.is_range == lhs.is_range);
}

std::ostream &operator<<(std::ostream &str, Camera::SettingOptions const &setting_options) {
    str << std::setprecision(15);
    str << "setting_options:" << '\n' << "{\n";
    str << "    setting_id: " << setting_options.setting_id << '\n';
    str << "    setting_description: " << setting_options.setting_description << '\n';
    str << "    options: [";
    for (auto it = setting_options.options.begin(); it != setting_options.options.end(); ++it) {
        str << *it;
        str << (it + 1 != setting_options.options.end() ? ", " : "]\n");
    }
    str << "    is_range: " << setting_options.is_range << '\n';
    str << '}';
    return str;
}

bool operator==(const Camera::Information &lhs, const Camera::Information &rhs) {
    return (rhs.vendor_name == lhs.vendor_name) && (rhs.model_name == lhs.model_name) &&
           ((std::isnan(rhs.focal_length_mm) && std::isnan(lhs.focal_length_mm)) ||
            rhs.focal_length_mm == lhs.focal_length_mm) &&
           ((std::isnan(rhs.horizontal_sensor_size_mm) &&
             std::isnan(lhs.horizontal_sensor_size_mm)) ||
            rhs.horizontal_sensor_size_mm == lhs.horizontal_sensor_size_mm) &&
           ((std::isnan(rhs.vertical_sensor_size_mm) && std::isnan(lhs.vertical_sensor_size_mm)) ||
            rhs.vertical_sensor_size_mm == lhs.vertical_sensor_size_mm) &&
           (rhs.horizontal_resolution_px == lhs.horizontal_resolution_px) &&
           (rhs.vertical_resolution_px == lhs.vertical_resolution_px);
}

std::ostream &operator<<(std::ostream &str, Camera::Information const &information) {
    str << std::setprecision(15);
    str << "information:" << '\n' << "{\n";
    str << "    vendor_name: " << information.vendor_name << '\n';
    str << "    model_name: " << information.model_name << '\n';
    str << "    focal_length_mm: " << information.focal_length_mm << '\n';
    str << "    horizontal_sensor_size_mm: " << information.horizontal_sensor_size_mm << '\n';
    str << "    vertical_sensor_size_mm: " << information.vertical_sensor_size_mm << '\n';
    str << "    horizontal_resolution_px: " << information.horizontal_resolution_px << '\n';
    str << "    vertical_resolution_px: " << information.vertical_resolution_px << '\n';
    str << '}';
    return str;
}

}  // namespace mid