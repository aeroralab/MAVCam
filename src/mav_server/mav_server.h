#pragma once

#include <grpc++/grpc++.h>

namespace mavcam {

class MavServer final {
public:
    MavServer() {}
    ~MavServer() {}
public:
    bool init(int rpc_port, int num_thread);
    bool start_runloop();
    void stop_runloop();
private:
    int _rpc_port;
    int _num_thread{0};
    std::unique_ptr<grpc::Server> _server;
};

}  // namespace mavcam
