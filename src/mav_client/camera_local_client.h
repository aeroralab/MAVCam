#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "camera_client.h"
#include "libirextension.h"
#include "mav_camera.h"
#include "plugins/camera/camera.h"

namespace mavcam {

class CameraLocalClient : public CameraClient {
public:
    CameraLocalClient();
    virtual ~CameraLocalClient();
public:  // operation
    virtual mavsdk::CameraServer::Result take_photo(int index) override;
    virtual mavsdk::CameraServer::Result start_video() override;
    virtual mavsdk::CameraServer::Result stop_video() override;
    virtual mavsdk::CameraServer::Result start_video_streaming(int stream_id) override;
    virtual mavsdk::CameraServer::Result stop_video_streaming(int stream_id) override;
    virtual mavsdk::CameraServer::Result set_mode(mavsdk::CameraServer::Mode mode) override;
    virtual mavsdk::CameraServer::Result format_storage(int storage_id) override;
    virtual mavsdk::CameraServer::Result reset_settings() override;
    virtual mavsdk::CameraServer::Result set_timestamp(int64_t time_unix_msec) override;
    virtual mavsdk::CameraServer::Result set_zoom_range(float range) override;
public:  // subscribe
    virtual mavsdk::CameraServer::Result fill_information(
        mavsdk::CameraServer::Information &information) override;
    virtual mavsdk::CameraServer::Result fill_video_stream_info(
        std::vector<mavsdk::CameraServer::VideoStreamInfo> &video_stream_infos) override;
    virtual mavsdk::CameraServer::Result fill_storage_information(
        mavsdk::CameraServer::StorageInformation &storage_information) override;
    virtual mavsdk::CameraServer::Result fill_capture_status(
        mavsdk::CameraServer::CaptureStatus &capture_status) override;
    virtual mavsdk::CameraServer::Result fill_settings(
        mavsdk::CameraServer::Settings &settings) override;
public:  // settings
    virtual mavsdk::CameraServer::Result retrieve_current_settings(
        std::vector<mavsdk::Camera::Setting> &settings) override;
    mavsdk::CameraServer::Result set_setting(mavsdk::Camera::Setting setting) override;
    std::pair<mavsdk::CameraServer::Result, mavsdk::Camera::Setting> get_setting(
        mavsdk::Camera::Setting setting) const override;
public:
    /**
     * @biref int local camera client instance
     */
    bool init();
private:
    /**
     * @brief free resource abort camera
     */
    void deinit();
    /**
     * @brief set camera display mode
     */
    bool set_camera_display_mode(std::string mode);
    /**
     * @brief get current camera display mode
     */
    std::string get_camera_display_mode();
    /**
     * @brief set whitebalance mode
    */
    bool set_whitebalance_mode(std::string mode);
    /**
     * @brief get camera whitebalance mode
    */
    std::string get_whitebalance_mode();
    /**
     * @brief get camera exposure value
     */
    std::string get_ev_value();
    /**
     * @brief get camera iso value
     */
    std::string get_iso_value();
    /**
     * @brief get shutter speed value
     */
    std::string get_shutter_speed_value();
    /**
     * @brief get video resoltuion
     */
    std::string get_video_resolution();
    /**
     * @brief set video resoltuion
     */
    bool set_video_resolution(std::string value);
    /**
     * @brief set metering mode
     */
    bool set_metering_mode(std::string value);
    /**
     * @brief init ir camera
     */
    bool init_ir_camera();
    /**
     * @breif free ir camera
     */
    void free_ir_camera();
    /**
     * @brief get ir camera palette
     */
    int get_ir_palette();
    /**
     * @brief set ir camera palette
     */
    bool set_ir_palette(std::string color_mode);
    /**
     * @brief execute ir camera FFC
     */
    bool set_ir_FFC(std::string ignore);
private:
    mavsdk::Camera::Setting build_setting(std::string name, std::string value);
    mavsdk::CameraServer::Result convert_camera_result_to_mav_server_result(
        mav_camera::Result input_result);
private:
    std::atomic<int> _image_count;
    std::atomic<bool> _is_recording_video;
    std::chrono::steady_clock::time_point _start_video_time;
    mutable mavsdk::CameraServer::Mode _current_mode{mavsdk::CameraServer::Mode::Unknown};
    mutable std::mutex _storage_information_mutex;
    mutable mav_camera::StorageInformation _current_storage_information;
    mutable std::unordered_map<std::string, std::string> _settings;
    int32_t _framerate;
private:
    std::mutex _mutex{};
private:
    void *_plugin_handle{NULL};
    mav_camera::MavCamera *_mav_camera{nullptr};
private:
    void *_ir_camera_handle{NULL};
    struct ir_extension_api *_ir_camera{nullptr};
};

}  // namespace mavcam
