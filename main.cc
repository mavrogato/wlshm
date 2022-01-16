
#include <iostream>
#include <iomanip>
#include <string_view>
#include <cassert>
#include <wayland-client.h>

int main() {
    if (auto display = wl_display_connect(nullptr)) {
        assert(display);
        static wl_compositor* compositor = nullptr;
        static wl_shm* shm = nullptr;
        static wl_shell* shell = nullptr;
        static wl_seat* seat = nullptr;
        if (auto registry = wl_display_get_registry(display)) {
            assert(registry);
            static constexpr wl_registry_listener listener {
                .global = [](void* data,
                             wl_registry* registry,
                             uint32_t name,
                             char const* interface,
                             uint32_t version)
                noexcept
                {
                    if (std::string_view(interface) == wl_compositor_interface.name) {
                        compositor = (wl_compositor*) wl_registry_bind(registry,
                                                                       name,
                                                                       &wl_compositor_interface,
                                                                       version);
                    }
                    else if (std::string_view(interface) == wl_shm_interface.name) {
                        shm = (wl_shm*) wl_registry_bind(registry,
                                                         name,
                                                         &wl_shm_interface,
                                                         version);
                    }
                    else if (std::string_view(interface) == wl_shell_interface.name) {
                        shell = (wl_shell*) wl_registry_bind(registry,
                                                             name,
                                                             &wl_shell_interface,
                                                             version);
                    }
                    else if (std::string_view(interface) == wl_seat_interface.name) {
                        seat = (wl_seat*) wl_registry_bind(registry,
                                                           name,
                                                           &wl_seat_interface,
                                                           version);
                    }
                },
            };
            wl_registry_add_listener(registry, &listener, nullptr);
            wl_display_roundtrip(display);
            assert(compositor);
            assert(shm);
            assert(shell);
            assert(seat);
            wl_registry_destroy(registry);
        }
        {
            static constexpr wl_shm_listener listener {
                .format = [](auto, auto, uint32_t format) noexcept {
                    std::cerr << "format: " << std::hex << format << std::endl;
                },
            };
            wl_shm_add_listener(shm, &listener, nullptr);
        }
        {
            static constexpr wl_seat_listener listener {
                .capabilities = [](auto, auto, uint32_t caps) noexcept {
                    if (caps & WL_SEAT_CAPABILITY_POINTER) {
                        std::cerr << "*** pointer device found." << std::endl;
                    }
                    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
                        std::cerr << "*** keyboard device found." << std::endl;
                    }
                    if (caps & WL_SEAT_CAPABILITY_TOUCH) {
                        std::cerr << "*** touch device found." << std::endl;
                    }
                },
                .name = [](auto, auto, char const* name) noexcept {
                    std::cout << "*** name: " << name << std::endl;
                },
            };
            wl_seat_add_listener(seat, &listener, nullptr);
        }
        wl_display_roundtrip(display);
        wl_seat_destroy(seat);
        wl_shell_destroy(shell);
        wl_shm_destroy(shm);
        wl_compositor_destroy(compositor);
        wl_display_disconnect(display);
    }
    return 0;
}
