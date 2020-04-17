#include "settings_loader.hpp"
#include "eos/configuration/models.pb.h"

namespace eos
{
namespace eosconfig
{
namespace tag
{
constexpr char SETTINGS[] = "settings";
constexpr char KEYS[] = "keys";
constexpr char VALUE[] = "value";
}

namespace
{
constexpr char DELIMITER[] = "#delimiter#";

typedef std::vector<std::string> StringList;

StringList split(std::string const &input, std::string const &delimiter = DELIMITER)
{
    StringList list;
    if (input.size() == 0) return list;
    size_t start = 0;
    size_t loc = input.find(delimiter);
    while (loc != std::string::npos)
    {
        std::string s = input.substr(start, loc - start);
        if (s.size())
            list.push_back(s);
        start += list.back().size() + delimiter.size();
        loc = input.find(delimiter, start);
    }
    std::string s = input.substr(start, loc - start);
    if (s.size())
        list.push_back(s);
    return list;
}

std::string join(StringList const &input, std::string const &delimiter = DELIMITER)
{
    std::string output = "";
    for (auto const &str : input)
        output += str + delimiter;
    return output;
}
}

BaseResource::Ptr SettingsLoader::load(RawData const &data) const
{
    TiXmlDocument doc;
    doc.Parse(data.c_str(), 0, TIXML_DEFAULT_ENCODING);
    auto settings = rsc::alloc<Settings>();
    TiXmlNode *settings_xml = doc.FirstChildElement(tag::SETTINGS);

    settings->set_timestamp(xmlhelpers::read_timestamp(doc.RootElement()));

    if (settings_xml)
    {
        StringList keys = split(xmlhelpers::read(settings_xml,tag::KEYS));
        StringList values = split(xmlhelpers::read(settings_xml,tag::VALUE));
        for (unsigned int i(0);i < keys.size();++i)
        {
            ::eos::JSONSettings* JSON = settings->mutable_settings()->Add();
            JSON->set_keys(keys[i]);
            if(values.size() > i)
                JSON->set_value(values[i]);
        }
    }
    return settings;
}

bool SettingsLoader::save(RawData &data, BaseResource::Ptr const &rsc) const
{
    TiXmlDocument doc;
    TiXmlElement *root = xmlhelpers::createDocument(tag::SETTINGS, doc);

    auto settings = rsc::as<Settings>(rsc);

    xmlhelpers::dump_timestamp(root, settings->timestamp());

    StringList keys;
    StringList values;
    for (int i(0);i < settings->settings().size();++i)
    {
        ::eos::JSONSettings JSON = settings->settings(i);
        keys.push_back(JSON.keys());
        values.push_back(JSON.value());
    }
    xmlhelpers::dump(root, tag::KEYS, join(keys));
    xmlhelpers::dump(root, tag::VALUE, join(values));

    return xmlhelpers::dump(doc, data);
}
}
}
