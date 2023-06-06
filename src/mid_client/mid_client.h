#pragma once

#include <string>

namespace mavsdk {
class CameraServer;
class ParamServer;
}  // namespace mavsdk

namespace mid {

class CameraClient;

class MidClient {
public:
    MidClient() {}
    ~MidClient() {}
public:
    bool Init(std::string &connection_url, int rpc_port);
    bool StartRunloop();
private:
    void SubscribeCameraOperation(mavsdk::CameraServer &camera_server);
    void SubscribeParamOperation(mavsdk::ParamServer &param_server);
private:
    std::string _connection_url;
    int _rpc_port;
    CameraClient *_camera_client;
};

}  // namespace mid