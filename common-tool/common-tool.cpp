#include <iostream>
#include "common.hpp"
#include<syncstream>
#include <chrono>
using namespace std;

class A
{
public:
	int i;
	A(int i) :i(i) { std::cout << "constructor" << std::endl; }
	~A() {}
	A(const A& other) :i(other.i) { std::cout << "copy constructor" << std::endl; }

	A(A&& other):i(other.i) { other.i = 0; std::cout << "move constructor" << std::endl; }

	operator int() { return i; }
};

common::circular_queue<int, nullptr_t, false> test(8);
std::queue<int> test2;
std::mutex mtx;
std::condition_variable cv, cv2;

common::bounded_queue<int>* test3=new common::bounded_queue<int>(1000000);

void FreeFunc(int* a)
{
	delete a;
	return;
}
std::queue<std::vector<int>> pp;
void task1()
{
	auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < 5000000; i++)
	{
		test3->emplace_back(i);

	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::osyncstream{ std::cout } << "thread id: " << std::this_thread::get_id() << " task1 end, time: " << duration.count() << "us" << std::endl;
}
void task2()
{
	auto start = std::chrono::high_resolution_clock::now();
	int ans = 0;
	for (int i = 0; i < 5000000; i++)
	{
		auto a = test3->pop();
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::osyncstream{ std::cout } << ans << test2.size() << "thread id: " << std::this_thread::get_id() << " task2 end, time: "<< duration.count() << "us" << std::endl;
}
int main()
{
	common::ThreadPool pool(3,0);
	auto a = pool.enqueue(task1);
	auto b = pool.enqueue(task2);
	auto c = pool.enqueue(task2);
	auto d = pool.enqueue(task1);
	delete test3;
	a.wait();
	b.wait();
	c.wait();
	d.wait();


	system("pause");
	return 0;
}
