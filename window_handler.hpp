#include "main_header.hpp"

void relax_scaling_after_change_window_scaling(Dot &cursor_pos) {
  global_variables::scale_factor =
      global_variables::render_state.height() * global_variables::render_scale;

  // relax arena
  global_variables::arena_half_size =
      Dot(static_cast<efloat>(global_variables::render_state.width()) /
              global_variables::scale_factor,
          static_cast<efloat>(1) / global_variables::render_scale) *
      0.5;

  cursor_pos /= global_variables::scale_factor;
  cursor_pos -= global_variables::arena_half_size;
}

void increase_window_scaling(Dot &cursor_pos, efloat delta_time) {
  cursor_pos += global_variables::arena_half_size;
  cursor_pos *= global_variables::scale_factor;

  global_variables::render_scale -=
      global_variables::render_scale * 0.01 * delta_time * 100;

  relax_scaling_after_change_window_scaling(cursor_pos);
}

void decrease_window_scaling(Dot &cursor_pos, efloat delta_time) {
  cursor_pos += global_variables::arena_half_size;
  cursor_pos *= global_variables::scale_factor;

  global_variables::render_scale +=
      global_variables::render_scale * 0.01 * delta_time * 100;

  relax_scaling_after_change_window_scaling(cursor_pos);
}

// draw fps and others useful info
void draw_debug_info(efloat delta_time, const Cursor &cursor) {
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

#ifdef GAME_ENGINE_DRAW_FPS_PLOT
  static std::vector<efloat> vec_delta_times;
  {
    vec_delta_times.push_back(delta_time);
    if (vec_delta_times.size() > 1500) {
      vec_delta_times.erase(vec_delta_times.begin());
    }
  }
#endif

  if (global_variables::show_fps) {
#ifdef GAME_ENGINE_DRAW_FPS_PLOT
    Dot pos(60, -global_variables::arena_half_size.y + 5);
    for (int i = static_cast<int>(vec_delta_times.size()) - 1; i >= 0; i--) {
      draw_rect(pos + Dot(0, vec_delta_times[i] * 1000), Dot(0.1, 0.1), RED);
      draw_rect(pos, Dot(0.1, 0.1), WHITE);
      pos.x -= 0.05;
    }
#endif

    draw_object(static_cast<int>(summary_fps / global_time_accum),
                Dot(5, 12) - global_variables::arena_half_size, 0.5, WHITE);

    draw_object(global_time_accum,
                Dot(20, 12) - global_variables::arena_half_size, 0.5, WHITE);

    draw_object(visible_fps, Dot(5, 5) - global_variables::arena_half_size, 0.5,
                WHITE);

    draw_object(static_cast<int>(delta_time * 1000),
                Dot(20, 5) - global_variables::arena_half_size, 0.5, WHITE);

    draw_object(to_string(global_variables::render_state.height()) + "x" +
                    to_string(global_variables::render_state.width()),
                Dot(30, 5) - global_variables::arena_half_size, 0.5, WHITE);

    Dot cursor_pos = cursor.pos;
    cursor_pos += global_variables::camera.pos;
    draw_object(cursor_pos, Dot(60, 5) - global_variables::arena_half_size, 0.5,
                WHITE);
  }
}

// общается с ОС Windows, чтобы обновлять и хранить состояние окна, кнопок,
// курсора
class WindowHandler {
  HINSTANCE hInstance; // дескриптор указанного модуля
  WNDCLASS window_class{sizeof(WNDCLASS)};
  HWND window;

  // Дескриптор устройства (DC) для клиентской области указанного окна или для
  // всего экрана. Используется в последующих функциях GDI для рисования в DС
  HDC hdc;

public:
  Input input;

  Cursor cursor;

  WindowHandler() : cursor(Cursor(SP_CURSOR, SP_FOCUS_CURSOR, 0.09)) {
    hInstance = GetModuleHandle(nullptr);

    // Create Window class
    {
      window_class.style = CS_HREDRAW | CS_VREDRAW;
      window_class.lpszClassName = GAME_ENGINE_MY_LPCSTR("Game Window Class");
      window_class.lpfnWndProc = window_callback;
      // window_class.hIcon = static_cast<HICON>(
      //     LoadImage(nullptr, GAME_ENGINE_MY_LPCSTR("apple.ico"), IMAGE_ICON,
      //     0, 0, LR_LOADFROMFILE)
      //);

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
    window =
        CreateWindow(window_class.lpszClassName,
                     reinterpret_cast<GAME_ENGINE_MY_LPCSTR>("C++ Game Engine"),
                     WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
                     CW_USEDEFAULT, 0, 0, nullptr, nullptr, hInstance, nullptr);

    hdc = GetDC(window);

    // Update window mode
    if (global_variables::fullscreen_mode) {
      set_fullscreen_mode();
    } else {
      set_window_mode();
    }
  }

  WindowHandler(const WindowHandler &other) = delete;
  WindowHandler(WindowHandler &&other) = delete;
  WindowHandler &operator=(const WindowHandler &other) = delete;
  WindowHandler &operator=(WindowHandler &&other) = delete;

  // обновляет окно, курсор, кнопки
  // обновляет входные данные с клавиатуры и мыши
  void update_controls() {
    input.previous = input.current;

    auto button_up = [&input = input](button_t button_id) -> void {
      input.current.set_button(button_id, false);
    };
    auto button_down = [&input = input](button_t button_id) -> void {
      input.current.set_button(button_id, true);
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
        uint64_t vk_code = message.wParam;
        bool is_down = (message.message == WM_KEYDOWN);

#define update_button(b, vk)                                                   \
  case vk: {                                                                   \
    input.current.set_button(b, is_down);                                      \
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
          update_button(BUTTON_G, 'G');
          update_button(BUTTON_H, 'H');
          update_button(BUTTON_I, 'I');
          update_button(BUTTON_J, 'J');
          update_button(BUTTON_K, 'K');
          update_button(BUTTON_L, 'L');
          update_button(BUTTON_M, 'M');
          update_button(BUTTON_N, 'N');
          update_button(BUTTON_O, 'O');
          update_button(BUTTON_P, 'P');
          update_button(BUTTON_Q, 'Q');
          update_button(BUTTON_R, 'R');
          update_button(BUTTON_T, 'T');
          update_button(BUTTON_U, 'U');
          update_button(BUTTON_V, 'V');
          update_button(BUTTON_X, 'X');
          update_button(BUTTON_Y, 'Y');
          update_button(BUTTON_Z, 'Z');
          update_button(BUTTON_DEL, VK_DELETE);
          update_button(BUTTON_LEFT, VK_LEFT);
          update_button(BUTTON_RIGHT, VK_RIGHT);
          update_button(BUTTON_ENTER, VK_RETURN);
          update_button(BUTTON_ESC, VK_ESCAPE);
          update_button(BUTTON_TAB, VK_TAB);
          update_button(BUTTON_SPACE, VK_SPACE);
          update_button(BUTTON_SHIFT, VK_SHIFT);
          update_button(BUTTON_NUMPAD0, VK_NUMPAD0);
          update_button(BUTTON_NUMPAD1, VK_NUMPAD1);
          update_button(BUTTON_NUMPAD2, VK_NUMPAD2);
          update_button(BUTTON_NUMPAD3, VK_NUMPAD3);
          update_button(BUTTON_NUMPAD4, VK_NUMPAD4);
          update_button(BUTTON_NUMPAD5, VK_NUMPAD5);
          update_button(BUTTON_NUMPAD6, VK_NUMPAD6);
          update_button(BUTTON_NUMPAD7, VK_NUMPAD7);
          update_button(BUTTON_NUMPAD8, VK_NUMPAD8);
          update_button(BUTTON_NUMPAD9, VK_NUMPAD9);
          update_button(BUTTON_1, '1');
          update_button(BUTTON_2, '2');
          update_button(BUTTON_3, '3');
          update_button(BUTTON_4, '4');
          update_button(BUTTON_5, '5');
          update_button(BUTTON_6, '6');
          update_button(BUTTON_7, '7');
          update_button(BUTTON_8, '8');
          update_button(BUTTON_9, '9');
          update_button(BUTTON_0, '0');
          update_button(BUTTON_MINUS, VK_OEM_MINUS);
          update_button(BUTTON_PLUS, VK_OEM_PLUS);
          update_button(BUTTON_COLON, VK_OEM_1);
          update_button(BUTTON_DOT, VK_OEM_PERIOD);
          update_button(BUTTON_COMMA, VK_OEM_COMMA);
          update_button(BUTTON_QUESTION_MARK, VK_OEM_2);
          update_button(BUTTON_BACKSPACE, VK_BACK);
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
      // cursor update

      RECT rect;
      GetWindowRect(window, &rect);

      cursor.pos = Dot(static_cast<double>(message.pt.x) -
                           std::max<int>(0, rect.left) + 0.2,
                       static_cast<double>(rect.bottom) - message.pt.y) /
                       global_variables::scale_factor -
                   global_variables::arena_half_size;
    }
  }

  void simulate_input(efloat delta_time) {

    if (PRESSED(BUTTON_ENTER)) {
      global_variables::fullscreen_mode = !global_variables::fullscreen_mode;

      if (global_variables::fullscreen_mode) {
        set_fullscreen_mode();
      } else {
        set_window_mode();
      }
    }

    if (PRESSED(BUTTON_TAB)) {
      global_variables::debug_mode = !global_variables::debug_mode;
    }

    if (PRESSED(BUTTON_K)) {
      global_variables::show_locator = !global_variables::show_locator;
    }

    if (PRESSED(BUTTON_F)) {
      global_variables::show_fps = !global_variables::show_fps;
    }

    // update render_scale
    {
      if (IS_DOWN(BUTTON_UP)) {
        increase_window_scaling(cursor.pos, delta_time);
      }

      if (IS_DOWN(BUTTON_DOWN)) {
        decrease_window_scaling(cursor.pos, delta_time);
      }
    }

    cursor.simulate(input);
  }

  // рисует игру, полезную для дебага информацию, курсор
  void draw_frame(efloat delta_time, int client_id) {
#ifndef LEVEL_MAKER_MODE
    draw_game(client_id);
#endif
    draw_debug_info(delta_time, cursor);
    cursor.draw();

#if MULTITHREAD_RENDER != 0
    wait_all_render_threads();
#endif
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
    SetWindowLong(window, GWL_STYLE,
                  GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
    SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    EnableWindow(window, true);
  }

  void set_window_mode() {
    SetWindowLong(window, GWL_STYLE,
                  WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX |
                      WS_SIZEBOX);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
    SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_SHOWWINDOW);
    EnableWindow(window, true);
  }

  // передает кадр ОС, чтобы та вывела его на монитор
  void release_frame() {
    StretchDIBits(
        hdc, 0, 0, static_cast<int>(global_variables::render_state.width()),
        static_cast<int>(global_variables::render_state.height()), 0, 0,
        static_cast<int>(global_variables::render_state.width()),
        static_cast<int>(global_variables::render_state.height()),
        reinterpret_cast<void *>(
            global_variables::render_state.render_memory()),
        &global_variables::render_state.bitmap_info(), DIB_RGB_COLORS, SRCCOPY);
  }

private:
  // обновляет состояние окна:
  // окно закрыли, изменили размер
  static LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam) {
    LRESULT result = 0;
    switch (uMsg) {
    case WM_CLOSE:
    case WM_DESTROY: {
      global_variables::running = false;
    } break;
    case WM_SIZE: {
      // get rect window
      {
        RECT rect;
        GetClientRect(hwnd, &rect);
        global_variables::render_state.resize(rect.right, rect.bottom);
      }

      // relax scaling
      global_variables::scale_factor = global_variables::render_state.height() *
                                       global_variables::render_scale;

      // relax arena
      global_variables::arena_half_size =
          Dot(static_cast<efloat>(global_variables::render_state.width()) /
                  global_variables::scale_factor,
              static_cast<efloat>(1) / global_variables::render_scale) *
          0.5;

    } break;
    case WM_MOUSEWHEEL: {
      global_variables::mouse_wheel += GET_WHEEL_DELTA_WPARAM(wParam);
    } break;
    default: {
      result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return result;
  }
};
