# include <iostream>
# include <thread>
# include <functional>
# include <list>
# include <condition_variable>
# include <mutex>
# include <thread>


using namespace std;


class Workers {

    int thread_pool_size;
    mutex tasks_mutex;
    mutex worker_stopped_mutex;
    mutex waiting_mutex;
    bool has_stopped = false;
    bool should_wait = true;
    condition_variable cv;
    list<function<void()>> tasks;
    list<thread> worker_threads;

    public:
        Workers (int thread_pool_size) {
            this->thread_pool_size = thread_pool_size;
        }

        void post(function<void()> task) {
            {
                lock_guard<mutex> lock(tasks_mutex);
                tasks.emplace_back(task);
            }
            {
                lock_guard<mutex> lock(waiting_mutex);
                should_wait = false;
            }
            cv.notify_all();
        };

        void post_timeout(function<void()> task, int timeout_in_milliseconds) {
            this_thread::sleep_for(std::chrono::milliseconds(timeout_in_milliseconds));
            post(task);
        };

        void start() {
            worker_threads.clear();

            for (int i = 0; i < thread_pool_size; i++) {
                worker_threads.emplace_back([this, i] {
                    bool is_done = false;
                    bool has_tasks_left = false;
                    cout << "Running task in thread " << i + 1 << endl;

                    do_work(is_done, has_tasks_left);
                });
            }        
        };

        void do_work(bool is_done, bool has_tasks_left) {
            while (!is_done) {
                function<void()> task;
                wait_if_necessary(has_tasks_left);

                lock_guard<mutex> lock(tasks_mutex);
                if (!tasks.empty()) {
                    task = *tasks.begin();
                    tasks.pop_front();
                    has_tasks_left = true;
                } else {
                    {
                        unique_lock<mutex> lock(worker_stopped_mutex);
                        if (has_stopped) {
                            is_done = true;
                        } else {
                            unique_lock<mutex> lock(waiting_mutex);
                            should_wait = true;
                            has_tasks_left = false;
                        }
                    }
                } 

                if (task) 
                    task();

                cv.notify_one();
            }
        };

        void wait_if_necessary(bool has_tasks_left) {
            if (!has_tasks_left) {
                unique_lock<mutex> lock(waiting_mutex);
                while (should_wait) 
                    cv.wait(lock);
            }
        }

        void stop() {
            {
                unique_lock<mutex> lock(worker_stopped_mutex);
                has_stopped = true;
            }
            {
                lock_guard<mutex> lock(waiting_mutex);
                should_wait = false;
            }
            cv.notify_all();
        };

        void join() {           
            for (auto &thread : worker_threads)
                thread.join();
        };
};


int main() {
    Workers worker_threads(4);
    Workers event_loop(1);

    worker_threads.start();
    event_loop.start();

    worker_threads.post([] {
        cout << "Task A" << endl;
    });
    worker_threads.post([] {
        cout << "Task B" << endl;
    });

    worker_threads.post_timeout([] {
        cout << "Timeout task A" << endl;
    }, 5000);
    
    event_loop.post([] {
        cout << "Task C" << endl;
    });
    event_loop.post([] {
        cout << "Task D" << endl;
    });

    worker_threads.stop();
    event_loop.stop();

    worker_threads.join();
    event_loop.join();    
}