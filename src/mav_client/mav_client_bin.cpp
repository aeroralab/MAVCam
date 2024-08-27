#include <filesystem>
#include <iostream>

#include "mav_client.h"
#include "version.h"

static auto constexpr default_connection = "udp://169.254.254.2:14550";
static auto constexpr default_rpc_port = 50051;
static std::string default_ftp_path = "";

static void usage(const char *bin_name);
static bool is_integer(const std::string &tested_integer);

int main(int argc, const char *argv[]) {
    std::string connection_url = default_connection;
    int rpc_port = default_rpc_port;
    bool use_local = false;
    default_ftp_path = std::filesystem::current_path();

    for (int i = 1; i < argc; i++) {
        const std::string current_arg = argv[i];

        if (current_arg == "-h" || current_arg == "--help") {
            usage(argv[0]);
            return 0;
        } else if (current_arg == "-v" || current_arg == "--version") {
            std::cout << "Mavlink client version : " << VERSION << std::endl;
            std::cout << "build time : " << BUILD_TIME << std::endl;
            return 0;
        } else if (current_arg == "-u") {
            if (argc <= i + 1) {
                usage(argv[0]);
                return 1;
            }
            connection_url = std::string(argv[i + 1]);
            i++;
        } else if (current_arg == "-l") {
            use_local = true;
        } else if (current_arg == "-r") {
            if (argc <= i + 1) {
                usage(argv[0]);
                return 1;
            }
            const std::string rpc_port_string(argv[i + 1]);
            i++;
            if (!is_integer(rpc_port_string)) {
                usage(argv[0]);
                return 1;
            }
            rpc_port = std::stoi(rpc_port_string);
        } else if (current_arg == "-f" || current_arg == "--ftp_path") {
            if (argc <= i + 1) {
                usage(argv[0]);
                return 1;
            }
            default_ftp_path = std::string(argv[i + 1]);
            i++;
        }
    }

    mav::MavClient client;
    if (!client.init(connection_url, use_local, rpc_port, default_ftp_path)) {
        std::cout << "Cannot init mav client " << connection_url << std::endl;
        return 1;
    }

    client.start_runloop();
    return 0;
}

void usage(const char *bin_name) {
    std::cout << "Usage: " << bin_name << " [Options] [Connection URL]" << '\n'
              << '\n'
              << "Connection URL format should be:" << '\n'
              << "\tSerial: serial:///path/to/serial/dev[:baudrate]" << '\n'
              << "\tUDP:    udp://[bind_host][:bind_port]" << '\n'
              << "\tTCP:    tcp://[server_host][:server_port]" << '\n'
              << '\n'
              << "Options:" << '\n'
              << "\t-h | --help    : show this help" << '\n'
              << "\t-v | --version : show version information " << '\n'
              << "\t-u             : set the url on which the mavsdk server is running,"
              << " (default is " << default_connection << ")" << '\n'
              << "\t-l             : use local client" << '\n'
              << "\t-r             : set the remote port," << " (default is " << default_rpc_port
              << ")\n"
              << "\t-f | --ftp_path: set the ftp root path," << " (default is " << default_ftp_path
              << ")" << '\n';
}

bool is_integer(const std::string &tested_integer) {
    for (const auto &digit : tested_integer) {
        if (!std::isdigit(digit)) {
            return false;
        }
    }
    return true;
}
