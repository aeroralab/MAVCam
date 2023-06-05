
project(mid_client)

set(MID_CLIENT_SOURCES
    mid_client_bin.cpp
    mid_client.cpp
)

set(EXECUTABLE_NAME mid_client)

add_executable(${EXECUTABLE_NAME}
    ${MID_CLIENT_SOURCES}
)

find_package(MAVSDK REQUIRED)

target_link_libraries(${EXECUTABLE_NAME}
    MAVSDK::mavsdk
)