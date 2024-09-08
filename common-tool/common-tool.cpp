#include <iostream>
#include "common.hpp"
#include<syncstream>
#include <chrono>
using namespace std;

common::circular_queue<int, nullptr_t, false> test(8);
void FreeFunc(int* a)
{
	delete a;
	return;
}

void task1()
{
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 10000000; i++)
	{
		while (test.try_emplace(i)==false)
		{
			std::this_thread::yield();
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::osyncstream{ std::cout } << "thread id: " << std::this_thread::get_id() << " task1 end, time: " << duration.count() << "us" << std::endl;
}
void task2()
{
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 10000000; i++)
	{
		while (true)
		{
			auto _ptr = test.try_pop();
			if (_ptr != nullptr) {
				//std::osyncstream{ std::cout } << "thread id: "<< std::this_thread::get_id() << " pop value: " << *_ptr << std::endl;
				break;
			}
			std::this_thread::yield();
		} ;

	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::osyncstream{ std::cout } << "thread id: " << std::this_thread::get_id() << " task2 end, time: "<< duration.count() << "us" << std::endl;
}

int main()
{
	common::ThreadPool pool(2,4);
	auto a = pool.enqueue(task1);
	auto b = pool.enqueue(task2);
	a.get();
	b.get();
	return 0;
}
