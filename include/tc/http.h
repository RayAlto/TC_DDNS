#ifndef TC_DNS_TC_HTTP_H_
#define TC_DNS_TC_HTTP_H_

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace tc::http {

struct Response {
    double time_elapsed;
    std::int64_t bytes_downloaded;
    std::int64_t bytes_uploaded;
    std::string local_ip;
    std::int64_t local_port;
    std::string message;
    std::int64_t http_version;
    std::int64_t code;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> cookies;
    std::string body;
};

class Request {
public:
    Request() = delete;
    Request(const Request&) = delete;
    Request(Request&&) noexcept = delete;
    Request& operator=(const Request&) = delete;
    Request& operator=(Request&&) noexcept = delete;

    explicit Request(const std::string& url);
    explicit Request(const std::string& url,
                     const std::map<std::string, std::string>& params);
    virtual ~Request();

    Request& method(const std::string& method);
    Request& resolve(const std::vector<std::string>& resolve);
    Request& headers(const std::map<std::string, std::string>& headers);
    Request& cookies(const std::map<std::string, std::string>& cookies);
    Request& body(const std::string& body);
    Request& body(const std::vector<std::uint8_t>& body);
    Request& add_part_from_data(
        const std::string& name,
        const std::variant<std::string, std::vector<std::uint8_t>>& data,
        const std::string& file_name = {},
        const std::string& mime_type = {});
    Request& add_part_from_file(const std::string& name,
                                const std::string& file,
                                const std::string& file_name = {},
                                const std::string& mime_type = {});
    Response perform();

    static std::string url_encode(const std::string& data);
    static std::string url_encode(const std::vector<std::uint8_t>& data);

    static std::string url_decode_2str(const std::string& url_encoded);
    static std::vector<std::uint8_t> url_decode_2bytes(
        const std::string& url_encoded);

protected:
    void* curl_handle_ = nullptr;
    void* curl_resolve_ = nullptr;
    void* curl_headers_ = nullptr;
    void* curl_mime_ = nullptr;
};

} // namespace tc::http

#endif // TC_DNS_TC_HTTP_H_
