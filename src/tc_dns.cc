#include <iostream>

#include "tc.h"

int main(int argc, const char *argv[]) {
    std::cout << tc::util::utc_format("%Y-%m-%d") << std::endl;
    return 0;
}
