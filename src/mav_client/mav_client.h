#pragma once

#include <string>

namespace mavsdk {
class CameraServer;
class ParamServer;
}  // namespace mavsdk

namespace mav {

class CameraClient;

class MavClient {
public:
    MavClient() {}
    ~MavClient() {}
public:
    bool init(std::string &connection_url, bool use_local, int32_t rpc_port,
              std::string &ftp_root_path);
    bool start_runloop();
private:
    void subscribe_camera_operation(mavsdk::CameraServer &camera_server);
    void subscribe_param_operation(mavsdk::ParamServer &param_server);
private:
    std::string _connection_url;
    int32_t _rpc_port;
    CameraClient *_camera_client;
    std::string _ftp_root_path;
};

}  // namespace mav
