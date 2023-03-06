#include "tc/config.h"

#include <cstdlib>

namespace tc {

Config Config::from_env(const std::string& id_env_name,
                        const std::string& key_env_name) {
    Config c {};
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    const char* id_from_env = std::getenv(id_env_name.c_str());
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    const char* key_from_env = std::getenv(key_env_name.c_str());
    if (id_from_env != nullptr) {
        c.secret_id = id_from_env;
    }
    if (key_from_env != nullptr) {
        c.secret_key = key_from_env;
    }
    return c;
}

} // namespace tc
