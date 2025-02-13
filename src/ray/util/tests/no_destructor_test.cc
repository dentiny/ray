// Copyright 2025 The Ray Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ray/util/no_destructor.h"

#include <gtest/gtest.h>

namespace ray {

namespace {
class MyClass {
 public:
  MyClass() = default;
  MyClass(int) {}
  MyClass(const MyClass&) = default;
  MyClass(MyClass&&) = default;

  int Get() const { return 1; }
  ~MyClass() { assert(false); }
};
}  // namespace

TEST(NoDestructorTest, BasicTest) {
  // Default constructor.
  {
    NoDestructor<MyClass> obj{};
    EXPECT_EQ(obj->Get(), 1);
  }
  // Copy constructor.
  {
    MyClass obj{};
    NoDestructor<MyClass> no_dest{obj};
    EXPECT_EQ(no_dest->Get(), 1);
  }
  // Move constructor.
  {
    MyClass obj{};
    NoDestructor<MyClass> no_dest{std::move(obj)};
    EXPECT_EQ(no_dest->Get(), 1);
  }
  // Perfect forwarding with emplace.
  {
    NoDestructor<MyClass> no_dest{1};
    EXPECT_EQ(no_dest->Get(), 1);
  }
}

}  // namespace ray
