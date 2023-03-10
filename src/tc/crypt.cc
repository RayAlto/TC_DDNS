#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

#include "openssl/crypto.h"
#include "openssl/evp.h"
#include "openssl/hmac.h"
#include "openssl/sha.h"

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

std::vector<std::uint8_t> hmac_sha256(
    const std::variant<std::string, std::vector<std::uint8_t>>& key,
    const std::variant<std::string, std::vector<std::uint8_t>>& data) {
    const unsigned char* p_key = nullptr;
    std::size_t key_len = 0;
    const unsigned char* p_data = nullptr;
    std::size_t data_len = 0;
    if (std::holds_alternative<std::string>(key)) {
        const std::string& key_str = std::get<std::string>(key);
        p_key = reinterpret_cast<const unsigned char*>(key_str.c_str());
        key_len = key_str.length();
    }
    else if (std::holds_alternative<std::vector<std::uint8_t>>(key)) {
        const std::vector<std::uint8_t>& key_bytes =
            std::get<std::vector<std::uint8_t>>(key);
        p_key = reinterpret_cast<const unsigned char*>(key_bytes.data());
        key_len = key_bytes.size();
    }
    if (std::holds_alternative<std::string>(data)) {
        const std::string& data_str = std::get<std::string>(data);
        p_data = reinterpret_cast<const unsigned char*>(data_str.c_str());
        data_len = data_str.length();
    }
    else if (std::holds_alternative<std::vector<std::uint8_t>>(data)) {
        const std::vector<std::uint8_t>& data_bytes =
            std::get<std::vector<std::uint8_t>>(data);
        p_data = reinterpret_cast<const unsigned char*>(data_bytes.data());
        data_len = data_bytes.size();
    }
    return hmac_sha256(p_key, key_len, p_data, data_len);
}

std::vector<std::uint8_t> sha256(const unsigned char* data,
                                 const std::size_t& data_len) {
    std::size_t digested_len = EVP_MD_size(EVP_sha256());
    unsigned char digest_buf[digested_len];
    if (SHA256(data, data_len, digest_buf) == nullptr) {
        return {};
    }
    return {digest_buf, digest_buf + digested_len};
}

std::vector<std::uint8_t> sha256(const std::string& data) {
    return sha256(reinterpret_cast<const unsigned char*>(data.c_str()),
                  data.length());
}

std::vector<std::uint8_t> sha256(const std::vector<std::uint8_t>& data) {
    return sha256(data.data(), data.size());
}

} // namespace tc::crypt
