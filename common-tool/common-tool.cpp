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

common::circular_queue<int, nullptr_t, true> test(10);
std::queue<int> test2;
std::mutex mtx;
std::condition_variable cv, cv2;

common::bounded_queue<std::unique_ptr<int>> test3{1024};


std::queue<std::vector<int>> pp;
void task1()
{
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 100000000; i++)
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
int task2()
{
	auto start = std::chrono::high_resolution_clock::now();
	int ans = 0;
	for (int i = 0; i < 100000000; i++)
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

	std::osyncstream{ std::cout }<< test2.size() << "thread id: " << std::this_thread::get_id() << " task2 end, time: "<< duration.count() << "us" << std::endl;
	return ans;
}


volatile int sum(int a, int b)
{
	throw std::runtime_error("error");
	return a + b;
}


int main()
{
	//{
	//	common::ThreadPool pool(4, 0);

	//	auto f1 = pool.enqueue(task1);
	//	auto f2 = pool.enqueue(task2);
	//	auto f3 = pool.enqueue(task1);
	//	auto f4 = pool.enqueue(task2);

	//	std::this_thread::sleep_for(std::chrono::seconds(3));

	//	f3.get();
	//	f1.get();
	//	std::osyncstream{ std::cout } << "task2 ans: " << f2.get() << std::endl;
	//	std::osyncstream{ std::cout } << "task2 ans: " << f4.get() << std::endl;
	//}
	std::packaged_task<int(int, int)> task(sum);
	auto f = task.get_future();
	task(1, 2);
	std::osyncstream{ std::cout } << "task result: " << f.get() << std::endl;

	system("pause");
	return 0;
}
