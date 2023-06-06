#!/usr/bin/env bash

set -e

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
proto_dir="${script_dir}/../proto/protos"

build_dir="${script_dir}/../build"

third_party_dir="${build_dir}/third_party"
mavsdk_server_generated_dir="${script_dir}/../src/generated"
protoc_binary="${third_party_dir}/install/bin/protoc"
protoc_grpc_binary="${third_party_dir}/install/bin/grpc_cpp_plugin"

echo "Looking for ${protoc_binary}"
if ! command -v ${protoc_binary} > /dev/null; then
    echo "Falling back to looking for protoc in PATH"
    if ! protoc_binary="$(command -v protoc)"; then
        echo >&2 "No protoc binary found"
        echo >&2 "'protoc' not found"
        echo >&2 ""
        echo >&2 "You may want to run the CMake configure step first:"
        echo >&2 ""
        echo >&2 "    cmake -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H."
        echo >&2 ""
        echo >&2 "or set the build directory used with -b"
        echo >&2 ""
        usage
        exit 1
    fi
fi
echo "Found protoc ($(${protoc_binary} --version)): ${protoc_binary}"

echo "Looking for ${protoc_grpc_binary}"
if ! command -v ${protoc_grpc_binary} > /dev/null; then
echo "Falling back to looking for grpc_cpp_plugin in PATH"
    if ! protoc_grpc_binary="$(command -v grpc_cpp_plugin)"; then
        echo >&2 "No grpc_cpp_plugin binary found"
        echo >&2 "'grpc_cpp_plugin' not found"
        echo >&2 ""
        echo >&2 "You may want to run the CMake configure step first:"
        echo >&2 ""
        echo >&2 "    cmake -DBUILD_MAVSDK_SERVER=ON -Bbuild/default -H."
        echo >&2 ""
        echo >&2 "or set the build directory used with -b"
        echo >&2 ""
        usage
        exit 1
    fi
fi
echo "Found grpc_cpp_plugin: ${protoc_grpc_binary}"

command -v ${protoc_binary} > /dev/null && command -v ${protoc_grpc_binary} > /dev/null || {
    echo "-------------------------------"
    echo " Error"
    echo "-------------------------------"
}

mkdir -p ${mavsdk_server_generated_dir}

echo "Processing mavsdk_options.proto"
${protoc_binary} -I ${proto_dir} --cpp_out=${mavsdk_server_generated_dir} --grpc_out=${mavsdk_server_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/mavsdk_options.proto

plugin_list=("camera" "camera_server")
plugin_count=${#plugin_list[*]}

for ((i=0; i<${plugin_count}; i++));
do
    plugin=${plugin_list[$i]}
    echo "Processing ${plugin}/${plugin}.proto"

    ${protoc_binary} -I ${proto_dir} --cpp_out=${mavsdk_server_generated_dir} --grpc_out=${mavsdk_server_generated_dir} --plugin=protoc-gen-grpc=${protoc_grpc_binary} ${proto_dir}/${plugin}/${plugin}.proto
done