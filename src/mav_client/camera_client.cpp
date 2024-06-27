#include "camera_client.h"

#include <base/log.h>

#include "camera_local_client.h"
#ifdef ENABLE_SERVER
#include "camera_rpc_client.h"
#endif

namespace mav {

CameraClient *CreateLocalCameraClient() {
    return new CameraLocalClient();
}

CameraClient *CreateRpcCameraClient(int rpc_port) {
#ifdef ENABLE_SERVER
    CameraRpcClient *client = new CameraRpcClient();
    client->init(rpc_port);
    return client;
#else
    base::LogError() << "Cannot use rpc server when disable server build";
    return nullptr;
#endif
}

}  // namespace mav
