#include <stdio.h>
#include <string.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace std;

void task_1()
{
    cout << "task_1 start" << endl;
    cout << "thead_id(task_1): " << boost::this_thread::get_id() << endl;
    for (int i = 0; i < 10; i++)
    {
        cout << "1111111111111111111111111" << endl;
        sleep(1);
    }
}

int task_2()
{
    cout << "task_2 start" << endl;
    cout << "thead_id(task_2): " << boost::this_thread::get_id() << endl;
    for (int i = 0; i < 30; i++)
    {
        cout << "222222222222222222222222" << endl;
        sleep(1);
    }
    return 0;
}

void DoGetVersionNoForUpdate(int a)
{
    cout << "task_3 start" << endl;
    cout << "thead_id(task_3): " << boost::this_thread::get_id() << endl;
    for (int i = 0; i < 5; i++)
    {
        cout << a * a << endl;
        sleep(1);
    }
}

int main(int argc, char *argv[])
{
    //设置允许开启的线程数
    boost::asio::thread_pool tp(2);

    //加入线程调度，可以通过指针传参
    boost::asio::post(tp, &task_1);
    boost::asio::post(tp, &task_2);
    int i = 10;
    boost::asio::post(tp, boost::bind(DoGetVersionNoForUpdate, i));

    tp.join();
    return (0);
}
