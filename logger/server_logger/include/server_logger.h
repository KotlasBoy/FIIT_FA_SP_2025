#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include <logger.h>
#include <unordered_map>
#include <memory>           //added
#include <httplib.h>        // added

class server_logger_builder;
class server_logger final:
    public logger
{

    std::unique_ptr<httplib::Client> _client;
    std::unordered_map<logger::severity, std::pair<std::string, bool>> _streams;
    std::string _format;

public:
    enum class flag { DATE, TIME, SEVERITY, MESSAGE, NO_FLAG };

private:
    static std::mutex log_mutex;
    std::mutex client_mutex;

protected:
    server_logger(const std::string& dest,
                  const std::unordered_map<logger::severity ,std::pair<std::string, bool>>& streams,
                  std::string format);

    friend server_logger_builder;

public:
    static int inner_getpid();
    std::string make_format(const std::string &message, severity sev) const;
    static flag char_to_flag(char c) noexcept;

    server_logger(server_logger const &other) = delete;
    server_logger &operator=(server_logger const &other) = delete;

    server_logger(server_logger &&other) noexcept = default;
    server_logger &operator=(server_logger &&other) noexcept = default;

    ~server_logger() noexcept final;

public:

    [[nodiscard]] logger& log(
        const std::string &message,
        logger::severity severity) & override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H