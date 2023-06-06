#include "camera_client.h"

#include "camera_local_client.h"
#include "camera_rpc_client.h"

namespace mid {

CameraClient *CreateLocalCameraClient() {
    return new CameraLocalClient();
}

CameraClient *CreateRpcCameraClient() {
    return new CameraRpcClient();
}

}  // namespace mid