#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#include "plugins/camera/camera.h"

namespace mav {

class CameraImpl final {
public:
    explicit CameraImpl();
    ~CameraImpl();

    /**
     * @brief Prepare the camera plugin (e.g. download the camera definition, etc).
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result prepare();

    /**
     * @brief Take one photo.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result take_photo();

    /**
     * @brief Start photo timelapse with a given interval.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result start_photo_interval(float interval_s);

    /**
     * @brief Stop a running photo timelapse.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result stop_photo_interval();

    /**
     * @brief Start a video recording.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result start_video();

    /**
     * @brief Stop a running video recording.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result stop_video();

    /**
     * @brief Start video streaming.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result start_video_streaming(int32_t stream_id);

    /**
     * @brief Stop current video streaming.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result stop_video_streaming(int32_t stream_id);

    /**
     * @brief Set camera mode.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result set_mode(Camera::Mode mode);

    /**
     * @brief List photos available on the camera.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    std::pair<Camera::Result, std::vector<Camera::CaptureInfo>> list_photos(
        Camera::PhotosRange photos_range);

    /**
     * @brief Subscribe to camera mode updates.
     */
    void mode_async(const Camera::ModeCallback &callback);

    /**
     * @brief Poll for 'Mode' (blocking).
     *
     * @return One Mode update.
     */
    Camera::Mode mode() const;

    /**
     * @brief Subscribe to camera information updates.
     */
    void information_async(const Camera::InformationCallback &callback);

    /**
     * @brief Poll for 'Information' (blocking).
     *
     * @return One Information update.
     */
    Camera::Information information() const;

    /**
     * @brief Subscribe to video stream info updates.
     */
    void video_stream_info_async(const Camera::VideoStreamInfoCallback &callback);

    /**
     * @brief Poll for 'std::vector<VideoStreamInfo>' (blocking).
     *
     * @return One std::vector<VideoStreamInfo> update.
     */
    std::vector<Camera::VideoStreamInfo> video_stream_info() const;

    /**
     * @brief Subscribe to capture info updates.
     */
    void capture_info_async(const Camera::CaptureInfoCallback &callback);

    /**
     * @brief Subscribe to camera status updates.
     */
    void status_async(const Camera::StatusCallback &callback);

    /**
     * @brief Poll for 'Status' (blocking).
     *
     * @return One Status update.
     */
    Camera::Status status() const;

    /**
     * @brief Get the list of current camera settings.
     */
    void current_settings_async(const Camera::CurrentSettingsCallback &callback);

    /**
     * @brief Get the list of settings that can be changed.
     */
    void possible_setting_options_async(const Camera::PossibleSettingOptionsCallback &callback);

    /**
     * @brief Poll for 'std::vector<SettingOptions>' (blocking).
     *
     * @return One std::vector<SettingOptions> update.
     */
    std::vector<Camera::SettingOptions> possible_setting_options() const;

    /**
     * @brief Set a setting to some value.
     *
     * Only setting_id of setting and option_id of option needs to be set.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result set_setting(Camera::Setting setting);

    /**
     * @brief Get a setting.
     *
     * Only setting_id of setting needs to be set.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    std::pair<Camera::Result, Camera::Setting> get_setting(Camera::Setting setting);

    /**
     * @brief Format storage (e.g. SD card) in camera.
     *
     * This will delete all content of the camera storage!
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result format_storage(int32_t storage_id);

    /**
     * @brief Select current camera .
     *
     * Bind the plugin instance to a specific camera_id
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result select_camera(int32_t camera_id);

    /**
     * @brief Reset all settings in camera.
     *
     * This will reset all camera settings to default value
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result reset_settings();

    /**
     * @brief Manual set the definition data
     * e.g. use mavlink ftp download definition file and set to camera
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    Camera::Result set_definition_data(std::string definition_data);
private:
    mav::Camera::Setting build_setting(std::string name, std::string value);
private:
    Camera::ModeCallback _camera_mode_callback;
    Camera::CaptureInfoCallback _capture_info_callback;
    mutable Camera::Status _status;
    Camera::StatusCallback _status_callback;
private:
    mutable Camera::Mode _current_mode{Camera::Mode::Unknown};
    mutable std::chrono::steady_clock::time_point _start_video_time;
    mutable std::vector<Camera::Setting> _settings;
    mutable std::atomic<float> _total_storage_mib;
    mutable std::atomic<float> _available_storage_mib;
};

}  // namespace mav