#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_LOGGER_H

#include <iostream>

class logger    //Абстрактный класс
{

public:
                            //Severity -- серьёзность бага
    enum class severity     //уровни важности сообщений
    {
        trace,
        debug,
        information,
        warning,
        error,
        critical
    };
    // Это вложенный тип, поэтому вызывается через logger::severity

public:
                                                            //Virtual -- can be override in derived class
    virtual ~logger() noexcept = default;                   //noexcept -- won't throw any exception

public:
                                                //Можно реализовать цепочки вызовов
    virtual logger& log(                        //принимает msg, sev и вызывает соотв. обёртку
        std::string const &message,
        logger::severity severity) & = 0;       //lvalue objects    // = 0 - pure virtual

public:

    logger& trace (                             //TODO: all wrappers are done
        std::string const &message) &;

    logger& debug(
        std::string const &message) &;

    logger& information(
        std::string const &message) &;

    logger& warning(
        std::string const &message) &;

    logger& error(
        std::string const &message) &;

    logger& critical(
        std::string const &message) &;

protected:                                  //TODO: all helpers are done
    //Вспомогательные методы для преобразования параметров в строку
    static std::string severity_to_string(              //severity code to str
        logger::severity severity);

    static std::string current_datetime_to_string();    //date && time to str

    static std::string current_date_to_string();        //date to str

    static std::string current_time_to_string();        //time to str

};


#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_LOGGER_H