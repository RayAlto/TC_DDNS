#ifndef TC_DNS_TC_UTIL_H_
#define TC_DNS_TC_UTIL_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace tc::util {

/**
 * Convert the date and time information now to string.
 *   Not thread safe (use gmtime)
 */
std::string utc_format(const std::string& format,
                       const std::size_t& buf_len = 32);

std::string data2hexstr(const unsigned char* data,
                        const std::size_t& data_length,
                        const bool& upper_case = false);
std::string data2hexstr(const std::vector<std::uint8_t>& data,
                        const bool& upper_case = false);
std::string data2hexstr(const std::string& data,
                        const bool& upper_case = false);

std::vector<std::uint8_t> hexstr2data(const char* hexstr,
                                      const std::size_t& len);
std::vector<std::uint8_t> hexstr2data(const std::string& hexstr);

} // namespace tc::util

#endif // TC_DNS_TC_UTIL_H_
