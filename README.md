# A simple C++ channel simulating Go channel

## Thread-safe channel C++ supporting push and fetch data to/from channel

- [x] Support push and fetch to/from channel.
- [x] Support chaining pushing and fetching.
- [x] Use stream operators for pushing and fetching operations.
- [x] Support various types.
- [x] When pushing data into closed channel is blocking. Fetching data from
empty channel is also blocking.
- [] Support unbuffered channel.
- [] Channel is thread-safe(need to check).

## Requirements
C++17 or newer

## Examples
```cpp

#include <include/channel.h>

int main() {
  comm::channel<int,1> channel;

  int in = 1;
  int out = 0;

  // Send to channel
  channel << in;

  // Read from channel
  channel >> out;
}
```

```cpp
#include <include/channel.h>

int main() {
  struct Test {
      int a_;

      std::string b_;

      Test(int a, std::string b) : a_(a), b_(std::move(b)) {}
  };

  comm::Channel<std::unique_ptr<Test>> channel(2);

  auto in1 = std::make_unique<Test>(1, "a");
  auto in2 = std::make_unique<Test>(2, "b");

  // Chaining pushing.
  channel << std::move(in1) << std::move(in2);

  std::unique_ptr<Test> out1, out2;
  // Read from channel
  channel >> out1 >> out2;
}
```