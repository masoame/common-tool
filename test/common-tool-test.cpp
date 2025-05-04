#include<common-tool.hpp>
#include<iostream>

int main()
{
    common::circular_queue<char> q1(5);
    common::bounded_queue<char> q2(3);
    q1.try_push('a');
    q1.try_emplace('b');
    q1.empty();
    q1.back();



    std::cout << "Hello, world!" << std::endl;
    return 0;
}