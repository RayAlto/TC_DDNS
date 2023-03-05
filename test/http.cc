#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

#include "tc/http.h"

int main(int argc, char const* argv[]) {
    tc::http::Response r =
        tc::http::Request("https://httpbin.org/anything", {{"key", "value"}})
            .method("POST")
            .headers({{"User-Agent", "TC_DNS_CLIENT"},
                      {"foo", "bar"},
                      /* {"Content-Type", "text/json"}, */
                      {"114514", "1919810"}})
            .cookies({{"brand", "Nabisco Oreo"}, {"comment", "delicious"}})
            .body("{\"key\": 114514}")
            .add_part_from_data("data1", "{\"key\": 114514}", {}, "text/json")
            .add_part_from_data(
                "file1", "{\"key\": 114514}", "filename1.json", "text/json")
            .perform();
    std::cout << r.body << std::endl;
    std::cout << tc::http::Request::url_encode("我草") << std::endl;
    return 0;
}
