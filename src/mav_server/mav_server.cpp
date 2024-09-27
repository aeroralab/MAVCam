#include "mav_server.h"

#include <chrono>
#include <string>
#include <thread>

#include "base/log.h"
#include "plugins/camera/camera_impl.h"
#include "plugins/camera/camera_service_impl.h"

namespace mavcam {

bool MavServer::init(int rpc_port) {
    _rpc_port = rpc_port;
    return true;
}

bool MavServer::start_runloop() {
    std::string server_address{"127.0.0.1"};
    server_address += ":" + std::to_string(_rpc_port);
    CameraServiceImpl service(std::make_shared<Camera>());

    // Build server
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    _server = builder.BuildAndStart();
    if (!_server) {
        base::LogError() << "Failed to start server on " << server_address;
        return false;
    }

    // Run server
    base::LogInfo() << "Server listening on " << server_address;
    _running = true;
    while (_running.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
    return 0;
}

void MavServer::stop_runloop() {
    _running = false;
    _server->Shutdown();
}

}  // namespace mavcam
