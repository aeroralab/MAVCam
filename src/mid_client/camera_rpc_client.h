#pragma once

#include "camera_client.h"

namespace mid {

class CameraRpcClient : public CameraClient {
public:
    CameraRpcClient() {}
    virtual ~CameraRpcClient() {}
public:
    virtual mavsdk::CameraServer::Result take_photo(int index);
    virtual mavsdk::CameraServer::Result start_video();
    virtual mavsdk::CameraServer::Result stop_video();
    virtual mavsdk::CameraServer::Result start_video_streaming(int stream_id);
    virtual mavsdk::CameraServer::Result stop_video_streaming(int stream_id);
    virtual mavsdk::CameraServer::Result set_mode(mavsdk::CameraServer::Mode mode);
    virtual mavsdk::CameraServer::Result format_storage(int storage_id);
    virtual mavsdk::CameraServer::Result reset_settings();
public:
    virtual mavsdk::CameraServer::Result fill_information(
        mavsdk::CameraServer::Information &information);
    virtual mavsdk::CameraServer::Result fill_storage_information(
        mavsdk::CameraServer::StorageInformation &storage_information);
    virtual mavsdk::CameraServer::Result fill_capture_status(
        mavsdk::CameraServer::CaptureStatus &capture_status);
public:
    bool Init(int rpc_port) const;
};

}  // namespace mid