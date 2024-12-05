#include <queue>
#include <functional>
#include "SimpleCoroutine.hpp"

CoRet simpleCoFunc(int input_value, int i_value_2)
{
    // CoRet::promise_type promise; // 编译器先创建一个 promise 对象

    // CoRet ret = promise.get_return_object();
    // ret 就是 simpleCoFunc 的返回值，ret 类型的一般做法是内含一个 std::coroutine_handle 对象
    // 这个对象可以控制 simpleCoFunc 的运行，如恢复协程执行(ret.handle.resume())

    // co_await promise.initial_suspend(); // 表示协程是否需要在最开始的时候暂停一遍

    std::println("coroutine entered");

    Input input{input_value};
    int   g = co_await input;
    // co_await后面需要是一个 Awaitable 对象
    // 每当遇见co_await时，都会执行 input.await_ready()，询问是否已经等到input的结果，如果是false，表示没等到，则暂停协程，反之继续执行协程
    // input.await_suspend()，表示协程被暂停要跳转时需要进行的操作。见await_suspend内的注释。
    // 而 int g 的值则从 await_resume() 获取，await_resume() 在 simpleCoFunc() 恢复执行时调用

    co_yield 2;
    // 等价于 co_await promise.yield_value(2);

    std::println("g = {}", g);

    std::println("coroutine ended");

    // co_await promise.final_suspend(); // 表示协程是否需要在结束前暂停一遍
    co_return; // 代表协程结束
}

void simpleCoroutineTest()
{
    CoRet ret = simpleCoFunc(8, 7);
    std::println("0 ret = {}", ret.get());
    ret.handle.resume();
    std::println("1 ret = {}", ret.get());
    ret.handle.resume();
    std::println("2 ret = {}", ret.get());
    ret.handle.resume();
    std::println("3 ret = {}", ret.get());
}

int main()
{
    simpleCoroutineTest();
    return 0;
}
