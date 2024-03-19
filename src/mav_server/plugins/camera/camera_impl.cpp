#include "camera_impl.h"

namespace mav {

CameraImpl::CameraImpl() {}

CameraImpl::~CameraImpl() {}

Camera::Result CameraImpl::prepare() {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::take_photo() {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::start_photo_interval(float interval_s) {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::stop_photo_interval() {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::start_video() {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::stop_video() {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::start_video_streaming(int32_t stream_id) {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::stop_video_streaming(int32_t stream_id) {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::set_mode(Camera::Mode mode) {
    // TODO :)
    return {};
}

std::pair<Camera::Result, std::vector<Camera::CaptureInfo>> CameraImpl::list_photos(
    Camera::PhotosRange photos_range) {
    // TODO :)
    return {};
}

Camera::Mode CameraImpl::mode() const {
    // TODO :)
    return {};
}

Camera::Information CameraImpl::information() const {
    // TODO :)
    return {};
}

std::vector<Camera::VideoStreamInfo> CameraImpl::video_stream_info() const {
    // TODO :)
    return {};
}

Camera::Status CameraImpl::status() const {
    // TODO :)
    return {};
}

std::vector<Camera::SettingOptions> CameraImpl::possible_setting_options() const {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::set_setting(Camera::Setting setting) {
    // TODO :)
    return {};
}

std::pair<Camera::Result, Camera::Setting> CameraImpl::get_setting(Camera::Setting setting) {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::format_storage(int32_t storage_id) {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::select_camera(int32_t camera_id) {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::reset_settings() {
    // TODO :)
    return {};
}

Camera::Result CameraImpl::set_definition_data(std::string definition_data) {
    // TODO :)
    return {};
}

}  // namespace mav