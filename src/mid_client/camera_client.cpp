#include "camera_client.h"

#include "camera_local_client.h"
#include "camera_rpc_client.h"

namespace mid {

CameraClient *CreateLocalCameraClient() {
    return new CameraLocalClient();
}

CameraClient *CreateRpcCameraClient(int rpc_port) {
    CameraRpcClient *client = new CameraRpcClient();
    client->Init(rpc_port);

    return client;
}

}  // namespace mid