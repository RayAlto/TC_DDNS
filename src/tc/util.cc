#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>

#include "tc/util.h"

namespace tc::util {

constexpr std::uint8_t hex2bin(const char& c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

constexpr std::uint8_t hex2byte(const char& l, const char& r) {
    std::uint8_t bin = 0;
    bin |= (hex2bin(l) << 4);
    bin |= hex2bin(r);
    return bin;
}

std::string utc_format(const std::string& format, const std::size_t& buf_len) {
    std::time_t now = std::time(nullptr);
    std::tm* now_tm = std::gmtime(&now);
    char utc_buf[buf_len];
    std::size_t utc_str_len =
        std::strftime(utc_buf, buf_len, format.c_str(), now_tm);
    return {utc_buf, utc_str_len};
}

std::string data2hexstr(const unsigned char* data,
                        const std::size_t& data_length,
                        const bool& upper_case) {
    std::string str;
    str.reserve(data_length * 2 + 1);
    static const char hex_map_lower[] {"0123456789abcdef"};
    static const char hex_map_upper[] {"0123456789ABCDEF"};
    const char* hex_map = upper_case ? hex_map_upper : hex_map_lower;
    std::size_t i = 0;
    const unsigned char& first_byte = data[0];
    // 0xfff -> "fff" (not "0fff")
    if ((first_byte & 0xf0) == 0) {
        str.push_back(hex_map[first_byte & 0x0f]);
        i++;
    }
    while (i < data_length) {
        const unsigned char& b = data[i];
        str.push_back(hex_map[b >> 4]);
        str.push_back(hex_map[b & 0x0f]);
        i++;
    }
    return str;
}

std::string data2hexstr(const std::vector<std::uint8_t>& data,
                        const bool& upper_case) {
    return data2hexstr(reinterpret_cast<const unsigned char*>(data.data()),
                       data.size(),
                       upper_case);
}

std::string data2hexstr(const std::string& data, const bool& upper_case) {
    return data2hexstr(reinterpret_cast<const unsigned char*>(data.c_str()),
                       data.length(),
                       upper_case);
}

std::vector<std::uint8_t> hexstr2data(const char* hexstr,
                                      const std::size_t& len) {
    std::vector<std::uint8_t> buf;
    buf.reserve(len / 2);
    std::size_t i = 0;
    // "fff" -> 0xfff (not 0xfff0)
    if (len % 2 == 1) {
        buf.emplace_back(hex2bin(hexstr[0]));
        i++;
    }
    while (i < len) {
        const char& byte_l = hexstr[i];
        const char& byte_r = hexstr[i + 1];
        buf.emplace_back(hex2byte(byte_l, byte_r));
        i += 2;
    }
    return buf;
}

std::vector<std::uint8_t> hexstr2data(const std::string& hexstr) {
    return hexstr2data(hexstr.c_str(), hexstr.length());
}

} // namespace tc::util
