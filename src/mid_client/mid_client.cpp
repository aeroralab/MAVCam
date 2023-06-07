#include "mid_client.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera_server/camera_server.h>
#include <mavsdk/plugins/param_server/param_server.h>

#include <chrono>
#include <thread>

#include "base/log.h"
#include "camera_client.h"

namespace mid {

bool MidClient::init(std::string &connection_url, int rpc_port) {
    //Todo need check connection url first
    _connection_url = connection_url;
    _rpc_port = rpc_port;

    _camera_client = CreateLocalCameraClient();  // use local client
    //_camera_client = CreateRpcCameraClient(_rpc_port);  // use rpc client
    return true;
}

bool MidClient::start_runloop() {
    mavsdk::Mavsdk mavsdk;
    mavsdk::Mavsdk::Configuration configuration(mavsdk::Mavsdk::Configuration::UsageType::Camera);
    mavsdk.set_configuration(configuration);

    auto result = mavsdk.add_any_connection(_connection_url);
    if (result != mavsdk::ConnectionResult::Success) {
        LogError() << "Could not establish connection: " << result;
        return false;
    }
    LogInfo() << "Created middleware client success";

    // works as camera
    auto server_component =
        mavsdk.server_component_by_type(mavsdk::Mavsdk::ServerComponentType::Camera);
    auto camera_server = mavsdk::CameraServer{server_component};
    subscribe_camera_operation(camera_server);
    auto param_server = mavsdk::ParamServer{server_component};
    subscribe_param_operation(param_server);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MidClient::subscribe_camera_operation(mavsdk::CameraServer &camera_server) {
    camera_server.subscribe_take_photo([this, &camera_server](int32_t index) {
        _camera_client->take_photo(index);

        //TODO no position info for now
        auto position = mavsdk::CameraServer::Position{};
        auto attitude = mavsdk::CameraServer::Quaternion{};

        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();
        auto success = true;
        camera_server.respond_take_photo(mavsdk::CameraServer::TakePhotoFeedback::Ok,
                                         mavsdk::CameraServer::CaptureInfo{
                                             .position = position,
                                             .attitude_quaternion = attitude,
                                             .time_utc_us = static_cast<uint64_t>(timestamp),
                                             .is_success = success,
                                             .index = index,
                                             .file_url = {},
                                         });
    });

    camera_server.subscribe_start_video(
        [this](int32_t stream_id) { _camera_client->start_video(); });

    camera_server.subscribe_stop_video([this](int32_t stream_id) { _camera_client->stop_video(); });

    camera_server.subscribe_start_video_streaming(
        [this](int32_t stream_id) { _camera_client->start_video_streaming(stream_id); });

    camera_server.subscribe_stop_video_streaming(
        [this](int32_t stream_id) { _camera_client->stop_video_streaming(stream_id); });

    camera_server.subscribe_set_mode(
        [this](mavsdk::CameraServer::Mode mode) { _camera_client->set_mode(mode); });

    camera_server.subscribe_storage_information([this, &camera_server](int32_t storage_id) {
        mavsdk::CameraServer::StorageInformation storage_information;
        _camera_client->fill_storage_information(storage_information);
        camera_server.respond_storage_information(storage_information);
    });

    camera_server.subscribe_capture_status([this, &camera_server](int32_t reserved) {
        mavsdk::CameraServer::CaptureStatus capture_status;
        _camera_client->fill_capture_status(capture_status);
        camera_server.respond_capture_status(capture_status);
    });

    camera_server.subscribe_format_storage(
        [this](int storage_id) { _camera_client->format_storage(storage_id); });

    camera_server.subscribe_reset_settings(
        [this](int camera_id) { _camera_client->reset_settings(); });
    // Then set the initial state of everything.

    // Finally call set_information() to "activate" the camera plugin.
    mavsdk::CameraServer::Information information;
    _camera_client->fill_information(information);
    auto ret = camera_server.set_information(information);

    if (ret != mavsdk::CameraServer::Result::Success) {
        LogError() << "Failed to set camera info";
    }
}

void MidClient::subscribe_param_operation(mavsdk::ParamServer &param_server) {
    param_server.subscribe_custom_param_changed(
        [this](mavsdk::ParamServer::CustomParam custom_param) {
            mavsdk::Camera::Setting setting;
            setting.setting_id = custom_param.name;
            setting.option.option_id = custom_param.value;
            _camera_client->set_setting(setting);
        });

    std::vector<mavsdk::Camera::Setting> settings;
    _camera_client->retrieve_current_settings(settings);

    for (auto &setting : settings) {
        if (setting.setting_id == "CAM_SHUTTERSPD" || setting.setting_id == "CAM_EV") {
            param_server.provide_param_float(setting.setting_id,
                                             std::stof(setting.option.option_id));
        } else {
            param_server.provide_param_int(setting.setting_id, std::stoi(setting.option.option_id));
        }
    }
}

}  // namespace mid