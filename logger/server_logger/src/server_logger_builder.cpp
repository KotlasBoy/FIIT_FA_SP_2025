#include "../include/server_logger_builder.h"

#include <utility>
#include <fstream>
#include <filesystem>

logger_builder& server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
    auto opened_stream = _output_streams.find(severity());
    if (opened_stream == _output_streams.end())
    {
        opened_stream = _output_streams.emplace(severity, std::make_pair(std::string(), false)).first;
    }
    opened_stream->second.first = stream_file_path;
    return *this;
}

logger_builder& server_logger_builder::add_console_stream(
    logger::severity severity) &
{
    auto opened_stream = _output_streams.find(severity());
    if (opened_stream == _output_streams.end())
    {
        opened_stream = _output_streams.emplace(severity, std::make_pair(std::string(), false)).first;
    }

    opened_stream->second.second = true;
    return *this;
}

logger_builder& server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    json js;
    std::ifstream i_stream(configuration_file_path);
    if (i_stream.is_open())
    {

        json::parser_callback_t callback = [&configuration_path](int depth, json::parse_event_t event, json &parsed)
        {
            if (event == json::parse_event_t::key && depth == 1 && parsed != json(configuraton_path))
                return false;
            return true;
        }

        j = json::parse(stream, callback);
        i_stream.close();
        if (!j.contains(configuration_path))
            return *this;
        
        j = j[configuration_path];

        if (j.contains("format"))
        {
            set_format(j["format"]);
        }

        if (j.contains("streams"))
        {
            for (auto& stream_item : j["streams"])
            {
                std::string type = stream_item["type"];

                if (type == "file" && stream_item.contains("path") && stream_item.contains("severities"))
                {
                    std::string path = stream_item["path"];
                    for (auto& sev : stream_item["severities"])
                    {
                        add_file_stream(path, string_to_severity(sev.get<std::string>()));
                    }
                }
                else if (type == "console" && stream_item.contains("severities"))
                {
                    for (auto& sev : stream_item["severities"])
                    {
                        add_console_stream(string_to_severity(sev.get<std::string>()));
                    }
                }
            }
        }
    }

    return *this;
}

logger_builder& server_logger_builder::clear() &
{
    _output_streams.clear();
    _destination = "http://127.0.0.1:9200";
    _format = "%m";     //TODO:FIXME:
    return *this;
}

logger *server_logger_builder::build() const
{
    return new server_logger(_destination, _output_streams, _format);
}

logger_builder& server_logger_builder::set_destination(const std::string& dest) & : 
{
    _destination = dest;
    return *this;
}

logger_builder& server_logger_builder::set_format(const std::string &format) &
{
    _format = format;
    return *this;
}
