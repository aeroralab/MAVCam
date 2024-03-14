#include "camera_client.h"

#include "camera_local_client.h"
#ifdef ENABLE_SERVER
#include "camera_rpc_client.h"
#endif

namespace mid {

CameraClient *CreateLocalCameraClient() {
    return new CameraLocalClient();
}

CameraClient *CreateRpcCameraClient(int rpc_port) {
#ifdef ENABLE_SERVER
    CameraRpcClient *client = new CameraRpcClient();
    client->Init(rpc_port);
    return client;
#else
    return nullptr;
#endif
}

}  // namespace mid