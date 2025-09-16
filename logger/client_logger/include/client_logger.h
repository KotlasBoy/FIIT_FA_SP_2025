#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H

#include <logger.h>
#include <array>
#include <unordered_map>        //Храним потоки вывода по уровням важности
#include <forward_list>         //Храним список потоков
#include <ctime>
#include <fstream>

#define DATE_TIME_STR_LENGTH 40

class client_logger_builder;

class client_logger final:
    public logger
{
private:
    //region refcounted_stream

    class refcounted_stream final
    {       // <path_to_file, <counter, ofstream>>
        static std::unordered_map<std::string, std::pair<size_t, std::ofstream>> _global_streams;       // Статическая анордеред мапа для хранения потоков

        std::pair<std::string, std::ofstream*> _stream;
        friend client_logger;
        friend client_logger_builder;
    public:

        explicit refcounted_stream(const std::string& path);            // Explicit constructor

        refcounted_stream(const refcounted_stream& oth);                // Const Copy constructor

        refcounted_stream& operator=(const refcounted_stream& oth);     // Const Operator =

        refcounted_stream(refcounted_stream&& oth) noexcept;            // Move constructor

        refcounted_stream& operator=(refcounted_stream&& oth) noexcept; // Move Operator =

        //if ofstream* is nullptr initializes it with opened file from global map
        void open();                                                    //open stream

        ~refcounted_stream();                                           //Destructor, if ref counter == 0
    };

    //region refcounted_stream

    enum class flag                                                     // msg format flags
    { DATE, TIME, SEVERITY, MESSAGE, NO_FLAG };

private:

    std::unordered_map<logger::severity ,std::pair<std::forward_list<refcounted_stream>, bool>> _output_streams;

    std::string _format;


private:

    //opens all streams         // streams (key - severity, value -- pair of list + bool parameter)             //Constructor
    client_logger(const std::unordered_map<logger::severity ,std::pair<std::forward_list<refcounted_stream>, bool>>& streams, std::string format);

    std::string make_format(const std::string& message, severity sev) const;

    static flag char_to_flag(char c) noexcept;

    friend client_logger_builder;

public:

    client_logger(client_logger const &other);                      // Const Copy constructor

    client_logger &operator=(client_logger const &other);           // Const operator =

    client_logger(client_logger &&other) noexcept;                  // Move constructor

    client_logger &operator=(client_logger &&other) noexcept;       //Move operator =

    ~client_logger() noexcept final;                                //destructor

public:

    [[nodiscard]] logger& log(                                      // override log(msg, sev)       // [[nodiscard]] -- return value shouldn't be ingored. CE
        const std::string &message,
        logger::severity severity) & override;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_H