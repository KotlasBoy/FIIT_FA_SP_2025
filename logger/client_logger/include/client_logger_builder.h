#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H

#include <logger_builder.h>
#include <unordered_map>
#include <forward_list>
#include <nlohmann/json.hpp>
#include "client_logger.h"

class client_logger_builder final:
    public logger_builder
{
private:

    std::unordered_map<logger::severity ,std::pair<std::forward_list<client_logger::refcounted_stream>, bool>> _output_streams;

    std::string _format;                                            // формат вывода

    void parse_severity(const std::string &sev_str, logger::severity &sev);       // парсим sev из json'a

public:

    client_logger_builder() : _format("%m"){};                      // конструктор по умолчанию

    client_logger_builder(                                          // запрет конст копирования
        client_logger_builder const &other) =delete;

    client_logger_builder &operator=(
        client_logger_builder const &other) =delete;                // запрет конст присваивания 

    client_logger_builder(
        client_logger_builder &&other) noexcept =default;           // move консмтруктор?

    client_logger_builder &operator=(
        client_logger_builder &&other) noexcept =default;           // move operator=

    ~client_logger_builder() noexcept override =default;            // деструктор по дефолту

public:

    logger_builder& add_file_stream(                                            // Добавить файловый поток для конкретного уровня sev
        std::string const &stream_file_path,
        logger::severity severity) & override;

    logger_builder& add_console_stream(                                         // Добавить консольный поток для конкретного уровня sev
        logger::severity severity) & override;

    logger_builder& transform_with_configuration(
        std::string const &configuration_file_path,                             // путь к джейсону
        std::string const &configuration_path) & override;                      // имя объекта в джейсоне

    logger_builder& set_format(const std::string& format) & override;           // формат логов

    logger_builder& set_destination(const std::string& format) & override;      // not implemented?

    logger_builder& clear() & override;                                         // очистить все потоки и настройки

    [[nodiscard]] logger *build() const override;                               // создаёт объект с текущими настройками

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_CLIENT_LOGGER_BUILDER_H