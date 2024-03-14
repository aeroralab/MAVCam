#pragma once

namespace mid {

class MidServer final {
public:
    MidServer() {}
    ~MidServer() {}
public:
    bool init(int rpc_port);
    bool start_runloop();
private:
    int _rpc_port;
};

}  // namespace mid