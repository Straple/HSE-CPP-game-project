#define GAME_ENGINE_TAG "lunar oblivion engine v1.0"

/*			


										+------+
										|dot   |
				+----------+			|rect  |
				|   file   |			|line  |----------------+
				|png or eng|			|circle|				|
				+----------+			+------+				|
					|						|					|
					|						|					|
	+-----+		+------+			+----------------+		+---------+		+----+		+---------------+
	|color|-----|sprite|			|geometry objects|		|collision|		|time|		|platform common|
	+-----+		+------+			+----------------+		+---------+		+----+		+---------------+
					|						|					|				|			|
					|						|					|				|			|
					|				+-------------+				|				|			|
					+---------------|engine object|-------------+---------------+-----------+
									+-------------+																		+------+
											|																	+-------|logics|
											|																	|		+------+
											|																	|
			+----+					++===========++															+------------+				+------+
			|GAME|<=================||GAME ENGINE||---------------------------------------------------------|game objects|--------------|sprite|
			+----+					++===========++															+------------+				+------+
											|	|																	|
											|	+-----------------------------------+								+-------+
											|										|										|
					+------------+		+------+								+-------+								+---------+
					|render state|------|render|						+-------|physics|-----------+					|collision|
					+------------+		+------+						|		+-------+			|					+---------+
											|							|			|				|
											|							|			|				|
										+------------+				+----+		+---------+		+----+
										|basic render|				|time|		|collision|		|move|
										+------------+				+----+		+---------+		+----+
											|
											|
									+---------------+
					+---------------|render geometry|-------------------+
					|				+---------------+					|
					|						|							|
					|						|							|
				+-----------+			+-------------+			+---------------+
				|render text|			|render sprite|			|render phenomen|
				+-----------+			+-------------+			+---------------+
											|
											|
										+---------+
										|animation|
										+---------+
 
 
*/

/*
*       y ^
*         |
*         |
*         |
*         |
*         |
*         +----------->
*         O           x
*
*/

/*
*         left                                            right
*           |                                               |
*     top --+-----------------------------------------------+
*           |                                               |
*           |                                               |
*           |                                               |
*           |                                               |
*           |                                               |
*           |                                               |
*           |                                               |
*           |                                               |
*           |                                               |
*           |                                               |
*  bottom --+-----------------------------------------------+
*
*
*/

/*  
*   p0 = (x0, y0)
*   p1 = (x1, y1)
* 
*   p0---+
*   |    |
*   |    |
*   |    |
*   +----p1
*/

/*
ESC = exit
ENTER = windows mode
TAB = debug mode
K = locator visibility
BUTTON_F = fps mode
UP, DOWN = render_scale
*/

#define DEBUG_MODE

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// сетевой режим
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#undef max
#undef min

#include "utils.h"

#include "Objects/objects.h"

Engine_state eng_state(RUN_SETUP | CONSOLE_VIS | FULLSCREEN | CAMERA);

#include "Sprites/sprite.cpp"

Camera camera;

Render_state render_state;

dot arena_half_size;

int simulate_client(const char* ip);
int simulate_server();

#include "Serialize/serialize.h"

#include "Render/render.cpp"

#include "Game/game.cpp"

void update_controls(HWND& window, Input& input) {

	// обнуляем значение нажатия кнопки
	for (int i = 0; i < BUTTON_COUNT; i++) {
		input.set_button(button_t(i), input.button_is_down(button_t(i)), false);
	}

	auto button_up = [&input](button_t b) -> void {
		input.set_button(b, false, true);
	};

	auto button_down = [&input](button_t b) -> void {
		input.set_button(b, true, true);
	};

	bool isPeekMessage = false;

	MSG message;
	while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
		isPeekMessage = true;

		switch (message.message) {

		case WM_LBUTTONUP: {

			button_up(BUTTON_MOUSE_L);

		}break;

		case WM_LBUTTONDOWN: {

			button_down(BUTTON_MOUSE_L);

		}break;

		case WM_RBUTTONUP: {
			button_up(BUTTON_MOUSE_R);

		}break;

		case WM_RBUTTONDOWN: {

			button_down(BUTTON_MOUSE_R);

		}break;

		case WM_KEYUP:
		case WM_KEYDOWN: {

			u64 vk_code = message.wParam;
			bool is_down = (message.message == WM_KEYDOWN);


#define update_button(b, vk)\
case vk: {\
	input.set_button(b, is_down, (is_down != input.button_is_down(b)));\
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
			}

#undef update_button

		}break;

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

		mouse.pos = dot(
			static_cast<s64>(message.pt.x) - std::max<s32>(0, rect.left) + 0.2,
			static_cast<s64>(rect.bottom) - message.pt.y
		)
			/ scale_factor - arena_half_size;
	}
}

LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		eng_state.flip(RUN_SETUP);
	} break;

	case WM_SIZE: {

		// get rect window
		{
			RECT rect;
			GetClientRect(hwnd, &rect);

			render_state.resize(rect.right, rect.bottom);
		}

		// relax scaling
		scale_factor = render_state.height * render_scale;

		// relax arena
		arena_half_size = dot(static_cast<point_t>(render_state.width) / scale_factor, static_cast<point_t>(1) / render_scale) * 0.5;

	} break;

	default: {
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
	return result;
}


int simulate_client_or_single_player(bool is_client) {

	// clients objects

	WSADATA wsaData;

	char mem_pool[1024 * 32];
	memset(mem_pool, '0', sizeof(mem_pool));

	SOCKET socket_read;
	SOCKET socket_write;

	u8 player_id;

	if (is_client) {

		char ip[INET_ADDRSTRLEN + 1];
		{
			std::cout << "Введите ip адрес сервера: ";
			scanf("%s", ip);
			ip[INET_ADDRSTRLEN] = '\0';
		}

		// wsa startup
		{
			std::cout << "Запуск WSA...\n";
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				std::cout << "WSAStartup ошибка запуска: " << WSAGetLastError() << "\n";
				return -1;
			}
			std::cout << "Успешно\n\n";
		}

		{
			std::cout << "Создание сокета на чтение...\n";
			socket_read = ::socket(AF_INET, SOCK_STREAM, 0);
			if (socket_read == INVALID_SOCKET) {
				std::cout << "ошибка создания: " << WSAGetLastError() << "\n";
				WSACleanup();
				return -1;
			}
			std::cout << "Успешно\n\n";

			std::cout << "Создание сокета для записи...\n";
			socket_write = ::socket(AF_INET, SOCK_STREAM, 0);
			if (socket_write == INVALID_SOCKET) {
				std::cout << "Ошибка создания: " << WSAGetLastError() << "\n";
				WSACleanup();
				return -1;
			}
			std::cout << "Успешно\n\n";
		}

		struct sockaddr_in serv_addr;
		{
			memset(&serv_addr, '0', sizeof(serv_addr));

			serv_addr.sin_port = htons(5000);
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.S_un.S_addr = inet_addr(reinterpret_cast<const char*>(ip));

			if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) != 1) {
				std::cout << "inet_pton failed\n";
				WSACleanup();
				return -1;
			}
		}

		// socket_read
		{
			std::cout << "Подключение к серверу...\n";
			if (connect(socket_read, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
				std::cout << "Не удалось подключиться к серверу\n";
				closesocket(socket_read);
				WSACleanup();
				return -1;
			}
			std::cout << "Успешно\n\n";
		}

		// получаем id игрока и сокет чтения
		{
			// отправили сообщение, что у нас нет id игрока
			mem_pool[0] = -1;
			send(socket_read, mem_pool, 1, 0);

			// считали id игрока
			int n = recv(socket_read, mem_pool, sizeof(mem_pool) - 1, 0);
			player_id = mem_pool[0];

			mem_pool[0] = '\0';
		}

		// socket_write
		{

			std::cout << "Подключение к серверу...\n";
			if (connect(socket_write, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
				std::cout << "Не удалось подключиться к серверу\n";
				closesocket(socket_write);
				WSACleanup();
				return -1;
			}
			std::cout << "Успешно\n\n";
		}

		// подключаем к серверу сокет записи
		{
			mem_pool[0] = player_id;
			send(socket_write, mem_pool, 1, 0);

			// мы не ожидаем получить сообщение от сервера
			// Он просто должен определить нас и добавить этот сокет к игроку

			mem_pool[0] = '\0';
		}

		// registered
		{
			std::cout << "Регистрация: ";

			scanf("%s", &mem_pool);
			send(socket_write, mem_pool, strlen(mem_pool), 0);

			mem_pool[0] = '\0';
		}
	}


	//=========================================//
	//==============CREATE WINDOW==============//
	//=========================================//


	ShowWindow(GetConsoleWindow(), eng_state.show_console() ? SW_SHOW : SW_HIDE);

	ShowCursor(eng_state.show_cursor());

	read_sprites();

	// Create Window class
	WNDCLASS window_class = {};
	{
		window_class.style = CS_HREDRAW | CS_VREDRAW;
		window_class.lpszClassName = L"Game Window Class";
		window_class.lpfnWndProc = window_callback;
		window_class.hIcon = static_cast<HICON>(LoadImage(NULL, L"apple.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	}

	// Register class
	RegisterClass(&window_class);

	HINSTANCE hInstance = GetModuleHandle(NULL); // дескриптор указанного модуля

	// Create window
	HWND window = CreateWindow(window_class.lpszClassName, L"C++ Game Engine", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1600, 900, 0, 0, hInstance, 0);
	
	// SetWindowLong WS макросы
	// 
	// WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX | WS_SIZEBOX
	// WS_CAPTION создает верхнюю панель окна и убирает окно из fullscreen
	// WS_SYSMENU создает "крестик" на панели окна
	// WS_TABSTOP создает "квадратик" на панели окна
	// WS_MINIMIZEBOX создает кнопку сворачивания окна
	// WS_SIZEBOX создает бох для изменения размеров окна
	// вместе они создают обычное окно

	auto set_fullscreen_mod = [&]() {
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		EnableWindow(window, true);
	};
	
	auto set_window_mod = [&]() {
		SetWindowLong(window, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_TABSTOP | WS_MINIMIZEBOX | WS_SIZEBOX);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_SHOWWINDOW);
		EnableWindow(window, true);
	};

	if (eng_state.fullscreen_mode()) {
		set_fullscreen_mod();
	}
	else {
		set_window_mod();
	}

	// дескриптор устройства (DC) для клиентской области указанного окна или для всего экрана
	// Используется в последующих функциях GDI для рисования в DС
	HDC hdc = GetDC(window); 

	Input input;

	// получение статуса мира
	if(is_client) {
		clear_screen(0xff4d4d4d);
		draw_text_align("waiting for players...", dot(), 1, WHITE);

		// render screen
		{
			StretchDIBits(hdc, 0, 0, render_state.width, render_state.height,
				0, 0, render_state.width, render_state.height,
				reinterpret_cast<void*>(render_state.render_memory), &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		}

		int n = recv(socket_read, mem_pool, sizeof(mem_pool), 0);

		set_world_state(std::string(mem_pool + 1, n - 1));

		mem_pool[0] = '\0';
	}

	if (!is_client) {
		build_world();
		Players.resize(1);
		Players[0].name[0] = '\0';
	}

	point_t delta_time = 0;
	auto update_delta_time = [&delta_time]() -> void {
		static Timer timer;

		delta_time = timer.time();
		timer.reset();
	};

	update_delta_time();

	Timer global_time;

	std::string win_player;

	while (eng_state.run()) {

		update_controls(window, input);

		if (is_client) {
			if (!win_player.empty()) {
				clear_screen(0xff4d4d4d);
				draw_text_align("WINNER", dot(0, 20), 3, GREEN);
				draw_text_align(win_player.c_str(), dot(0, -10), 1.75, GREEN);

				draw_text("pressed button ESC to exit", dot(-arena_half_size.x + 10, -arena_half_size.y + 10), 0.75, WHITE);

				if (pressed(BUTTON_ESC)) {
					eng_state.flip(RUN_SETUP);
				}
			}
			else {
				send(socket_write, reinterpret_cast<const char*>(&input), sizeof(input), 0);

				simulate_input(input, [&]() -> void {
					eng_state.flip(FULLSCREEN);

					if (eng_state.fullscreen_mode()) {
						set_fullscreen_mod();
					}
					else {
						set_window_mod();
					}
					});

				camera.simulate(Players[find_player_ind(player_id)].pos, delta_time);

				int n = recv(socket_read, mem_pool, sizeof(mem_pool), 0);

				if (mem_pool[0] == 't') { // have winner
					mem_pool[n] = '\0';
					win_player = mem_pool + 1;

					continue;
				}
				set_world_state(std::string(mem_pool + 1, n - 1));

				mem_pool[0] = '\0';

				render_game(player_id);
			}
		}
		else {
			simulate_game(input, delta_time, [&]() -> void {
				eng_state.flip(FULLSCREEN);

				if (eng_state.fullscreen_mode()) {
					set_fullscreen_mod();
				}
				else {
					set_window_mod();
				}
			});
		}

		// update fps
		{
			static s32 fps = 0;
			static s32 frame_count = 0;
			static point_t frame_time_accum = 0;

			frame_count++;
			frame_time_accum += delta_time;
			if (frame_time_accum > 0.5) {
				fps = frame_count * 2;
				frame_time_accum = 0;
				frame_count = 0;
			}

			if (eng_state.show_fps()) {
				draw_object(fps, dot(5, 5) - arena_half_size, 0.5, WHITE);

				draw_object(int(delta_time * 1000), dot(20, 5) - arena_half_size, 0.5, WHITE);

				draw_object(global_time, dot(35, 5) - arena_half_size, 0.5, WHITE);
			}
		}

		// render screen
		{
			draw_text(GAME_ENGINE_TAG, dot(arena_half_size.x - 50, -arena_half_size.y + 5), 0.33, WHITE);

			StretchDIBits(hdc, 0, 0, render_state.width, render_state.height,
				0, 0, render_state.width, render_state.height,
				reinterpret_cast<void*>(render_state.render_memory), &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		}

		update_delta_time();
	}

	return 0;
}

int simulate_server() {

	WSADATA wsaData;
	{
		std::cout << "Запуск WSA...\n";
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cout << "WSAStartup ошибка запуска: " << WSAGetLastError() << "\n";
			return -1;
		}
		std::cout << "Успешно\n\n";
	}

	sockaddr_in serv_addr;
	{
		memset(&serv_addr, '0', sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(5000);
	}

	int Socket;
	{
		std::cout << "Создание сокета...\n";
		Socket = socket(AF_INET, SOCK_STREAM, 0);
		if (Socket == INVALID_SOCKET) {
			std::cout << "ошибка создание сокета: " << WSAGetLastError() << "\n";
			WSACleanup();
			return -1;
		}
		std::cout << "Успешно\n\n";

		//=================================//
		//=================================//

		std::cout << "Связываем сокет с портом...\n";
		if (bind(Socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
			std::cout << "ошибка привязки: " << WSAGetLastError() << "\n";
			closesocket(Socket);
			WSACleanup();
			return -1;
		}
		std::cout << "Успешно\n\n";

		listen(Socket, 10); // макс колво подключенных = 10


		/*sockaddr name;
		int len = sizeof(name);
		if (getsockname(Socket, &name, &len) == SOCKET_ERROR) {
			std::cout << "getsockname failed: " << WSAGetLastError() << "\n";
			closesocket(Socket);
			WSACleanup();
			return -1;
		}*/
	}


	char mem_pool[1024];
	memset(mem_pool, '0', sizeof(mem_pool));

	time_t ticks;

	struct client_info {
		int socket_read, socket_write;
		std::string name;
		std::string ip;
		u32 id;

		client_info(int socket_read, int socket_write, std::string name, std::string ip, u32 id) {
			this->socket_read = socket_read;
			this->socket_write = socket_write;
			this->name = name;
			this->ip = ip;
			this->id = id;
		}
	};

	std::vector<client_info> Clients;

	// подключаем всех игроков
	{
		std::cout << "Введите количество игроков: ";
		int players_cnt;
		std::cin >> players_cnt;

		Players.clear();

		while (players_cnt) {
			fd_set rfds;
			int max_fd = Socket;
			{
				FD_ZERO(&rfds);
				FD_SET(Socket, &rfds);

				for (auto& it : Clients) {

					FD_SET(it.socket_write, &rfds);

					if (it.socket_read != -1) {
						FD_SET(it.socket_read, &rfds);
					}

					max_fd = std::max<int>(max_fd, it.socket_write);
					max_fd = std::max<int>(max_fd, it.socket_read);
				}
			}

			// add new clients
			int ready = select(max_fd + 1, &rfds, NULL, NULL, NULL);

			if (ready > 0) {

				if (FD_ISSET(Socket, &rfds)) {

					// пришло новое подключение

					struct sockaddr_in client_addr;
					int len = sizeof(client_addr);

					// примим подключение
					int connfd = accept(Socket, (struct sockaddr*)&client_addr, &len);
						
					struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_addr;

					struct in_addr ipAddr = pV4Addr->sin_addr;

					char client_ip[INET_ADDRSTRLEN];

					inet_ntop(AF_INET, &ipAddr, client_ip, INET_ADDRSTRLEN);

					// нужно считать его сообщение об id

					int n = recv(connfd, mem_pool, sizeof(mem_pool) - 1, 0);

					if (n > 0) {
						mem_pool[n] = '\0';

						if (mem_pool[0] == -1) { // этого игрока еще не было

							send(connfd, new char(Clients.size()), 1, 0); // отправили ему его id

							Clients.push_back(client_info(-1, connfd, "", client_ip, Clients.size()));

							ticks = time(NULL);
							printf("%.24s Новый клиент подключился: %s::%d\n", ctime(&ticks), Clients.back().ip.c_str(), Clients.back().socket_write);
						}
						else { // мы уже получали его socket_write. это socket_read

							int i;
							for (i = 0; i < Clients.size() && Clients[i].id != mem_pool[0]; i++) {}

							Clients[i].socket_read = connfd;

							ticks = time(NULL);
							printf("%.24s Клиент %s::%d успешно подключил сокет чтения\n", ctime(&ticks), Clients.back().ip.c_str(), Clients.back().socket_write);
						}
					}

					/*if (send(connfd, new char(Clients.size()), 1, 0) > 0) {

						Clients.push_back(client_info(connfd, "", client_ip, Clients.size()));

						ticks = time(NULL);
						printf("%.24s Новый клиент подключился: %s::%d\n", ctime(&ticks), Clients.back().ip.c_str(), Clients.back().socket);
					}*/

					/*if (send(connfd, new char(Clients.size()), 1, 0) > 0) {

							Clients.push_back(client_info(-1, connfd, "", client_ip, Clients.size()));

							ticks = time(NULL);
							printf("%.24s Новый клиент подключился: %s::%d\n", ctime(&ticks), client_ip, connfd);

							int n = recv(connfd, mem_pool, sizeof(mem_pool) - 1, 0);
							mem_pool[n] = '\0';

							ticks = time(NULL);
							printf("%.24s Он зарегистрировался: %s\n", ctime(&ticks), mem_pool);

							for (u32 i = 0; i < sizeof(Players[Clients.size() - 1].name); i++) {
								Players[Clients.size() - 1].name[i] = mem_pool[i];
							}

							mem_pool[n] = '\0';
						}*/
				}

				for (int i = 0; i < Clients.size(); i++) {
					auto& client = Clients[i];

					if (client.socket_read != -1 && FD_ISSET(client.socket_read, &rfds)) {

						// регистрируем игроков

						int n = recv(client.socket_read, mem_pool, sizeof(mem_pool) - 1, 0);

						if (n > 0) {

							mem_pool[n] = '\0';

							if (client.name.empty()) {

								client.name = mem_pool;

								// клиент полностью готов к игре
								Players.push_back(dot());
								Players.back().id = Clients[i].id;
								for (u32 i = 0; i < sizeof(Players.back().name); i++) {
									Players.back().name[i] = mem_pool[i];
								}

								ticks = time(NULL);
								printf("%.24s Клиент %s::%d зарегистрировался: %s\n", ctime(&ticks), client.ip.c_str(), client.socket_write, client.name.c_str());
								players_cnt--;
							}
							else {
								ASSERT(true, "что мы тут забыли?");
							}
						}
						else { // disconnected
							printf("Клиент %s::%d disconnected\n", client.ip.c_str(), client.socket_write);

							if (!Clients[i].name.empty()) {
								players_cnt++;

								int j;
								for (j = 0; j < Players.size() && Players[j].id != Clients[i].id; j++) {}

								Players.erase(Players.begin() + j);
							}

							Clients.erase(Clients.begin() + i);
							i--;
						}

						mem_pool[0] = '\0';
					}

					if (FD_ISSET(client.socket_write, &rfds)) {
						printf("Клиент %s::%d disconnected\n", client.ip.c_str(), client.socket_write);

						if (!Clients[i].name.empty()) {
							players_cnt++;

							int j;
							for (j = 0; j < Players.size() && Players[j].id != Clients[i].id; j++) {}

							Players.erase(Players.begin() + j);
						}

						Clients.erase(Clients.begin() + i);
						i--;
					}
				}
			}
		}

		std::cout << "Все игроки успешно подключены\n\n";
	}

	std::cout << "Создаем мир...\n";
	build_world();
	std::cout << "Успешно\n\n";

	// send world_state
	{
		auto world_state = get_world_state();

		std::cout << "world_state size = " << world_state.size() << "\n";

		// send inform to clients
		for (auto& client : Clients) {
			send(client.socket_write, world_state.c_str(), world_state.size(), 0);
		}
	}

	point_t delta_time = 0;
	auto update_delta_time = [&delta_time]() -> void {
		static Timer timer;

		delta_time = timer.time();
		timer.reset();
	};

	update_delta_time();

	while (!Clients.empty()) {

		for (int i = 0; i < Clients.size(); i++) {
			auto& client = Clients[i];

			int n = recv(client.socket_read, mem_pool, sizeof(mem_pool) - 1, 0);

			if (n > 0) { // получили действия игрока. обработаем их
				simulate_player(*reinterpret_cast<Input*>(mem_pool), delta_time, client.id);
			}
			mem_pool[0] = '\0';
		}

		simulate_game_collision(Players);

		simulate_physics(delta_time);

		auto world_state = get_world_state();

		// send world state to clients
		for(int i = 0; i < Clients.size(); i++) {

			auto& client = Clients[i];

			if (send(client.socket_write, world_state.c_str(), world_state.size(), 0) == SOCKET_ERROR) {

				printf("Клиент %s::%d disconnected\n", client.ip.c_str(), client.socket_write);

				int j;
				for (j = 0; j < Players.size() && Players[j].id != client.id; j++) {}
				Players.erase(Players.begin() + j);

				Clients.erase(Clients.begin() + i);
				i--;
			}
		}

		if (world_state[0] == 't') {
			std::cout << "У нас есть победитель: ";
			world_state[0] = '\0';
			printf("%s", world_state.c_str() + 1);
			std::cout << "\n";

			closesocket(Socket);
			WSACleanup();

			return 0;
		}

		update_delta_time();
	}

	std::cout << "Все игроки вышли\n";

	closesocket(Socket);
	WSACleanup();

	return 0;
}

int main() {

	setlocale(LC_ALL, "Russian");

	std::cout << "Введите server/client/single: ";

	while (true) {
		std::string type;
		std::cin >> type;

		if (type == "server") {
			return simulate_server();
		}
		else if (type == "client" || type == "single") {
			return simulate_client_or_single_player(type == "client");
		}
		else {
			std::cout << "Неизвестная команда. Попробуйте еще\n";
		}
	}

	return 0;
}

