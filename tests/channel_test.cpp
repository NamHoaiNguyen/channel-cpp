#include "include/channel.h"

#include <gtest/gtest.h>

#include <memory>
#include <string>

TEST(Channel, Traits) {
  using type = int;
  using channel = comm::Channel<type>;
  // https://stackoverflow.com/questions/65300353/google-test-gtest-expect-true-macro-wont-compile-with-stdis-same-te
  EXPECT_TRUE((std::is_same_v<channel::type, type>));

  // Reference
  using ref_type = int&;
  EXPECT_FALSE((std::is_same_v<channel::type, ref_type>));

  // Complex type
  struct Test {
    int a_;

    std::string b_;
  };
  using type_1 = Test;
  using channel_type_1 = comm::Channel<type_1>::type;
  EXPECT_TRUE((std::is_same_v<channel_type_1, type_1>));
}

TEST(Channel, PushAndFetch) {
  comm::Channel<int> channel(3);

  // Push data into channel
  int a = 1;
  channel << a;

  // Push data into channel until reaching channel capacity
  int b = 2, c = 3, d = 4;
  // Test push chaning
  channel << b << c;

  EXPECT_TRUE((!channel.IsClosed()));

  // TODO(namnh) : Test that in case size of channel = its capacity,
  // continue pushing into channel will be blocking.
  // channel << d;

  // Get data from channel
  int a_{0}, b_{0}, c_{0}, d_{0};
  // Chaining get data from channel
  channel >> a_ >> b_ >> c_;

  EXPECT_EQ(a_, a);
  EXPECT_EQ(b_, b);
  EXPECT_EQ(c_, c);

  channel.Close();
}

TEST(Channel, PushAndFetchMove) {
  struct Test {
    int a_;

    std::string b_;

    Test(int a, std::string b) : a_(a), b_(std::move(b)) {}
  };
  comm::Channel<std::unique_ptr<Test>> channel(3);

  auto test1 = std::make_unique<Test>(1, "a");
  auto test2 = std::make_unique<Test>(2, "b");
  auto test3 = std::make_unique<Test>(3, "c");

  // Push data into channel.
  channel << std::move(test1);
  // Chaining pushing.
  channel << std::move(test2) << std::move(test3);

  EXPECT_TRUE((!test1));
  EXPECT_TRUE((!test2));
  EXPECT_TRUE((!test3));

  std::unique_ptr<Test> test1_, test2_, test3_;

  // Get data form channel
  channel >> std::move(test1_) >> std::move(test2_) >> std::move(test3_);

  EXPECT_TRUE((test1_));
  EXPECT_TRUE((test2_));
  EXPECT_TRUE((test3_));

  EXPECT_EQ(test1_->a_, 1);
  EXPECT_EQ(test1_->b_, "a");
  EXPECT_EQ(test2_->a_, 2);
  EXPECT_EQ(test2_->b_, "b");
  EXPECT_EQ(test3_->a_, 3);
  EXPECT_EQ(test3_->b_, "c");

  channel.Close();
}

TEST(Channel, Close) {
  comm::Channel<int> channel(3);
  int a = 1, b = 2, c = 3;
  int a_{0}, b_{0}, c_{0};

  channel << a << b;
  channel.Close();

  EXPECT_TRUE((channel.IsClosed()));

  // Can't push data in channel after close
  EXPECT_THROW(channel << c,
               std::runtime_error);

  // Fetch data from channel until channel is empty
  channel >> a_ >> b_;
  EXPECT_EQ(a_, 1);
  EXPECT_EQ(b_, 2);

  // Fetching data from closed and empty channel cause error
  EXPECT_THROW(channel >> c_, std::runtime_error);
}

TEST(Channel, Multithread) {
  // TODO(namnh): Test with multithread
}