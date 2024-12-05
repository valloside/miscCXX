#include <coroutine>
#include <print>

struct CoRet
{
    struct promise_type
    {
        std::suspend_always initial_suspend() noexcept { return {}; }

        std::suspend_always final_suspend() noexcept { return {}; }

        CoRet get_return_object()
        {
            return CoRet{.handle = std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always yield_value(int ret)
        {
            this->value = ret;
            return {};
        }

        void return_void() {}

        void unhandled_exception() {}

        int value = -1;
    };

    int get() { return this->handle.promise().value; }

    std::coroutine_handle<promise_type> handle;
};

struct Input
{
    bool await_ready() { return false; }

    void await_suspend([[maybe_unused]] std::coroutine_handle<CoRet::promise_type> handle)
    {
        // await_suspend的返回值可以是void，也可以是其他协程的handle，后置表示本协程暂停后跳转到其他协程。
        // void 表示跳转到调用方，如 main()
    }

    int await_resume() { return this->in_val; }

    int in_val;
};
