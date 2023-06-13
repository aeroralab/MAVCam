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
    bool init(std::string &connection_url, bool use_local, int rpc_port);
    bool start_runloop();
private:
    void subscribe_camera_operation(mavsdk::CameraServer &camera_server);
    void subscribe_param_operation(mavsdk::ParamServer &param_server);
private:
    std::string _connection_url;
    int _rpc_port;
    CameraClient *_camera_client;
};

}  // namespace mid