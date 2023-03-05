#include <cstddef>
#include <cstdint>
#include <vector>

#include "openssl/hmac.h"
#include "openssl/evp.h"

#include "tc/crypt.h"

namespace tc::crypt {

std::vector<std::uint8_t> hmac_sha256(const unsigned char* key,
                                      const std::size_t& key_len,
                                      const unsigned char* data,
                                      const std::size_t& data_len) {
    unsigned char buf[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(EVP_sha256(),
         key,
         static_cast<int>(key_len),
         data,
         data_len,
         buf,
         &len);
    return {buf, buf + len};
}

std::vector<std::uint8_t> hmac_sha256(const std::string& key,
                                      const std::string& data) {
    return hmac_sha256(reinterpret_cast<const unsigned char*>(key.c_str()),
                       key.length(),
                       reinterpret_cast<const unsigned char*>(data.c_str()),
                       data.length());
}

std::vector<std::uint8_t> hmac_sha256(const std::vector<std::uint8_t>& key,
                                      const std::vector<std::uint8_t>& data) {
    return hmac_sha256(reinterpret_cast<const unsigned char*>(key.data()),
                       key.size(),
                       reinterpret_cast<const unsigned char*>(data.data()),
                       data.size());
}

} // namespace tc::crypt
