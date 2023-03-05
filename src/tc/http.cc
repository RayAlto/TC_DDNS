#include "tc/http.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "curl/curl.h"

namespace tc::http {

std::size_t curl_custom_write_function(char* ptr,
                                       std::size_t size,
                                       std::size_t nmemb,
                                       std::string* userdata) {
    std::size_t actual_size = size * nmemb;
    userdata->append(ptr, actual_size);
    return actual_size;
}

std::size_t curl_custom_header_function(
    char* buffer,
    std::size_t size,
    std::size_t nitems,
    std::map<std::string, std::string>* userdata) {
    std::size_t actual_size = size * nitems;
    std::string header_str(buffer, actual_size);
    if (header_str.empty()) {
        // empty line
        return actual_size;
    }
    header_str.erase(std::find_if(header_str.rbegin(),
                                  header_str.rend(),
                                  [](const char& c) { return std::isspace(c); })
                         .base(),
                     header_str.end());
    std::size_t colon = header_str.find(':');
    if (colon == std::string::npos) {
        // Something like "Str"
        return actual_size;
    }
    std::size_t value_start = colon + 1;
    while (value_start < header_str.length()
           && (std::isspace(header_str[value_start]) != 0)) {
        value_start++;
    }
    userdata->emplace(header_str.substr(0, colon),
                      header_str.substr(value_start));
    return actual_size;
}

void concatenate_header(const std::pair<std::string, std::string>& header,
                        std::string& header_str) {
    if (header.second.empty()) {
        header_str = header.first + ':';
        return;
    }
    header_str = header.first + ": " + header.second;
}

Request::Request(const std::string& url) : curl_handle_(curl_easy_init()) {
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl_handle_, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl_handle_, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(
        curl_handle_, CURLOPT_WRITEFUNCTION, curl_custom_write_function);
    curl_easy_setopt(
        curl_handle_, CURLOPT_HEADERFUNCTION, curl_custom_header_function);
}

Request::Request(const std::string& url,
                 const std::map<std::string, std::string>& params) :
    curl_handle_(curl_easy_init()) {
    std::string url_with_params = url + '?';
    std::map<std::string, std::string>::const_iterator param_iter =
        params.begin();
    std::map<std::string, std::string>::const_iterator param_iter_last =
        --(params.end());
    while (param_iter != param_iter_last) {
        url_with_params += url_encode(param_iter->first) + '='
                           + url_encode(param_iter->second) + '&';
        param_iter++;
    }
    url_with_params +=
        url_encode(param_iter->first) + '=' + url_encode(param_iter->second);
    curl_easy_setopt(curl_handle_, CURLOPT_URL, url_with_params.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl_handle_, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl_handle_, CURLOPT_COOKIEFILE, "");
    curl_easy_setopt(
        curl_handle_, CURLOPT_WRITEFUNCTION, curl_custom_write_function);
    curl_easy_setopt(
        curl_handle_, CURLOPT_HEADERFUNCTION, curl_custom_header_function);
}

Request::~Request() {
    curl_easy_cleanup(curl_handle_);
    curl_slist_free_all(reinterpret_cast<curl_slist*>(curl_resolve_));
    curl_slist_free_all(reinterpret_cast<curl_slist*>(curl_headers_));
    curl_mime_free(reinterpret_cast<curl_mime*>(curl_mime_));
}

Request& Request::method(const std::string& method) {
    curl_easy_setopt(curl_handle_, CURLOPT_CUSTOMREQUEST, method.c_str());
    return *this;
}

Request& Request::resolve(const std::vector<std::string>& resolve) {
    if (resolve.empty()) {
        return *this;
    }
    curl_slist** p_curl_resolve =
        reinterpret_cast<curl_slist**>(&curl_resolve_);
    if (*p_curl_resolve != nullptr) {
        curl_slist_free_all(*p_curl_resolve);
        *p_curl_resolve = nullptr;
    }
    std::vector<std::string>::const_iterator resolve_iter = resolve.begin();
    *p_curl_resolve = curl_slist_append(nullptr, resolve_iter->c_str());
    resolve_iter++;
    if (resolve_iter != resolve.end()) {
        curl_slist* temp =
            curl_slist_append(*p_curl_resolve, resolve_iter->c_str());
        resolve_iter++;
        while (resolve_iter != resolve.end()) {
            temp = curl_slist_append(temp, resolve_iter->c_str());
            resolve_iter++;
        }
    }
    curl_easy_setopt(curl_handle_, CURLOPT_RESOLVE, *p_curl_resolve);
    return *this;
}

Request& Request::headers(const std::map<std::string, std::string>& headers) {
    if (headers.empty()) {
        return *this;
    }
    curl_slist** p_curl_headers =
        reinterpret_cast<curl_slist**>(&curl_headers_);
    if (*p_curl_headers != nullptr) {
        curl_slist_free_all(*p_curl_headers);
        *p_curl_headers = nullptr;
    }
    std::map<std::string, std::string>::const_iterator headers_iter =
        headers.begin();
    std::string header_str;
    concatenate_header(*headers_iter, header_str);
    *p_curl_headers = curl_slist_append(nullptr, header_str.c_str());
    headers_iter++;
    if (headers_iter != headers.end()) {
        concatenate_header(*headers_iter, header_str);
        curl_slist* temp =
            curl_slist_append(*p_curl_headers, header_str.c_str());
        headers_iter++;
        while (headers_iter != headers.end()) {
            concatenate_header(*headers_iter, header_str);
            temp = curl_slist_append(temp, header_str.c_str());
            headers_iter++;
        }
    }
    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, *p_curl_headers);
    return *this;
}

Request& Request::cookies(const std::map<std::string, std::string>& cookies) {
    if (cookies.empty()) {
        return *this;
    }
    std::string cookies_str;
    std::map<std::string, std::string>::const_iterator cookies_iter =
        cookies.begin();
    std::map<std::string, std::string>::const_iterator cookies_last_iter =
        --cookies.end();
    while (cookies_iter != cookies_last_iter) {
        cookies_str += cookies_iter->first + '=' + cookies_iter->second + "; ";
        cookies_iter++;
    }
    cookies_str += cookies_iter->first + '=' + cookies_iter->second + ";";
    curl_easy_setopt(curl_handle_, CURLOPT_COOKIE, cookies_str.c_str());
    return *this;
}

Request& Request::body(const std::string& body) {
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE_LARGE, body.length());
    curl_easy_setopt(curl_handle_, CURLOPT_COPYPOSTFIELDS, body.c_str());
    return *this;
}

Request& Request::body(const std::vector<std::uint8_t>& body) {
    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE_LARGE, body.size());
    curl_easy_setopt(curl_handle_, CURLOPT_COPYPOSTFIELDS, body.data());
    return *this;
}

Request& Request::add_part_from_data(
    const std::string& name,
    const std::variant<std::string, std::vector<std::uint8_t>>& data,
    const std::string& file_name,
    const std::string& mime_type) {
    curl_mime** p_curl_mime = reinterpret_cast<curl_mime**>(&curl_mime_);
    if (*p_curl_mime == nullptr) {
        *p_curl_mime = curl_mime_init(curl_handle_);
    }
    curl_mimepart* part = curl_mime_addpart(*p_curl_mime);
    curl_mime_name(part, name.c_str());
    if (std::holds_alternative<std::string>(data)) {
        const std::string& data_str = std::get<std::string>(data);
        curl_mime_data(part, data_str.c_str(), data_str.length());
    }
    if (std::holds_alternative<std::vector<std::uint8_t>>(data)) {
        const std::vector<std::uint8_t>& data_bytes =
            std::get<std::vector<std::uint8_t>>(data);
        curl_mime_data(part,
                       reinterpret_cast<const char*>(data_bytes.data()),
                       data_bytes.size());
    }
    if (!file_name.empty()) {
        curl_mime_filename(part, file_name.c_str());
    }
    if (!mime_type.empty()) {
        curl_mime_type(part, mime_type.c_str());
    }
    return *this;
}

Request& Request::add_part_from_file(const std::string& name,
                                     const std::string& file,
                                     const std::string& file_name,
                                     const std::string& mime_type) {
    curl_mime** p_curl_mime = reinterpret_cast<curl_mime**>(&curl_mime_);
    if (*p_curl_mime == nullptr) {
        *p_curl_mime = curl_mime_init(curl_handle_);
    }
    curl_mimepart* part = curl_mime_addpart(*p_curl_mime);
    curl_mime_name(part, name.c_str());
    curl_mime_filedata(part, file.c_str());
    if (!file_name.empty()) {
        curl_mime_filename(part, file_name.c_str());
    }
    if (!mime_type.empty()) {
        curl_mime_type(part, mime_type.c_str());
    }
    return *this;
}

Response Request::perform() {
    Response response {};
    char error_buf[CURL_ERROR_SIZE] {};
    curl_easy_setopt(curl_handle_, CURLOPT_ERRORBUFFER, error_buf);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl_handle_, CURLOPT_HEADERDATA, &response.headers);
    if (curl_mime_ != nullptr) {
        curl_mime* mime = reinterpret_cast<curl_mime*>(curl_mime_);
        curl_easy_setopt(curl_handle_, CURLOPT_MIMEPOST, mime);
    }
    CURLcode curl_result = curl_easy_perform(curl_handle_);
    response.message = (std::strlen(error_buf) == 0)
                           ? curl_easy_strerror(curl_result)
                           : error_buf;
    curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, &response.code);
    curl_easy_getinfo(
        curl_handle_, CURLINFO_HTTP_VERSION, &response.http_version);
    curl_easy_getinfo(
        curl_handle_, CURLINFO_TOTAL_TIME, &response.time_elapsed);
    curl_easy_getinfo(
        curl_handle_, CURLINFO_SIZE_UPLOAD_T, &response.bytes_uploaded);
    curl_easy_getinfo(
        curl_handle_, CURLINFO_SIZE_DOWNLOAD_T, &response.bytes_downloaded);
    char* ip = nullptr;
    curl_easy_getinfo(curl_handle_, CURLINFO_LOCAL_IP, &ip);
    response.local_ip = ip;
    curl_easy_getinfo(curl_handle_, CURLINFO_LOCAL_PORT, &response.local_port);
    return response;
}

std::string Request::url_encode(const std::string& data) {
    char* encoded = curl_easy_escape(
        nullptr, data.c_str(), static_cast<int>(data.length()));
    std::string result = encoded;
    curl_free(encoded);
    return result;
}

std::string Request::url_encode(const std::vector<std::uint8_t>& data) {
    char* encoded = curl_easy_escape(nullptr,
                                     reinterpret_cast<const char*>(data.data()),
                                     static_cast<int>(data.size()));
    std::string result = encoded;
    curl_free(encoded);
    return result;
}

std::string Request::url_decode_2str(const std::string& url_encoded) {
    int decoded_len = 0;
    char* decoded = curl_easy_unescape(nullptr,
                                       url_encoded.c_str(),
                                       static_cast<int>(url_encoded.length()),
                                       &decoded_len);
    std::string result(decoded, decoded_len);
    curl_free(decoded);
    return result;
}

std::vector<std::uint8_t> Request::url_decode_2bytes(
    const std::string& url_encoded) {
    int decoded_len = 0;
    char* decoded = curl_easy_unescape(nullptr,
                                       url_encoded.c_str(),
                                       static_cast<int>(url_encoded.length()),
                                       &decoded_len);
    std::vector<std::uint8_t> result(decoded, decoded + decoded_len);
    curl_free(decoded);
    return result;
}

} // namespace tc::http
