#include <iostream>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <chrono>

constexpr auto DEVICE = "/dev/input/event22";

int main() {
    libevdev *dev = NULL;
    libevdev_uinput *uidev = NULL;
    int fd;
    int rc = 1;

    // Open the real device
    fd = open(DEVICE, O_RDONLY);
    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        exit(1);
    }
    printf("Input device name: \"%s\"\n", libevdev_get_name(dev));

    // Grab the device so events don't go to the system
    rc = libevdev_grab(dev, LIBEVDEV_GRAB);
    if (rc < 0) {
        fprintf(stderr, "Failed to grab device (%s)\n", strerror(-rc));
        exit(1);
    }

    // Create a virtual device to emit filtered events
    rc = libevdev_uinput_create_from_device(dev,
                                            LIBEVDEV_UINPUT_OPEN_MANAGED,
                                            &uidev);
    if (rc < 0) {
        fprintf(stderr, "Failed to create uinput device (%s)\n", strerror(-rc));
        exit(1);
    }

    printf("Virtual device created at: %s\n", libevdev_uinput_get_devnode(uidev));

    // Dead zone settings
    constexpr int DEAD_ZONE = 60; // Ignore movements smaller than this (3x15)
    constexpr int TIMEOUT_MS = 500; // Reset accumulated scroll after 500ms of no scrolling
    int accumulated = 0;
    auto last_scroll_time = std::chrono::steady_clock::now();

    do {
        input_event ev{};
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL | LIBEVDEV_READ_FLAG_BLOCKING, &ev);

        if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
            // Filter REL_WHEEL_HI_RES events
            if (ev.type == EV_REL && ev.code == REL_WHEEL_HI_RES) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_scroll_time).count();
                if (elapsed > TIMEOUT_MS) {
                    accumulated = 0; // Reset if too much time passed
                }
                last_scroll_time = now;
                accumulated += ev.value;
                // printf("Accumulated: %d (threshold: %d)\n", accumulated, DEAD_ZONE);

                // Only emit if we exceed the dead zone
                if (abs(accumulated) >= DEAD_ZONE) {
                    ev.value = accumulated;
                    libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
                    // printf("Emitting REL_WHEEL_HI_RES: %d\n", ev.value);
                    accumulated = 0; // Reset after emitting
                }
                // Don't pass through small movements
                continue;
            }

            // Pass through all other events unchanged
            libevdev_uinput_write_event(uidev, ev.type, ev.code, ev.value);
        }
    } while (rc == LIBEVDEV_READ_STATUS_SUCCESS || rc == -EAGAIN);

    libevdev_uinput_destroy(uidev);
    libevdev_free(dev);
    close(fd);
    return 0;
}
