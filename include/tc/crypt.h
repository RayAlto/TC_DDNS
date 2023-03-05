#ifndef TC_DNS_TC_CRYPT_H_
#define TC_DNS_TC_CRYPT_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace tc::crypt {

std::vector<std::uint8_t> hmac_sha256(const unsigned char* key,
                                      const std::size_t& key_len,
                                      const unsigned char* data,
                                      const std::size_t& data_len);
std::vector<std::uint8_t> hmac_sha256(const std::string& key,
                                      const std::string& data);
std::vector<std::uint8_t> hmac_sha256(const std::vector<std::uint8_t>& key,
                                      const std::vector<std::uint8_t>& data);

} // namespace tc::crypt

#endif // TC_DNS_TC_CRYPT_H_
