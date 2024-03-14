#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#include "plugins/camera/camera.h"

namespace mav {

class CameraImpl : public Camera {
public:
    CameraImpl();
    virtual ~CameraImpl();
public:
    virtual Camera::Result prepare() const override;
    virtual Camera::Result take_photo() const override;
    virtual Camera::Result start_photo_interval(float interval_s) const override;
    virtual Camera::Result stop_photo_interval() const override;
    virtual Camera::Result start_video() const override;
    virtual Camera::Result stop_video() const override;
    virtual Camera::Result start_video_streaming(int32_t stream_id) const override;
    virtual Camera::Result stop_video_streaming(int32_t stream_id) const override;
    virtual Camera::Result set_mode(Camera::Mode mode) const override;
    virtual std::pair<Result, std::vector<Camera::CaptureInfo>> list_photos(
        Camera::PhotosRange photos_range) const override;
    virtual void subscribe_mode(const Camera::ModeCallback &callback) override;
    virtual Camera::Mode mode() const override;
    virtual void subscribe_information(const Camera::InformationCallback &callback) override;
    virtual Camera::Information information() const override;
    virtual void subscribe_video_stream_info(
        const Camera::VideoStreamInfoCallback &callback) override;
    virtual std::vector<Camera::VideoStreamInfo> video_stream_info() const override;
    virtual void subscribe_capture_info(const Camera::CaptureInfoCallback &callback) override;
    virtual void subscribe_status(const Camera::StatusCallback &callback) override;
    virtual Camera::Status status() const override;
    virtual void subscribe_current_settings(
        const Camera::CurrentSettingsCallback &callback) override;
    virtual void subscribe_possible_setting_options(
        const Camera::PossibleSettingOptionsCallback &callback) override;
    virtual std::vector<Camera::SettingOptions> possible_setting_options() const override;
    virtual Camera::Result set_setting(Setting setting) const override;
    virtual std::pair<Camera::Result, Camera::Setting> get_setting(
        Camera::Setting setting) const override;
    virtual Camera::Result format_storage(int32_t storage_id) const override;
    virtual Camera::Result select_camera(int32_t camera_id) const override;
    virtual Camera::Result reset_settings() const override;
    virtual Camera::Result set_definition_data(std::string definition_data) const override;
private:
    void start();
    void stop();
    static void work_thread(CameraImpl *self);
    mav::Camera::Setting build_setting(std::string name, std::string value);
private:
    std::thread *_work_thread{nullptr};
    std::atomic<bool> _should_exit{false};
    std::mutex _callback_mutex{};
    Camera::ModeCallback _camera_mode_callback;
    std::atomic<bool> _need_update_camera_information;
    Camera::InformationCallback _camera_information_callback;
    std::atomic<bool> _need_update_video_stream_info;
    Camera::VideoStreamInfoCallback _video_stream_info_callback;
    mutable std::chrono::steady_clock::time_point _start_video_time;
    mutable Camera::Status _status;
    Camera::StatusCallback _status_callback;
private:
    mutable std::vector<Camera::Setting> _settings;
    mutable std::atomic<float> _total_storage_mib;
    mutable std::atomic<float> _available_storage_mib;
};

}  // namespace mav