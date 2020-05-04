#include<iostream>
#include <experimental/coroutine>
// using namespace std;

class resumable {
public:
  const char* return_val();
  const char* recent_val();
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
  const char* string_ = nullptr;
  void return_value(const char* string) {string_ = string;}

  using coro_handle = std::experimental::coroutine_handle<promise_type>;
  auto get_return_object() {
    return coro_handle::from_promise(*this);
  }
//   void* operator new(std::size_t) noexcept {
//     return nullptr;   
//   }
//   static resumable get_return_object_on_allocation_failure(){
//     throw std::bad_alloc();   
//   }
  auto initial_suspend() { return std::experimental::suspend_always(); }
  auto final_suspend() { return std::experimental::suspend_always(); }
//   void return_void() {}
  void unhandled_exception() {
    std::terminate();
  }
  auto yield_value(const char* string){
    string_=string;
    return std::experimental::suspend_always();
  }
};
const char* resumable::return_val(){
  return handle_.promise().string_;
}
const char* resumable::recent_val(){return handle_.promise().string_;}
// 1
// resumable foo(){
//   std::cout << "Hello" << std::endl;
//   co_await std::experimental::suspend_always();
//   std::cout << "Coroutine" << std::endl;
// }
// 2
// resumable foo(){
//   std::cout << "Hello" << std::endl;
//   co_await std::experimental::suspend_always();
//   co_return "Coroutine";
// }
// 3
resumable foo(){
  while(true){
    co_yield "Hello";
    co_yeild "Coroutine";
  }
}
// 1
// int main(){
//   resumable res = foo();
//   res.resume();
// //   while (res.resume());
// }
// 2
// int main(){
//   resumable res = foo();
//   while(res.resume());
//   std::cout << res.return_val() << std::endl;
// }
// 3
int main() {
  resumable res = foo();
  int i=10;  
  while (i--){
    res.resume();
    std::cout << res.recent_val() << std::endl;  
  }
}