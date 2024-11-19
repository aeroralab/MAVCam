#include "mav_server.h"

#include <chrono>
#include <string>
#include <thread>

#include "base/log.h"
#include "plugins/camera/camera_impl.h"
#include "plugins/camera/camera_service_impl.h"

namespace mavcam {

bool MavServer::init(int rpc_port, int num_thread) {
    _rpc_port = rpc_port;
    _num_thread = num_thread;
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

    // Configure the thread pool to use exactly threads.
    if (_num_thread != 0) {
        base::LogInfo() << "Config grpc thread pool thread num to " << _num_thread;
        builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::NUM_CQS, _num_thread);
        builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MIN_POLLERS,
                                    _num_thread);
        builder.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::MAX_POLLERS,
                                    _num_thread);
    }

    _server = builder.BuildAndStart();
    if (!_server) {
        base::LogError() << "Failed to start server on " << server_address;
        return false;
    }

    // Run server
    base::LogInfo() << "Server listening on " << server_address;
    _server->Wait();
    return true;
}

void MavServer::stop_runloop() {
    _server->Shutdown();
}

}  // namespace mavcam
