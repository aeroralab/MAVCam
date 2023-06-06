#pragma once

#include <atomic>
#include <chrono>
#include <mutex>

#include "camera_client.h"

namespace mid {

class CameraLocalClient : public CameraClient {
public:
    CameraLocalClient();
    virtual ~CameraLocalClient();
public:  //operation
    virtual mavsdk::CameraServer::Result take_photo(int index);
    virtual mavsdk::CameraServer::Result start_video();
    virtual mavsdk::CameraServer::Result stop_video();
    virtual mavsdk::CameraServer::Result start_video_streaming(int stream_id);
    virtual mavsdk::CameraServer::Result stop_video_streaming(int stream_id);
    virtual mavsdk::CameraServer::Result set_mode(mavsdk::CameraServer::Mode mode);
    virtual mavsdk::CameraServer::Result format_storage(int storage_id);
    virtual mavsdk::CameraServer::Result reset_settings();
public:  //subscribe
    virtual mavsdk::CameraServer::Result fill_information(
        mavsdk::CameraServer::Information &information);
    virtual mavsdk::CameraServer::Result fill_storage_information(
        mavsdk::CameraServer::StorageInformation &storage_information);
    virtual mavsdk::CameraServer::Result fill_capture_status(
        mavsdk::CameraServer::CaptureStatus &capture_status);
private:
    std::atomic<bool> _is_capture_in_progress;
    std::atomic<int> _image_count;
    std::atomic<bool> _is_recording_video;
    std::chrono::steady_clock::time_point _start_video_time;
private:
    std::mutex _mutex{};
};

}  // namespace mid