#include "semaphore.h"

#include <cassert>
#include <mutex>
#include <thread>

const int test_size = 100000;
const int slots_size = 5;

semaphore sem_filled(0);
semaphore sem_empty(slots_size);

int data[slots_size];
std::mutex lock_data;

void producer()
{
    int write_idx = 0;
    for(int i = 0; i != test_size; ++i) {
	sem_empty.wait();
	lock_data.lock();

	data[write_idx] = i;
	write_idx = (write_idx + 1) % slots_size;
	
	lock_data.unlock();
	sem_filled.notify();
    }
}

void consumer()
{
    int read_idx = 0;
    int expect = 0;
    while(expect != test_size) {
	sem_filled.wait();
	lock_data.lock();

	assert(data[read_idx] == expect);
	++expect;
	read_idx = (read_idx + 1) % slots_size;
	
	lock_data.unlock();
	sem_empty.notify();
    }
}

int main()
{
    std::thread pro(producer);
    std::thread con(consumer);
    pro.join();
    con.join();
}
