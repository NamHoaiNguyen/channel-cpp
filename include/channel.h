#ifndef CHANNEL_H
#define CHANNEL_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

namespace comm {

template <typename T>
class Channel {
 public:
  using type = typename std::decay_t<T>;

  // Channel() = default;
  // Only suppport buffered channel first.
  Channel(std::size_t capacity) : capacity_(capacity), is_closed_(false) {}

  // Copy constructor
  Channel(const Channel&) = delete;
  Channel& operator=(Channel&) = delete;

  // Move constructor
  Channel(Channel&&) = delete;
  Channel& operator=(Channel&&) = delete;

  ~Channel() = default;

  // Return type as Channel for chanining pushing/fetching
  // Note(because it is lvalue, In T&&, T will be deduced as lvalue references)
  template <typename U>
  friend Channel<typename std::decay_t<U>>& operator<<(
      Channel<typename std::decay_t<U>>& channel, U&& data);

  template <typename U>
  friend Channel<typename std::decay_t<U>>& operator>>(
      Channel<typename std::decay_t<U>>& channel, U&& data);

  // Close channel to prevent push and fetch data
  void Close();

  bool IsClosed();

  bool IsEmpty();

 private:
  std::condition_variable cv_;

  std::mutex mutex_;

  std::queue<type> queue_;

  // std::atomic<std::size_t> size_;

  const std::size_t capacity_{0};

  std::atomic<bool> is_closed_;
};

template <typename T>
Channel<typename std::decay_t<T>>& operator<<(
    Channel<typename std::decay_t<T>>& channel, T&& data) {
  if (channel.IsClosed()) {
    throw std::runtime_error("Push data in a closed channel");
  }

  {
    std::unique_lock<std::mutex> lock(channel.mutex_);
    if (channel.capacity_ > 0 && channel.queue_.size() >= channel.capacity_) {
      channel.cv_.wait(
          lock, [&]() { return channel.queue_.size() < channel.capacity_; });
    }

    channel.queue_.push(std::forward<T>(data));
    // channel.size_++;
  }

  channel.cv_.notify_one();

  return channel;
}

template <typename T>
Channel<typename std::decay_t<T>>& operator>>(
    Channel<typename std::decay_t<T>>& channel, T&& data) {
  {
    std::unique_lock<std::mutex> lock(channel.mutex_);

    if (channel.IsClosed() && channel.queue_.empty()) {
      throw std::runtime_error("Get data in a closed empty channel");
    }

    channel.cv_.wait(
        lock, [&]() { return !channel.queue_.empty() || channel.IsClosed(); });

    if (!channel.queue_.empty()) {
      data = std::move(channel.queue_.front());
      channel.queue_.pop();
      // channel.size_--;
    }
  }

  channel.cv_.notify_one();

  return channel;
}

template <typename T>
bool Channel<T>::IsEmpty() {
  return capacity_ == 0;
}

template <typename T>
bool Channel<T>::IsClosed() {
  return is_closed_.load();
}

template <typename T>
void Channel<T>::Close() {
  {
    is_closed_.store(true);
  }

  cv_.notify_all();
}

}  // namespace comm

#endif  // CHANNEL_H