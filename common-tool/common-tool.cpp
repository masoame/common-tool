#include <iostream>
#include "common.hpp"
#include<syncstream>
#include <chrono>
using namespace std;

class A
{
public:
	int i;

	A(int i) :i(i) { 
		//std::cout << "constructor" << std::endl; 
	}
	~A() {}
	A(const A& other) :i(other.i) { 
		//std::cout << "copy constructor" << std::endl; 
	}

	A(A&& other):i(other.i) { 
		other.i = 0; //std::cout << "move constructor" << std::endl;
	}

	operator int() { return i; }
};

void FreeFunc(int* a)
{
	delete a;
	return;
}

common::circular_queue<int, nullptr_t, false> test(10);
std::queue<int> test2;
std::mutex mtx;
std::condition_variable cv, cv2;

common::bounded_queue<std::unique_ptr<int>> test3{1024};


std::queue<std::vector<int>> pp;
void task1()
{
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 200000000; i++)
	{
		while (test.try_emplace(i) == false)
		{
			std::this_thread::yield();
		}
		//test3.emplace_back(new int(i));
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::osyncstream{ std::cout } << "thread id: " << std::this_thread::get_id() << " task1 end, time: " << duration.count() << "us" << std::endl;
}
void task2()
{
	auto start = std::chrono::high_resolution_clock::now();
	int ans = 0;
	for (int i = 0; i < 200000000; i++)
	{
		while (true)
		{
			auto a = test.try_pop();
			if (a)
			{
				ans += *a;
				break;
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::osyncstream{ std::cout } << ans << test2.size() << "thread id: " << std::this_thread::get_id() << " task2 end, time: "<< duration.count() << "us" << std::endl;
}
int main()
{
	common::ThreadPool pool(5,0);
	auto a = pool.enqueue(task1);
	auto b = pool.enqueue(task2);

	//delete test3;
	a.wait();
	b.wait();



	system("pause");
	return 0;
}
