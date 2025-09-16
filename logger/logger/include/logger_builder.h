#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_LOGGER_BUILDER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_LOGGER_BUILDER_H

#include "logger.h"

//АБК, чистые виртуальные функции

class logger_builder
{

public:

    virtual ~logger_builder() noexcept = default;       //Вирт. деструктор по умолчанию

public:

    virtual logger_builder& add_file_stream(        //Добавляет файловый поток для записи логов
        std::string const &stream_file_path,        // rval ref (can't be used on tmp object)
        logger::severity severity) & = 0;

    virtual logger_builder& add_console_stream(     // Консольный поток
        logger::severity severity) & = 0;

    virtual logger_builder& transform_with_configuration(   //Настроить строителя на основе данной конфигурации
        std::string const &configuration_file_path,         //путь к файлу с конфигом, 
        std::string const &configuration_path) & = 0;       //второе хз

    virtual logger_builder& clear() & = 0;                  //очистка текущего строителя (сброс потоков, формата и т.д.)

    //do not ignore the return value. We must assign the return value
    [[nodiscard]] virtual logger* build() const = 0;        //Фабричный метод. Создаёт и возвращает готовый логгер

    virtual logger_builder& set_format(const std::string& format) & =0;     //Установка формата логов

    virtual logger_builder& set_destination(const std::string& format) & =0;        //Адрес назначения, сервер??

    static logger::severity string_to_severity(     //TODO: IS DONE
        std::string const &severity_string);        //literally str to sev

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_LOGGER_BUILDER_H