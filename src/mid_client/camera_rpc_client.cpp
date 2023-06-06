#include "camera_rpc_client.h"

#include <string>

#include "base/log.h"
#include "camera/camera.pb.h"

namespace mid {

static mavsdk::CameraServer::Result translateFromRpcResult(
    const mavsdk::rpc::camera::CameraResult_Result result);

CameraRpcClient::CameraRpcClient() {}

CameraRpcClient::~CameraRpcClient() {}

bool CameraRpcClient::Init(int rpc_port) {
    std::string target = "0.0.0.0:" + std::to_string(rpc_port);
    //the channel isn't authenticated
    _channel = grpc::CreateChannel(target, grpc::InsecureChannelCredentials());
    _stub = mavsdk::rpc::camera::CameraService::NewStub(_channel);
    return true;
}

mavsdk::CameraServer::Result CameraRpcClient::take_photo(int index) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call take photo " << index;
    _is_capture_in_progress = true;

    mavsdk::rpc::camera::TakePhotoRequest request;
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    grpc::ClientContext context;
    mavsdk::rpc::camera::TakePhotoResponse response;
    grpc::Status status = _stub->TakePhoto(&context, request, &response);
    _is_capture_in_progress = false;
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Take photo result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::start_video() {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video() {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::start_video_streaming(int stream_id) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video_streaming(int stream_id) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::set_mode(mavsdk::CameraServer::Mode mode) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::format_storage(int storage_id) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::reset_settings() {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::fill_information(
    mavsdk::CameraServer::Information &information) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::fill_storage_information(
    mavsdk::CameraServer::StorageInformation &storage_information) {
    return mavsdk::CameraServer::Result();
}

mavsdk::CameraServer::Result CameraRpcClient::fill_capture_status(
    mavsdk::CameraServer::CaptureStatus &capture_status) {
    return mavsdk::CameraServer::Result();
}

static mavsdk::CameraServer::Result translateFromRpcResult(
    const mavsdk::rpc::camera::CameraResult_Result result) {
    switch (result) {
        default:
            LogError() << "Unknown result enum value: " << static_cast<int>(result);
        // FALLTHROUGH
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_UNKNOWN:
            return mavsdk::CameraServer::Result::Unknown;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS:
            return mavsdk::CameraServer::Result::Success;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_IN_PROGRESS:
            return mavsdk::CameraServer::Result::InProgress;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_BUSY:
            return mavsdk::CameraServer::Result::Busy;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_DENIED:
            return mavsdk::CameraServer::Result::Denied;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_ERROR:
            return mavsdk::CameraServer::Result::Error;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_TIMEOUT:
            return mavsdk::CameraServer::Result::Timeout;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_WRONG_ARGUMENT:
            return mavsdk::CameraServer::Result::WrongArgument;
        case mavsdk::rpc::camera::CameraResult_Result_RESULT_NO_SYSTEM:
            return mavsdk::CameraServer::Result::NoSystem;
    }
}

}  // namespace mid