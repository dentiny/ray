#include "absl/container/flat_hash_map.h"
#include <string>
#include <string_view>
#include "absl/types/span.h"
#include <iostream>
#include <vector>

absl::flat_hash_map<std::string, std::string> m;

template <typename Visitor>
void MyFunc(Visitor&& visitor) {
    std::forward<Visitor>(visitor)(m.begin()->first, m.begin()->second);
}

int main() {
    m.emplace("hello", "world");

    auto f = [](std::string_view key, std::string_view val) {
        std::cout << "key = " << key << std::endl;
        std::cout << "val = " << val << std::endl;
    };
    MyFunc(std::move(f));

    return 0;
}
