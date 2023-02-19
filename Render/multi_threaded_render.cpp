#include <thread>

struct render_thread_values {
    std::atomic<bool> spin = false;

    Dot pos;
    efloat size{};
    sprite_t sprite = SP_COUNT;
    int x0{}, y0{}, x1{}, y1{};

    void give_task(
        Dot npos,
        efloat nsize,
        sprite_t nsprite,
        int nx0,
        int ny0,
        int nx1,
        int ny1
    ) {
        wait();
        pos = npos;
        size = nsize;
        sprite = nsprite;
        x0 = nx0;
        y0 = ny0;
        x1 = nx1;
        y1 = ny1;
        spin = true;
    }

    void wait() const {
        while (spin) {
        }
    }

    bool is_busy() const {
        return spin;
    }
};

void simulate_render_thread(render_thread_values &thr) {
    while (running) {
        if (thr.spin) {
            // появилось новое задание

            // do task
            {
                auto &pixels = Sprites[thr.sprite];

                efloat rect_sz = thr.size * scale_factor;

                efloat y = thr.pos.y;

                for (int i = thr.y0; i < thr.y1; i++) {
                    efloat x = thr.pos.x;

                    s64 y0 = static_cast<s64>(y);
                    s64 y1 = static_cast<s64>(y + rect_sz);

                    for (int j = thr.x0; j < thr.x1;) {
                        s64 x0 = static_cast<s64>(x);
                        int k = j;
                        while (k < thr.x1 && pixels[i][k] == pixels[i][j]) {
                            k++;
                        }

                        x += (k - j) * rect_sz;
                        s64 x1 = static_cast<s64>(x);

                        if (is_draw(pixels[i][j])) {
                            draw_rect_in_pixels(x0, y0, x1, y1, pixels[i][j]);
                        } else if (debug_mode) {
                            draw_rect_in_pixels(
                                x0, y0, x1, y1, Color(0xffffff, 60)
                            );
                        }

                        j = k;
                    }

                    y -= rect_sz;
                }
            }

            thr.spin = false;
        }
    }
}

const int count_of_render_threads = 0;

render_thread_values *Threads_vals =
    new render_thread_values[count_of_render_threads];

std::thread *Threads = new std::thread[count_of_render_threads];

void init_render_threads() {
    for (int i = 0; i < count_of_render_threads; i++) {
        Threads[i] =
            std::thread(simulate_render_thread, std::ref(Threads_vals[i]));
    }
}

void wait_all_render_threads() {
    for (int i = 0; i < count_of_render_threads; i++) {
        Threads_vals[i].wait();
    }
}

void join_all_render_threads() {
    for (int i = 0; i < count_of_render_threads; i++) {
        Threads[i].join();
    }
}