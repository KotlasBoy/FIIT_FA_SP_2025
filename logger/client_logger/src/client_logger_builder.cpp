#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include "operation_not_supported.h"            

using namespace nlohmann;

logger_builder& client_logger_builder::add_file_stream(    
    std::string const &stream_file_path,
    logger::severity severity) &
{
    std::filesystem::path path(stream_file_path);
    if (path.has_parent_path())
    {
        std::filesystem::create_directories(path.parent_path());
    }

    auto& [stream_list, is_console_output] = _output_streams[severity];
    for (const auto& current_stream : stream_list)
    {
        std::string current_stream_path = current_stream._stream.first; 
        if (current_stream_path == stream_file_path)
        {
            return *this;
        }
    }
    _output_streams[severity].first.emplace_front(client_logger::refcounted_stream(stream_file_path));
    _output_streams[severity].second = true;        //Always add console output
    return *this;
}

logger_builder& client_logger_builder::add_console_stream(
    logger::severity severity) &
{
    //_output_streams[severity].first.emplace_front(client_logger::refcounted_stream(""));      // Вроде как у нас бул отвечает за консоль
    _output_streams[severity].second = true;                                                
    return *this;
}

logger_builder& client_logger_builder::transform_with_configuration(   
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    std::ifstream i_file(configuration_file_path);
    if (!i_file.is_open())
    {
        throw std::runtime_error("Cannot open configuration file: " + configuration_file_path);
    }

    json config;
    try {
        i_file >> config;
    } catch ( const json::parse_error& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
    i_file.close();

    auto settings = config.at(configuration_path);


    if (settings.contains("format"))
    {
        _format = settings["format"].get<std::string>();
    }
    
    if (settings.contains("streams"))
    {
        for (const auto& stream : settings["streams"])
        {
            std::string type = stream["type"].get<std::string>();

            logger::severity sev;
            std::string severity_str = stream["severity"].get<std::string>();

            parse_severity(severity_str, sev);

            if (type == "console")
            {
                add_console_stream(sev);
            }
            else if (type == "file")
            {
                std::string path = stream["path"].get<std::string>();
                add_file_stream(path, sev);
            }
            else
            {
                throw std::runtime_error("Unknown stream type: " + type);
            }
        }
    }

    return *this;
}

logger_builder& client_logger_builder::clear() &
{
    _output_streams.clear();
    _format = "%m";
    return *this;
}

logger *client_logger_builder::build() const
{
    if (_output_streams.empty())
    {
        throw std::runtime_error("There is no configured output streams");
    }
    return new client_logger(_output_streams, _format);
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
    _format = format;
    return *this;
}


void client_logger_builder::parse_severity(const std::string &sev_str, logger::severity &sev)
{
    if (sev_str == "trace") sev = logger::severity::trace;
    else if (sev_str == "debug") sev = logger::severity::debug;
    else if (sev_str == "information") sev = logger::severity::information;
    else if (sev_str == "warning") sev = logger::severity::warning;
    else if (sev_str == "error") sev = logger::severity::error;
    else if (sev_str == "critical") sev = logger::severity::critical;
    else{
        throw std::runtime_error("Unknown severity level: " + sev_str);
    } 

}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    throw operation_not_supported();     
}
