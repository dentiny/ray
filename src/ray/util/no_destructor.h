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

#pragma once

#include <new>
#include <utility>

namespace ray {

// Defines a class which wraps a static object which never destructs upon program
// termination. It's useful to avoid non-deterministic destruction order for global
// objects.
//
// Another commonly-used solution is `new` and never destructs, but suffer pointer
// indirection overhead compared with placement new.
template <class T>
class NoDestructor {
 public:
  // NOLINTNEXTLINE
  NoDestructor() { new (&storage_) T{}; }

  // NOLINTNEXTLINE
  explicit NoDestructor(const T &data) { new (&storage_) T{data}; }

  // NOLINTNEXTLINE
  explicit NoDestructor(T &&data) { new (&storage_) T{std::move(data)}; }

  template <typename... Args>
  explicit NoDestructor(Args &&...args) {
    new (&storage_) T{std::forward<Args>(args)...};
  }

  // No destruct objects constructed via placement new.
  ~NoDestructor() = default;

  // NOLINTNEXTLINE
  T *get_ptr() & { return std::launder<T>(reinterpret_cast<T *>(&storage_)); }

  T &operator*() & { return *get_ptr(); }
  T *operator->() & { return get_ptr(); }

 private:
  alignas(T) unsigned char storage_[sizeof(T)];  // NOLINT
};

}  // namespace ray
