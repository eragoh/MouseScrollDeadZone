cmake_minimum_required(VERSION 4.0)
project(mouse-scroll-dead-zone)

set(CMAKE_CXX_STANDARD 17)

# Find libevdev via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)

include_directories(${LIBEVDEV_INCLUDE_DIRS})
link_directories(${LIBEVDEV_LIBRARY_DIRS})

add_executable(mouse-scroll-dead-zone main.cpp)
target_link_libraries(mouse-scroll-dead-zone ${LIBEVDEV_LIBRARIES})
