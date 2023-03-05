#include <cstdint>
#include <ios>
#include <iostream>
#include <vector>

#include "tc/crypt.h"
#include "tc/util.h"

int main(int argc, const char *argv[]) {
    // echo -ne '\x19\x19\x81\x00' | openssl dgst -sha256 -mac -hmac -macopt hexkey:114514
    std::cout
        << std::boolalpha
        << (tc::util::data2hexstr(tc::crypt::hmac_sha256(
                std::vector<std::uint8_t> {0x11, 0x45, 0x14},
                std::vector<std::uint8_t> {0x19, 0x19, 0x81, 0x00}))
            == "cba51e0b3e4550e9d6a25ad6f610fcbbe81d3217b388886fdd98542f54271ede")
        << std::endl;
    return 0;
}
