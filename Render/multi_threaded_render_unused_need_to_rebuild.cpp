
#if MULTITHREAD_RENDER == 0

#elif MULTITHREAD_RENDER == 1

#include <atomic>
#include <thread>
#include <vector>

struct render_task {
    unsigned int x0;
    unsigned int y0;
    unsigned int x1;
    unsigned int y1;
    Color color;
};

std::vector<render_task> Render_tasks;

const int count_of_render_threads = 2;

std::thread Threads[count_of_render_threads];

void simulate_render_thread(int thread_id) {
    int task_index = 0;
    while (global_variables::running) {
        if (task_index)
            if (thr.spin) {
                // появилось новое задание

                Color color = thr.color;
                /*if(thread_id == 0){
                    color = GREEN;
                } else if(thread_id == 1){
                    color = BLUE;
                } else if(thread_id == 2){
                    color = RED;
                }*/

                draw_pixels_impl(thr.x0, thr.y0, thr.x1, thr.y1, color);

                thr.spin = false;
            }
    }
}

void init_render_threads() {
    for (int thread_id = 0; thread_id < count_of_render_threads; thread_id++) {
        Threads[thread_id] = std::thread(
            simulate_render_thread, std::ref(Threads_vals[thread_id]), thread_id
        );
    }
}

void wait_all_render_threads() {
    for (const auto &Threads_val : Threads_vals) {
        Threads_val.wait();
    }
}

void join_all_render_threads() {
    for (auto &Thread : Threads) {
        Thread.join();
    }
}

#else
static_assert(false);
#endif