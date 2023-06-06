#include "camera_rpc_client.h"

namespace mid {

bool CameraRpcClient::Init(int rpc_port) const {
    return true;
}

mavsdk::CameraServer::Result CameraRpcClient::take_photo(int index) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::start_video() {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video() {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::start_video_streaming(int stream_id) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video_streaming(int stream_id) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::set_mode(mavsdk::CameraServer::Mode mode) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::format_storage(int storage_id) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::reset_settings() {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::fill_information(
    mavsdk::CameraServer::Information &information) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::fill_storage_information(
    mavsdk::CameraServer::StorageInformation &storage_information) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::fill_capture_status(
    mavsdk::CameraServer::CaptureStatus &capture_status) {
    return mavsdk::CameraServer::Result();
}

}  // namespace mid