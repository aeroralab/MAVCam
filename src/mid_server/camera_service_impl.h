#pragma once

//TODO may need auto generate

#include "base/log.h"
#include "camera/camera.grpc.pb.h"

namespace mid {

class CameraServiceImpl final : public mavsdk::rpc::camera::CameraService::Service {
    ::grpc::Status TakePhoto(::grpc::ServerContext *context,
                             const mavsdk::rpc::camera::TakePhotoRequest *request,
                             mavsdk::rpc::camera::TakePhotoResponse *response) override {
        LogDebug() << "call take photo method";

        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("Success");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }

    ::grpc::Status StartVideo(::grpc::ServerContext *context,
                              const mavsdk::rpc::camera::StartVideoRequest *request,
                              mavsdk::rpc::camera::StartVideoResponse *response) override {
        LogDebug() << "call start video method";

        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("Success");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }

    ::grpc::Status StopVideo(::grpc::ClientContext *context,
                             const ::mavsdk::rpc::camera::StopVideoRequest &request,
                             ::mavsdk::rpc::camera::StopVideoResponse *response) {
        LogDebug() << "call stop video method";

        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("RTFD");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }

    ::grpc::Status StartVideoStreaming(
        ::grpc::ServerContext *context,
        const ::mavsdk::rpc::camera::StartVideoStreamingRequest *request,
        ::mavsdk::rpc::camera::StartVideoStreamingResponse *response) override {
        LogDebug() << "call start video streaming method";

        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("RTFD");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }

    ::grpc::Status StopVideoStreaming(
        ::grpc::ServerContext *context,
        const ::mavsdk::rpc::camera::StopVideoStreamingRequest *request,
        ::mavsdk::rpc::camera::StopVideoStreamingResponse *response) override {
        LogDebug() << "call stop video streaming method";

        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("RTFD");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }

    ::grpc::Status SetMode(::grpc::ServerContext *context,
                           const ::mavsdk::rpc::camera::SetModeRequest *request,
                           ::mavsdk::rpc::camera::SetModeResponse *response) override {
        LogDebug() << "call set mode method";

        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("RTFD");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }

    ::grpc::Status ListPhotos(::grpc::ServerContext *context,
                              const ::mavsdk::rpc::camera::ListPhotosRequest *request,
                              ::mavsdk::rpc::camera::ListPhotosResponse *response) override {
        LogDebug() << "call list photos method";
        return ::grpc::Status::OK;
    }

    ::grpc::Status SubscribeMode(
        ::grpc::ServerContext *context, const ::mavsdk::rpc::camera::SubscribeModeRequest *request,
        ::grpc::ServerWriter< ::mavsdk::rpc::camera::ModeResponse> *writer) override {
        LogDebug() << "call subscirbe mode";
        return ::grpc::Status::OK;
    }

    ::grpc::Status SubscribeInformation(
        ::grpc::ServerContext *context,
        const ::mavsdk::rpc::camera::SubscribeInformationRequest *request,
        ::grpc::ServerWriter< ::mavsdk::rpc::camera::InformationResponse> *writer) override {
        LogDebug() << "call subscirbe information";
        return ::grpc::Status::OK;
    }

    ::grpc::Status SubscribeVideoStreamInfo(
        ::grpc::ServerContext *context,
        const ::mavsdk::rpc::camera::SubscribeVideoStreamInfoRequest *request,
        ::grpc::ServerWriter< ::mavsdk::rpc::camera::VideoStreamInfoResponse> *writer) override {
        LogDebug() << "call subscirbe information";
        return ::grpc::Status::OK;
    }

    ::grpc::Status FormatStorage(::grpc::ServerContext *context,
                                 const ::mavsdk::rpc::camera::FormatStorageRequest *request,
                                 ::mavsdk::rpc::camera::FormatStorageResponse *response) override {
        LogDebug() << "call format storage";
        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("Success");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }

    ::grpc::Status ResetSettings(::grpc::ServerContext *context,
                                 const ::mavsdk::rpc::camera::ResetSettingsRequest *request,
                                 ::mavsdk::rpc::camera::ResetSettingsResponse *response) override {
        LogDebug() << "call reset settings";
        auto *rpc_camera_result = new mavsdk::rpc::camera::CameraResult();
        rpc_camera_result->set_result(mavsdk::rpc::camera::CameraResult_Result_RESULT_SUCCESS);
        rpc_camera_result->set_result_str("Success");
        response->set_allocated_camera_result(rpc_camera_result);
        return ::grpc::Status::OK;
    }
};

}  // namespace mid