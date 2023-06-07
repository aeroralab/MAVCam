#include <iostream>

#include "mid_server.h"

static auto constexpr default_rpc_port = 50051;

static void usage(const char *bin_name);
static bool is_integer(const std::string &tested_integer);

int main(int argc, const char *argv[]) {
    int rpc_port = default_rpc_port;

    for (int i = 1; i < argc; i++) {
        const std::string current_arg = argv[i];

        if (current_arg == "-h" || current_arg == "--help") {
            usage(argv[0]);
            return 0;
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
        }
    }

    mid::MidServer server;
    if (!server.init(rpc_port)) {
        std::cout << "Init rpc server failed";
        return 1;
    }

    server.start_runloop();
    return 0;
}

void usage(const char *bin_name) {
    std::cout << "Usage: " << bin_name << " [Options]" << '\n'
              << '\n'
              << "Options:" << '\n'
              << "  -h | --help : show this help" << '\n'
              << "  -r          : set the rpc port,\n"
              << "                (default is " << default_rpc_port << ")\n";
}

bool is_integer(const std::string &tested_integer) {
    for (const auto &digit : tested_integer) {
        if (!std::isdigit(digit)) {
            return false;
        }
    }

    return true;
}