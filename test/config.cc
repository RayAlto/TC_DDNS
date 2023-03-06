#include <iostream>
#include <string>

#include "tc/config.h"

int main(int argc, const char* argv[]) {
    tc::Config c = tc::Config::from_env();
    std::cout << "secret_id: " << c.secret_id
              << ", secret_key: " << c.secret_key << std::endl;
}
