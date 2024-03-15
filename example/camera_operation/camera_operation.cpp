#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>
#include <mavsdk/plugins/ftp/ftp.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

static void do_camera_operation(mavsdk::Camera &camera);

int main(int argc, const char *argv[]) {
    // we run client plugins to act as the GCS
    // to communicate with the camera server plugins.
    mavsdk::Mavsdk mavsdk{
        mavsdk::Mavsdk::Configuration{mavsdk::Mavsdk::ComponentType::GroundStation}};

    auto result = mavsdk.add_any_connection("udp://:14550");
    if (result == mavsdk::ConnectionResult::Success) {
        std::cout << "Connected success !" << std::endl;
    }

    //wait for all component added
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto prom = std::promise<std::shared_ptr<mavsdk::System>>{};
    auto fut = prom.get_future();
    mavsdk::Mavsdk::NewSystemHandle handle =
        mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
            auto system = mavsdk.systems().back();

            if (system->has_camera()) {
                std::cout << "Discovered camera from Client" << std::endl;

                // Unsubscribe again as we only want to find one system.
                mavsdk.unsubscribe_on_new_system(handle);
                prom.set_value(system);
            } else {
                std::cout << "System has no camera" << std::endl;
            }
        });

    if (fut.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
        std::cout << "No camera found, exiting" << std::endl;
        return -1;
    }

    auto system = fut.get();
    auto camera = mavsdk::Camera{system};

    camera.subscribe_status(
        [](mavsdk::Camera::Status status) { std::cout << status << std::endl; });

    camera.subscribe_capture_info(
        [](mavsdk::Camera::CaptureInfo capture_info) { std::cout << capture_info << std::endl; });

    do_camera_operation(camera);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

static void do_camera_operation(mavsdk::Camera &camera) {
    auto operation_result = camera.format_storage(1);
    std::cout << "format storage result : " << operation_result << std::endl;

    operation_result = camera.reset_settings();
    std::cout << "Reset camera settings result : " << operation_result << std::endl;

    operation_result = camera.take_photo();
    std::cout << "take photo result : " << operation_result << std::endl;

    operation_result = camera.take_photo();
    std::cout << "take photo result : " << operation_result << std::endl;

    int photo_count = 5;
    operation_result = camera.start_photo_interval(1.0);
    std::cout << "start take photo result : " << operation_result << std::endl;
    while (photo_count != 0) {
        photo_count--;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    operation_result = camera.stop_photo_interval();

    operation_result = camera.start_video();
    std::cout << "start video result : " << operation_result << std::endl;

    operation_result = camera.stop_video();
    std::cout << "stop video result : " << operation_result << std::endl;

    operation_result = camera.start_video_streaming(1);
    std::cout << "start video streaming result : " << operation_result << std::endl;

    operation_result = camera.stop_video_streaming(1);
    std::cout << "stop video streaming result : " << operation_result << std::endl;

    operation_result = camera.set_mode(mavsdk::Camera::Mode::Photo);
    std::cout << "Set camera to photo mode result : " << operation_result << std::endl;

    operation_result = camera.set_mode(mavsdk::Camera::Mode::Video);
    std::cout << "Set camera to video mode result : " << operation_result << std::endl;
}