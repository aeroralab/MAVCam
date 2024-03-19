#include "mav_server.h"

#include <grpc++/grpc++.h>

#include <chrono>
#include <string>
#include <thread>

#include "base/log.h"
#include "plugins/camera/camera_impl.h"
#include "plugins/camera/camera_service_impl.h"

namespace mav {

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

    std::unique_ptr<grpc::Server> server{builder.BuildAndStart()};

    // Run server
    base::LogInfo() << "Server listening on " << server_address;
    server->Wait();

    return 0;
}

}  // namespace mav
