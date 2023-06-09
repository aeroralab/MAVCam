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

static std::string download_camera_definition_file_by_ftp(std::shared_ptr<mavsdk::System> system);

static void do_camera_operation(mavsdk::Camera &camera);
static void do_camear_settings(mavsdk::Camera &camera);

static inline void set_camera_settings(mavsdk::Camera &camera, const std::string &name,
                                       const std::string &value);
static inline std::string get_camera_setting(mavsdk::Camera &camera, const std::string &name);

int main(int argc, const char *argv[]) {
    // we run client plugins to act as the GCS
    // to communicate with the camera server plugins.
    mavsdk::Mavsdk mavsdk;
    mavsdk::Mavsdk::Configuration configuration(
        mavsdk::Mavsdk::Configuration::UsageType::GroundStation);
    mavsdk.set_configuration(configuration);

    auto result = mavsdk.add_any_connection("udp://:14540");
    if (result == mavsdk::ConnectionResult::Success) {
        std::cout << "Connected!" << std::endl;
    }

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
                std::cout << "No MAVSDK found" << std::endl;
            }
        });

    if (fut.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
        std::cout << "No camera found, exiting" << std::endl;
        return -1;
    }

    auto system = fut.get();

    auto camera = mavsdk::Camera{system};
    camera.subscribe_information([](mavsdk::Camera::Information info) {
        std::cout << "Camera information:" << std::endl;
        std::cout << info << std::endl;
    });

    camera.subscribe_status([](mavsdk::Camera::Status status) {
        std::cout << "Camera status:" << std::endl;
        std::cout << status << std::endl;
    });

    do_camera_operation(camera);

    std::string define_data = download_camera_definition_file_by_ftp(system);
    if (define_data.size() > 0) {
        camera.set_definition_data(define_data);
        do_camear_settings(camera);
    }
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}

static void do_camera_operation(mavsdk::Camera &camera) {
    auto operation_result = camera.format_storage(1);
    std::cout << "format storage result : " << operation_result << std::endl;

    operation_result = camera.take_photo();
    std::cout << "take photo result : " << operation_result << std::endl;

    operation_result = camera.start_video();
    std::cout << "start video result : " << operation_result << std::endl;

    // operation_result = camera.stop_video();
    // std::cout << "stop video result : " << operation_result << std::endl;

    operation_result = camera.start_video_streaming(1);
    std::cout << "start video streaming result : " << operation_result << std::endl;

    operation_result = camera.stop_video_streaming(1);
    std::cout << "stop video streaming result : " << operation_result << std::endl;

    operation_result = camera.set_mode(mavsdk::Camera::Mode::Photo);
    std::cout << "Set camera to photo mode result : " << operation_result << std::endl;

    operation_result = camera.set_mode(mavsdk::Camera::Mode::Video);
    std::cout << "Set camera to video mode result : " << operation_result << std::endl;

    operation_result = camera.reset_settings();
    std::cout << "Reset camera settings result : " << operation_result << std::endl;
}

// demo for use mavlink ftp to download camera config file
static std::string download_camera_definition_file_by_ftp(std::shared_ptr<mavsdk::System> system) {
    auto ftp = mavsdk::Ftp{system};
    const std::string camera_define_file_name = "C10.xml";
    std::filesystem::path full_path = std::filesystem::current_path().append("build");

    std::cout << "Download camera define file to " << full_path << std::endl;
    auto prom = std::promise<std::string>{};
    auto fut = prom.get_future();
    ftp.download_async(camera_define_file_name, full_path,
                       [&full_path, &camera_define_file_name, &prom](
                           mavsdk::Ftp::Result result, mavsdk::Ftp::ProgressData progress_data) {
                           if (result == mavsdk::Ftp::Result::Success) {
                               std::cout << "download camera config file success";
                               std::string file_path_with_name = full_path;
                               file_path_with_name += "/" + camera_define_file_name;
                               std::cout << file_path_with_name << std::endl;
                               std::ifstream file_stream(file_path_with_name);
                               std::stringstream buffer;
                               buffer << file_stream.rdbuf();
                               prom.set_value(buffer.str());
                           } else if (result != mavsdk::Ftp::Result::Next) {
                               std::cout << "Download definition file failed : " << result
                                         << std::endl;
                               prom.set_value("");
                           }
                       });

    if (fut.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
        std::cout << "Download camera define file failed" << std::endl;
        return "";
    }

    return fut.get();
}

static void do_camear_settings(mavsdk::Camera &camera) {
    std::vector<std::pair<std::string, std::string>> settings;
    settings.push_back({"CAM_WBMODE", "1"});
    settings.push_back({"CAM_EXPMODE", "0"});
    settings.push_back({"CAM_EV", "2.0"});
    settings.push_back({"CAM_EXPMODE", "1"});
    settings.push_back({"CAM_SHUTTERSPD", "0.016666"});
    settings.push_back({"CAM_ISO", "6400"});

    for (auto &it : settings) {
        set_camera_settings(camera, it.first, it.second);
        auto value = get_camera_setting(camera, it.first);
        if (value.find(it.second, 0) != 0) {
            std::cerr << "invalid result : " << it.first << " origin value " << it.second
                      << " new value " << value << std::endl;
            return;
        }
    }
    camera.set_mode(mavsdk::Camera::Mode::Video);
    set_camera_settings(camera, "CAM_VIDFMT", "2");
    set_camera_settings(camera, "CAM_VIDRES", "5");

    camera.set_mode(mavsdk::Camera::Mode::Photo);
    set_camera_settings(camera, "CAM_PHOTORATIO", "3");
}

static inline void set_camera_settings(mavsdk::Camera &camera, const std::string &name,
                                       const std::string &value) {
    mavsdk::Camera::Setting setting;
    setting.setting_id = name;
    setting.option.option_id = value;
    auto operation_result = camera.set_setting(setting);
    std::cout << "set " << name << " value : " << value << " result : " << operation_result
              << std::endl;
}

static inline std::string get_camera_setting(mavsdk::Camera &camera, const std::string &name) {
    mavsdk::Camera::Setting setting;
    setting.setting_id = name;
    auto [result, out_setting] = camera.get_setting(setting);
    return out_setting.option.option_id;
}