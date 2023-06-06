#include "mid_client.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera_server/camera_server.h>
#include <mavsdk/plugins/param_server/param_server.h>

#include <chrono>
#include <thread>

#include "base/log.h"
#include "camera_client.h"

namespace mid {

bool MidClient::Init(std::string &connection_url, int rpc_port) {
    //Todo need check connection url first
    _connection_url = connection_url;
    _rpc_port = rpc_port;

    //_camera_client = CreateLocalCameraClient();  // use local client
    _camera_client = CreateRpcCameraClient(_rpc_port);  // use rpc client
    return true;
}

bool MidClient::StartRunloop() {
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
    SubscribeCameraOperation(camera_server);
    auto param_server = mavsdk::ParamServer{server_component};
    SubscribeParamOperation(param_server);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MidClient::SubscribeCameraOperation(mavsdk::CameraServer &camera_server) {
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

void MidClient::SubscribeParamOperation(mavsdk::ParamServer &param_server) {
    return;
    param_server.provide_param_custom("CAM_EV", "1.0", mavsdk::ParamServer::Type::Float);
    param_server.subscribe_param_changed(
        "CAM_EV", mavsdk::ParamServer::Type::Float,
        [](std::string value) { std::cout << "change CAM_EV to " << value << std::endl; });

    param_server.provide_param_custom("CAM_CUSTOMWB", "5500", mavsdk::ParamServer::Type::Uint16);
    param_server.subscribe_param_changed(
        "CAM_CUSTOMWB", mavsdk::ParamServer::Type::Uint16,
        [](std::string value) { std::cout << "change CAM_CUSTOMWB to " << value << std::endl; });

    param_server.provide_param_custom("CAM_SPOTAREA", "0", mavsdk::ParamServer::Type::Uint16);
    param_server.subscribe_param_changed(
        "CAM_SPOTAREA", mavsdk::ParamServer::Type::Uint16,
        [](std::string value) { std::cout << "change CAM_SPOTAREA to " << value << std::endl; });

    param_server.provide_param_custom("CAM_ASPECTRATIO", "1.777777",
                                      mavsdk::ParamServer::Type::Float);
    param_server.subscribe_param_changed(
        "CAM_ASPECTRATIO", mavsdk::ParamServer::Type::Float,
        [](std::string value) { std::cout << "change CAM_ASPECTRATIO to " << value << std::endl; });

    param_server.provide_param_custom("CAM_PHOTOQUAL", "1", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_PHOTOQUAL", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_PHOTOQUAL to " << value << std::endl; });

    param_server.provide_param_custom("CAM_FILENUMOPT", "0", mavsdk::ParamServer::Type::Uint8);
    param_server.subscribe_param_changed(
        "CAM_FILENUMOPT", mavsdk::ParamServer::Type::Uint8,
        [](std::string value) { std::cout << "change CAM_FILENUMOPT to " << value << std::endl; });

    param_server.provide_param_custom("CAM_PHOTOFMT", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_PHOTOFMT", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_PHOTOFMT to " << value << std::endl; });

    param_server.provide_param_custom("CAM_MODE", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_MODE", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_MODE to " << value << std::endl; });

    param_server.provide_param_custom("CAM_FLICKER", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_FLICKER", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_FLICKER to " << value << std::endl; });

    param_server.provide_param_custom("CAM_SHUTTERSPD", "0.01", mavsdk::ParamServer::Type::Float);
    param_server.subscribe_param_changed(
        "CAM_SHUTTERSPD", mavsdk::ParamServer::Type::Float,
        [](std::string value) { std::cout << "change CAM_SHUTTERSPD to " << value << std::endl; });

    param_server.provide_param_custom("CAM_WBMODE", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_WBMODE", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_WBMODE to " << value << std::endl; });

    param_server.provide_param_custom("CAM_COLORENCODE", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_COLORENCODE", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_COLORENCODE to " << value << std::endl; });

    param_server.provide_param_custom("CAM_EXPMODE", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_EXPMODE", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_EXPMODE to " << value << std::endl; });

    param_server.provide_param_custom("CAM_ISO", "100", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_ISO", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_ISO to " << value << std::endl; });

    param_server.provide_param_custom("CAM_VIDRES", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_VIDRES", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_VIDRES to " << value << std::endl; });

    param_server.provide_param_custom("CAM_IMAGEDEWARP", "0", mavsdk::ParamServer::Type::Uint8);
    param_server.subscribe_param_changed(
        "CAM_IMAGEDEWARP", mavsdk::ParamServer::Type::Uint8,
        [](std::string value) { std::cout << "change CAM_IMAGEDEWARP to " << value << std::endl; });

    param_server.provide_param_custom("CAM_PHOTORATIO", "1", mavsdk::ParamServer::Type::Uint8);
    param_server.subscribe_param_changed(
        "CAM_PHOTORATIO", mavsdk::ParamServer::Type::Uint8,
        [](std::string value) { std::cout << "change CAM_PHOTORATIO to " << value << std::endl; });

    param_server.provide_param_custom("CAM_VIDFMT", "1", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_VIDFMT", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_VIDFMT to " << value << std::endl; });

    param_server.provide_param_custom("CAM_METERING", "0", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_METERING", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_METERING to " << value << std::endl; });

    param_server.provide_param_custom("CAM_COLORMODE", "1", mavsdk::ParamServer::Type::Uint32);
    param_server.subscribe_param_changed(
        "CAM_COLORMODE", mavsdk::ParamServer::Type::Uint32,
        [](std::string value) { std::cout << "change CAM_COLORMODE to " << value << std::endl; });
}

}  // namespace mid