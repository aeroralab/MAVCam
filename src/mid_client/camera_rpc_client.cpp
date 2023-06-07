#include "camera_rpc_client.h"

#include <string>

#include "base/log.h"
#include "camera/camera.pb.h"

namespace mid {

static mavsdk::CameraServer::Result translateFromRpcResult(
    const mavsdk::rpc::camera::CameraResult_Result result);

static mavsdk::rpc::camera::Mode translateFromCameraServerMode(
    const mavsdk::CameraServer::Mode server_mode);

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
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call start video ";
    _is_recording_video = true;
    _start_video_time = std::chrono::steady_clock::now();

    mavsdk::rpc::camera::StartVideoRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::StartVideoResponse response;
    grpc::Status status = _stub->StartVideo(&context, request, &response);
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Start video result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call stop video ";
    _is_recording_video = false;

    mavsdk::rpc::camera::StopVideoRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::StopVideoResponse response;
    grpc::Status status = _stub->StopVideo(&context, request, &response);
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Stop video result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::start_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call start video streaming " << stream_id;

    mavsdk::rpc::camera::StartVideoStreamingRequest request;
    request.set_stream_id(stream_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::StartVideoStreamingResponse response;
    grpc::Status status = _stub->StartVideoStreaming(&context, request, &response);
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Start video streaming result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::stop_video_streaming(int stream_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call stop video streaming " << stream_id;

    mavsdk::rpc::camera::StopVideoStreamingRequest request;
    request.set_stream_id(stream_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::StopVideoStreamingResponse response;
    grpc::Status status = _stub->StopVideoStreaming(&context, request, &response);
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Stop video streaming result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::set_mode(mavsdk::CameraServer::Mode mode) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call set mode " << mode;

    mavsdk::rpc::camera::SetModeRequest request;
    request.set_mode(translateFromCameraServerMode(mode));
    grpc::ClientContext context;
    mavsdk::rpc::camera::SetModeResponse response;
    grpc::Status status = _stub->SetMode(&context, request, &response);
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << " Set mode result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::format_storage(int storage_id) {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call format storage " << storage_id;

    mavsdk::rpc::camera::FormatStorageRequest request;
    request.set_storage_id(storage_id);
    grpc::ClientContext context;
    mavsdk::rpc::camera::FormatStorageResponse response;
    grpc::Status status = _stub->FormatStorage(&context, request, &response);
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << "Format storage result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
}

mavsdk::CameraServer::Result CameraRpcClient::reset_settings() {
    std::lock_guard<std::mutex> lock(_mutex);
    LogDebug() << "rpc call reset settings";

    mavsdk::rpc::camera::ResetSettingsRequest request;
    grpc::ClientContext context;
    mavsdk::rpc::camera::ResetSettingsResponse response;
    grpc::Status status = _stub->ResetSettings(&context, request, &response);
    if (!status.ok()) {
        LogDebug() << "Grpc status error message : " << status.error_message();
        return mavsdk::CameraServer::Result::NoSystem;
    }
    LogDebug() << "Reset settings result : " << response.camera_result().result_str();
    return translateFromRpcResult(response.camera_result().result());
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

mavsdk::CameraServer::Result CameraRpcClient::retrieve_current_settings(
    std::vector<mavsdk::Camera::Setting> &settings) {
    return mavsdk::CameraServer::Result::Success;
}

mavsdk::CameraServer::Result CameraRpcClient::set_setting(mavsdk::Camera::Setting setting) {
    return mavsdk::CameraServer::Result::Success;
}

std::pair<mavsdk::CameraServer::Result, mavsdk::Camera::Setting> CameraRpcClient::get_setting(
    mavsdk::Camera::Setting setting) const {
    return {mavsdk::CameraServer::Result::Success, setting};
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

static mavsdk::rpc::camera::Mode translateFromCameraServerMode(
    const mavsdk::CameraServer::Mode server_mode) {
    switch (server_mode) {
        case mavsdk::CameraServer::Mode::Photo:
            return mavsdk::rpc::camera::Mode::MODE_PHOTO;
        case mavsdk::CameraServer::Mode::Video:
            return mavsdk::rpc::camera::Mode::MODE_VIDEO;
        case mavsdk::CameraServer::Mode::Unknown:
            return mavsdk::rpc::camera::Mode::MODE_UNKNOWN;
    }
}

}  // namespace mid