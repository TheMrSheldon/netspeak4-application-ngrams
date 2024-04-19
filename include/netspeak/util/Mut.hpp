#ifndef NETSPEAK_UTIL_MUT_HPP
#define NETSPEAK_UTIL_MUT_HPP

#include <mutex>

namespace netspeak::util {

template <class T>
class Mut {
  T value_;
  std::mutex mutex_;

public:
  Mut() = default;
  explicit Mut(const T& value) : value_(value), mutex_() {}
  explicit Mut(T&& value) : value_(std::move(value)), mutex_() {}

  class Lock {
    Mut<T>& mut_;

  public:
    Lock() = delete;
    Lock(const Lock&) = delete;
    explicit Lock(Mut<T>& mut) : mut_(mut) {}
    Lock(Lock&& lock) = default;
    ~Lock() {
      mut_.mutex_.unlock();
    }

    T& value() {
      return mut_.value_;
    }
  };

  Lock lock() {
    mutex_.lock();
    return Lock(*this);
  }
};

} // namespace netspeak::util

#endif
