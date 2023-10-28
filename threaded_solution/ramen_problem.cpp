#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <chrono>

using namespace std;

queue<int> data_queue;
mutex queue_mutex; //shared resources
condition_variable cond_var;

void cook(int id) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(0, 100);

    for (int i = 0; i < 5; ++i) {
        int item = distr(gen);
        {
            lock_guard<mutex> lock(queue_mutex); // locks thread when in the scope
            data_queue.push(item);
            cout << "Cook with hat " << id << " produced ramen number " << item << std::endl;
        } // <- this scope is what allows the thread to 'give up the key'
        cond_var.notify_one(); // lets consumer thread know queue has values  
        this_thread::sleep_for(chrono::milliseconds(rand() % 700 + 100));
    }
}

void consumer(int id) {
    while (true) {
        unique_lock<mutex> lock(queue_mutex);
        cond_var.wait(lock, []{ return !data_queue.empty(); }); 

        int item = data_queue.front(); //critical section
        data_queue.pop();

        lock.unlock();  // Unlock mutex before potentially exiting the loop

        if(item != -1) {
            cout << "Hungry person with a " << id << " tattooed on their head consumed ramen number " << item << std::endl;
            this_thread::sleep_for(chrono::milliseconds(rand() % 500 + 200));
        } else {
            break;
        }
    }
}

int main() {
    thread cook_threads[3];
    thread consumer_threads[5];

    // cook threads
    for (int i = 0; i < 3; ++i) {
        cook_threads[i] = thread(cook, i);
    }

    // consumer threads
    for (int i = 0; i < 5; ++i) {

        consumer_threads[i] = thread(consumer, i);
    }

    // terminate cook threads
    for (int i = 0; i < 3; ++i) {
        cook_threads[i].join();
    }

    // terminate consumer threads
    for(int i = 0; i < 5; ++i) {
        {
        lock_guard<mutex> lock(queue_mutex);
        data_queue.push(-1);
        }
        cond_var.notify_all();  // Notify all waiting consumer threads
    }
    for (int i = 0; i < 5; ++i) {
        consumer_threads[i].join();
    }
 
    return 0;
}
