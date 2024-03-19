#pragma once

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
     * @brief Poll for 'Mode' (blocking).
     *
     * @return One Mode update.
     */
    Camera::Mode mode() const;

    /**
     * @brief Poll for 'Information' (blocking).
     *
     * @return One Information update.
     */
    Camera::Information information() const;

    /**
     * @brief Poll for 'std::vector<VideoStreamInfo>' (blocking).
     *
     * @return One std::vector<VideoStreamInfo> update.
     */
    std::vector<Camera::VideoStreamInfo> video_stream_info() const;

    /**
     * @brief Poll for 'Status' (blocking).
     *
     * @return One Status update.
     */
    Camera::Status status() const;

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
};

}  // namespace mav