#ifndef TC_DNS_TC_CONFIG_H_
#define TC_DNS_TC_CONFIG_H_

#include <string>

namespace tc {

struct Config {
    std::string secret_id;
    std::string secret_key;

    static Config from_env(const std::string& id_env_name = "TC_SECRET_ID",
                           const std::string& key_env_name = "TC_SECRET_KEY");
};

} // namespace tc

#endif // TC_DNS_TC_CONFIG_H_
