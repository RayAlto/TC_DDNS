#include <iostream>

#include "tc/util.h"

int main(int argc, const char *argv[]) {
    unsigned char example[] {
        0x11, 0x45, 0x14, 0x19, 0x19, 0x81, 0x00, 0xab, 0xcd, 0xef};
    std::cout << tc::util::data2hexstr(example, sizeof(example)) << std::endl;
    std::cout << tc::util::data2hexstr(tc::util::hexstr2data("fff"))
              << std::endl;
    return 0;
}
