#ifndef TC_DNS_TC_CONFIG_H_
#define TC_DNS_TC_CONFIG_H_

#include <string>

namespace tc {

struct Config {
    std::string secret_id;
    std::string secret_key;
};

} // namespace tc

#endif // TC_DNS_TC_CONFIG_H_
