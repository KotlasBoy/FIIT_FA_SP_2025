//#include <httplib.h>      // is in server_logger.h
#include <memory>
#include <sstream>
#include "logger/server_logger/include/server_logger.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

server_logger::~server_logger() noexcept
{
    std::string pid = std::to_string(inner_getpid());
    if (_client) {
        auto res = _client->Get("/destroy?pid=" + pid);
    }
}

std::mutex server_logger::log_mutex;


logger& server_logger::log(
    const std::string &text,
    logger::severity severity) &
{
    std::lock_guard<std::mutex> lock(log_mutex);
    std::string pid = std::to_string(inner_getpid());
    std::string formatted_message = make_format(text, severity);
    std::string url = "/log?pid=" + "&sev=" + severity_to_string(severity) + "&message=" + formatted_message;

    if (_client)
    {
        auto res = _client->Get(url);

        if (!(res && res->status == 204))
        {
            std::cerr << "Logging failed!" << std::endl;
        }
    }
    return *this;
}

server_logger::server_logger(const std::string& dest,
                             const std::unordered_map<logger::severity, std::pair<std::string, bool>> &streams,
                             std::string format) : _client(std::make_unique<httplib::Client>(dest.c_str())),
                             _streams(streams),
                             _format(std::move(format))
{
    std::string pid = std::to_string(inner_getpid());
    for (const auto& [sev, stream_pair] : _streams)
    {
        auto url = "/init?pid=" + pid _ "&sev=" + severity_to_string(sev) + "&path=" + stream_pair.first + "&console=" + std::to_string(stream_info.second);
        if (_client)
        {
            auto res = _client->Get(url);
        }
    }



}

int server_logger::inner_getpid()
{
#ifdef _WIN32
    return ::_getpid();
#else
    return getpid();
#endif
}

std::string current_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::gmtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string current_time() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::gmtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

server_logger::flag server_logger::char_to_flag (char c) noexcept {
    switch(c) {
        case 'D':
        case 'd': return flag::DATE;
        case 'T':
        case 't': return flag::TIME;
        case 'S':
        case 's': return flag::SEVERITY;
        case 'M':
        case 'm': return flag::MESSAGE;
        default:  return flag::NO_FLAG;
    }
}

std::string server_logger::make_format (const std::string &message, severity sev) const {
    std::string result;
    bool in_format = false;
    for (char c : _format)
    {
        if (in_format)
        {
            switch(char_to_flag(c))
            {
                case flag::DATE:
                    result += current_date(); break;
                case flag::TIME:
                    result += current_time(); break;
                case flag::SEVERITY:
                    result += severity_to_string(sev); break;
                case flag::MESSAGE:
                    result += message;
                    break;
                default:
                    result += std::string("%") + c;
            }
            in_format = false;
        }
        else if (c == '%')
            in_format = true;
        else
            result += c;
    }
    return result;
}

//server_logger::server_logger(const server_logger &other) = delete;
//server_logger &server_logger::operator=(const server_logger &other) = delete;
//server_logger::server_logger(server_logger &&other) noexcept = default;
//server_logger &server_logger::operator=(server_logger &&other) noexcept = default;
