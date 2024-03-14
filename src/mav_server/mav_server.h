#pragma once

namespace mav {

class MavServer final {
public:
    MavServer() {}
    ~MavServer() {}
public:
    bool init(int rpc_port);
    bool start_runloop();
private:
    int _rpc_port;
};

}  // namespace mav