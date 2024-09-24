#pragma once

#include <grpc++/grpc++.h>

namespace mavcam {

class MavServer final {
public:
    MavServer() {}
    ~MavServer() {}
public:
    bool init(int rpc_port);
    bool start_runloop();
    void stop_runloop();
private:
    int _rpc_port;
    std::atomic<bool> _running{false};
    std::unique_ptr<grpc::Server> _server;
};

}  // namespace mavcam
