// use LPCSTR or LPCWSTR
#define GAME_ENGINE_MY_LPCSTR LPCSTR

/*
WARNINGS:
Dot::get_len() uses hypot which is very slow
*/

/*
ESC = exit
ENTER = windows mode
TAB = debug mode
K = locator visibility
BUTTON_F = fps mode
UP, DOWN = render_scale
*/

#include <windows.h>

#undef max
#undef min

bool camera_is_static = false;
bool debug_mode = true;
bool show_locator = false;
bool running = true;

bool show_console = true;
bool show_cursor = true;
bool show_fps = true;

bool fullscreen_mode = true;

#include "Objects\objects.h"
#include "utils.h"
// don't delete this
#include "Sprites\sprite.cpp"

Camera camera;

Render_state render_state;

Dot arena_half_size;

#include "Render\render.cpp"
// don't delete this
#include "Game\game.cpp"

class engine_app {
    HINSTANCE hInstance;  // дескриптор указанного модуля
    WNDCLASS window_class{sizeof(WNDCLASS)};
    HWND window;

    // дескриптор устройства (DC) для клиентской области указанного окна или для
    // всего экрана Используется в последующих функциях GDI для рисования в DС
    HDC hdc;

    Input input;

public:
    engine_app() {
        hInstance = GetModuleHandle(nullptr);

        // Create Window class
        {
            window_class.style = CS_HREDRAW | CS_VREDRAW;
            window_class.lpszClassName =
                GAME_ENGINE_MY_LPCSTR("Game Window Class");
            window_class.lpfnWndProc = window_callback;
            window_class.hIcon = static_cast<HICON>(LoadImage(
                nullptr, GAME_ENGINE_MY_LPCSTR("apple.ico"), IMAGE_ICON, 0, 0,
                LR_LOADFROMFILE
            ));

            window_class.cbClsExtra = 0;
            window_class.cbWndExtra = sizeof(LONG_PTR);
            window_class.hInstance = hInstance;

            window_class.hbrBackground = nullptr;
            window_class.lpszMenuName = nullptr;
            window_class.hCursor = LoadCursor(nullptr, IDI_APPLICATION);
        }

        // Register class
        RegisterClass(&window_class);

        // Create window
        window = CreateWindow(
            window_class.lpszClassName,
            reinterpret_cast<GAME_ENGINE_MY_LPCSTR>("C++ Game Engine"),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 0,
            0, nullptr, nullptr, hInstance, nullptr
        );

        hdc = GetDC(window);
    }

    // симулирует один игровой кадр
    // delta_time = время между кадрами
    void simulate_frame(efloat delta_time) {
        update_controls();

        simulate_game(input, delta_time, [&]() -> void {
            fullscreen_mode = !fullscreen_mode;

            if (fullscreen_mode) {
                set_fullscreen_mode();
            } else {
                set_window_mode();
            }
        });

        // update fps
        {
            static efloat global_time_accum = 0;
            global_time_accum += delta_time;

            static int summary_fps = 0;
            summary_fps++;

            static int visible_fps = 0;

            static int frame_accum = 0;
            frame_accum++;

            static efloat frame_time_accum = 0;
            frame_time_accum += delta_time;

            if (frame_time_accum > 0.5) {
                visible_fps = frame_accum * 2;
                frame_time_accum = 0;
                frame_accum = 0;
            }

            if (show_fps) {
                draw_object(
                    static_cast<int>(summary_fps / global_time_accum),
                    Dot(5, 12) - arena_half_size, 0.5, WHITE
                );

                draw_object(
                    global_time_accum, Dot(20, 12) - arena_half_size, 0.5, WHITE
                );

                draw_object(
                    visible_fps, Dot(5, 5) - arena_half_size, 0.5, WHITE
                );

                draw_object(
                    static_cast<int>(delta_time * 1000),
                    Dot(20, 5) - arena_half_size, 0.5, WHITE
                );

                draw_object(
                    to_string(render_state.height()) + "x" +
                        to_string(render_state.width()),
                    Dot(30, 5) - arena_half_size, 0.5, WHITE
                );

                // draw_object(
                //     static_cast<int>(game_engine_time_for_calc * 1000),
                //     Dot(60, 5) - arena_half_size, 0.5, RED
                //);
                game_engine_time_for_calc = 0;
            }
        }

        release_frame();
    }

    // SetWindowLong WS макросы
    //
    // WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX | WS_SIZEBOX
    // WS_CAPTION создает верхнюю панель окна и убирает окно из fullscreen
    // WS_SYSMENU создает "крестик" на панели окна
    // WS_TABSTOP создает "квадратик" на панели окна
    // WS_MINIMIZEBOX создает кнопку сворачивания окна
    // WS_SIZEBOX создает бох для изменения размеров окна
    // вместе они создают обычное окно

    void set_fullscreen_mode() {
        SetWindowLong(
            window, GWL_STYLE,
            GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW
        );
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfo(
            MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi
        );
        SetWindowPos(
            window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED
        );
        EnableWindow(window, true);
    }

    void set_window_mode() {
        SetWindowLong(
            window, GWL_STYLE,
            WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX | WS_SIZEBOX
        );
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfo(
            MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi
        );
        SetWindowPos(
            window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_SHOWWINDOW
        );
        EnableWindow(window, true);
    }

private:
    // передает кадр ОС, чтобы та вывела его на монитор
    void release_frame() {
        StretchDIBits(
            hdc, 0, 0, static_cast<int>(render_state.width()),
            static_cast<int>(render_state.height()), 0, 0,
            static_cast<int>(render_state.width()),
            static_cast<int>(render_state.height()),
            reinterpret_cast<void *>(render_state.render_memory()),
            &render_state.bitmap_info(), DIB_RGB_COLORS, SRCCOPY
        );
    }

    // обновляет состояние окна:
    // окно закрыли, изменили размер
    static LRESULT CALLBACK
    window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        LRESULT result = 0;
        switch (uMsg) {
            case WM_CLOSE:
            case WM_DESTROY: {
                running = false;
            } break;
            case WM_SIZE: {
                // get rect window
                {
                    RECT rect;
                    GetClientRect(hwnd, &rect);
                    render_state.resize(rect.right, rect.bottom);
                }

                // relax scaling
                scale_factor = render_state.height() * render_scale;
#ifndef GAME_ENGINE_STANDARD_RENDER_SYSTEM
                clear_sprites_cache();
#endif

                // relax arena
                arena_half_size = Dot(static_cast<efloat>(render_state.width()
                                      ) / scale_factor,
                                      static_cast<efloat>(1) / render_scale) *
                                  0.5;

            } break;
            default: {
                result = DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
        }
        return result;
    }

    // обновляет входные данные с клавиатуры и мыши
    void update_controls() {
        // обнуляем значение нажатия кнопки
        for (int i = 0; i < BUTTON_COUNT; i++) {
            input.set_button(
                button_t(i), input.button_is_down(button_t(i)), false
            );
        }

        auto button_up = [&](button_t b) -> void {
            input.set_button(b, false, true);
        };
        auto button_down = [&](button_t b) -> void {
            input.set_button(b, true, true);
        };

        bool isPeekMessage = false;

        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
            isPeekMessage = true;

            switch (message.message) {
                case WM_LBUTTONUP: {
                    button_up(BUTTON_MOUSE_L);
                } break;
                case WM_LBUTTONDOWN: {
                    button_down(BUTTON_MOUSE_L);
                } break;
                case WM_RBUTTONUP: {
                    button_up(BUTTON_MOUSE_R);
                } break;
                case WM_RBUTTONDOWN: {
                    button_down(BUTTON_MOUSE_R);
                } break;
                case WM_KEYUP:
                case WM_KEYDOWN: {
                    u64 vk_code = message.wParam;
                    bool is_down = (message.message == WM_KEYDOWN);

#define update_button(b, vk)                                                \
    case vk: {                                                              \
        input.set_button(b, is_down, (is_down != input.button_is_down(b))); \
    } break;

                    switch (vk_code) {
                        update_button(BUTTON_UP, VK_UP);
                        update_button(BUTTON_DOWN, VK_DOWN);
                        update_button(BUTTON_W, 'W');
                        update_button(BUTTON_S, 'S');
                        update_button(BUTTON_D, 'D');
                        update_button(BUTTON_A, 'A');
                        update_button(BUTTON_B, 'B');
                        update_button(BUTTON_C, 'C');
                        update_button(BUTTON_E, 'E');
                        update_button(BUTTON_F, 'F');
                        update_button(BUTTON_J, 'J');
                        update_button(BUTTON_K, 'K');
                        update_button(BUTTON_DEL, VK_DELETE);
                        update_button(BUTTON_LEFT, VK_LEFT);
                        update_button(BUTTON_RIGHT, VK_RIGHT);
                        update_button(BUTTON_ENTER, VK_RETURN);
                        update_button(BUTTON_ESC, VK_ESCAPE);
                        update_button(BUTTON_TAB, VK_TAB);
                        update_button(BUTTON_SPACE, VK_SPACE);
                        update_button(BUTTON_0, VK_NUMPAD0);
                        update_button(BUTTON_1, VK_NUMPAD1);
                        update_button(BUTTON_2, VK_NUMPAD2);
                        update_button(BUTTON_3, VK_NUMPAD3);
                        update_button(BUTTON_4, VK_NUMPAD4);
                        update_button(BUTTON_5, VK_NUMPAD5);
                        update_button(BUTTON_6, VK_NUMPAD6);
                        update_button(BUTTON_7, VK_NUMPAD7);
                        update_button(BUTTON_8, VK_NUMPAD8);
                        update_button(BUTTON_9, VK_NUMPAD9);
                        default: {
                        }
                    }

#undef update_button
                } break;
                default: {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
            }
        }

        if (isPeekMessage) {
            // mouse update

            RECT rect;
            GetWindowRect(window, &rect);

            mouse.pos = Dot(static_cast<double>(message.pt.x) -
                                std::max<int>(0, rect.left) + 0.2,
                            static_cast<double>(rect.bottom) - message.pt.y) /
                            scale_factor -
                        arena_half_size;
        }
    }
};

int main() {
    // initialize
    {
        std::cout << "performance_frequency: " << performance_frequency
                  << std::endl;

        ShowWindow(GetConsoleWindow(), show_console ? SW_SHOW : SW_HIDE);
        ShowCursor(show_cursor);

        read_sprites();
        read_spritesheets();

        build_world();

        init_render_threads();
    }

    engine_app eng;
    {
        if (fullscreen_mode) {
            eng.set_fullscreen_mode();
        } else {
            eng.set_window_mode();
        }
    }

    u64 time_tick_global_start = get_ticks();
    u64 time_tick_prev = time_tick_global_start;

    efloat delta_time = 0;

    while (running) {
        eng.simulate_frame(delta_time);

        // update time
        {
            u64 cur_time_tick = get_ticks();
            delta_time = static_cast<efloat>(cur_time_tick - time_tick_prev) /
                         performance_frequency;
            time_tick_prev = cur_time_tick;
        }
    }

    join_all_render_threads();
    return 0;
}
