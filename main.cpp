#include<iostream>
#include <experimental/coroutine>
// using namespace std;

class resumable {
public:
  struct promise_type;
  using coro_handle = std::experimental::coroutine_handle<promise_type>;
  resumable(coro_handle handle) : handle_(handle) { assert(handle); }
  resumable(resumable&) = delete;
  resumable(resumable&&) = delete;
  bool resume() {
    if (not handle_.done())
      handle_.resume();
    return not handle_.done();
  }
  ~resumable() { handle_.destroy(); }
private:
  coro_handle handle_;
};
struct resumable::promise_type {
  using coro_handle = std::experimental::coroutine_handle<promise_type>;
  auto get_return_object() {
    return coro_handle::from_promise(*this);
  }
  void* operator new(std::size_t) noexcept {
    return nullptr;   
  }
  static resumable get_return_object_on_allocation_failure(){
    throw std::bad_alloc();   
  }
  auto initial_suspend() { return std::experimental::suspend_always(); }
  auto final_suspend() { return std::experimental::suspend_always(); }
  void return_void() {}
  void unhandled_exception() {
    std::terminate();
  }
};

resumable foo(){
  std::cout << "Hello" << std::endl;
  co_await std::experimental::suspend_always();
  std::cout << "Coroutine" << std::endl;
}
int main(){
  resumable res = foo();
  res.resume();
//   while (res.resume());
}
