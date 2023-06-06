#pragma once

#include <mavsdk/plugins/camera_server/camera_server.h>
namespace mid {

class CameraClient {
public:
    virtual ~CameraClient() {}
protected:
    CameraClient() {}
public:  // operation
    virtual mavsdk::CameraServer::Result take_photo(int index) = 0;
    virtual mavsdk::CameraServer::Result start_video() = 0;
    virtual mavsdk::CameraServer::Result stop_video() = 0;
    virtual mavsdk::CameraServer::Result start_video_streaming(int stream_id) = 0;
    virtual mavsdk::CameraServer::Result stop_video_streaming(int stream_id) = 0;
    virtual mavsdk::CameraServer::Result set_mode(mavsdk::CameraServer::Mode mode) = 0;
    virtual mavsdk::CameraServer::Result format_storage(int storage_id) = 0;
    virtual mavsdk::CameraServer::Result reset_settings() = 0;
public:  //subscribe
    virtual mavsdk::CameraServer::Result fill_information(
        mavsdk::CameraServer::Information &information) = 0;
    virtual mavsdk::CameraServer::Result fill_storage_information(
        mavsdk::CameraServer::StorageInformation &storage_information) = 0;
    virtual mavsdk::CameraServer::Result fill_capture_status(
        mavsdk::CameraServer::CaptureStatus &capture_status) = 0;
public:  //settings
};

CameraClient *CreateLocalCameraClient();
CameraClient *CreateRpcCameraClient(int rpc_port);
}  // namespace mid