#ifndef SENTINEL_LOADER_H
#define SENTINEL_LOADER_H

#include <vector>
#include <memory>
#include <functional>
#include <cstring>
#include <locale>
#include <sstream>

#include "resource.hpp"
#include "tinyxml.h"

namespace eos
{
namespace eosconfig
{
  namespace priv
  {
    constexpr char X[] = "x";
    constexpr char Y[] = "y";
    constexpr char TIMESTAMP[] = "timestamp";
  }

  class Loader
  {
  public:
    typedef std::unique_ptr<Loader> Ptr;
    virtual ~Loader()
    {
    }

    virtual BaseResource::Ptr load(RawData const &data) const = 0;

    virtual bool save(RawData &data, BaseResource::Ptr const &rsc) const = 0;
  };

  namespace xmlhelpers
  {
    inline TiXmlElement *createDocument(std::string const &root_tag, TiXmlDocument &doc)
    {
      TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "", "");
      doc.LinkEndChild(decl);
      TiXmlElement *root = new TiXmlElement(root_tag.c_str());
      doc.LinkEndChild(root);
      return root;
    }

    inline bool dump(TiXmlDocument const &doc, std::string &data)
    {
      TiXmlPrinter printer;
      printer.SetIndent("    ");
      doc.Accept(&printer);
      data = printer.CStr();
      return data.size() != 0;
    }

    template <typename T>
    inline void dump(TiXmlNode *node, std::string const &tag, T const &value)
    {
      TiXmlElement *type = new TiXmlElement(tag.c_str());
      std::stringstream ss;
      ss << value;
      type->LinkEndChild(new TiXmlText(ss.str().c_str()));
      node->LinkEndChild(type);
    }

    inline void dump_x_y(TiXmlNode *node, std::string const &tag, int x, int y)
    {
      TiXmlElement *position = new TiXmlElement(tag.c_str());
      dump(position, priv::X, x);
      dump(position, priv::Y, y);
      node->LinkEndChild(position);
    }

    inline void dump_timestamp(TiXmlNode *node, std::uint64_t timestamp)
    {
      dump<std::uint64_t>(node, priv::TIMESTAMP, timestamp);
    }

    inline std::string read(TiXmlNode *node, std::string const &name)
    {
      if (node)
      {
        TiXmlElement *element = node->FirstChildElement(name.c_str());
        if (element)
        {
          if (element->GetText())
            return std::string(element->GetText());
        }
      }
      return "";
    }

    template <typename Type>
    inline Type read_type(TiXmlNode *node, std::string const &name)
    {
      return static_cast<Type>(atoi(read(node, name).c_str()));
    }

    inline bool read_bool(TiXmlNode *node, std::string const &name)
    {
      std::string value = read(node, name);
      return value.compare("yes") == 0 || value.compare("true") == 0 || value.compare("1") == 0;
    }

    inline bool read_float(TiXmlNode *node, std::string const &name)
    {
      return atof(read(node, name).c_str());
    }

    inline int read_int(TiXmlNode *node, std::string const &name)
    {
      return std::atoi(read(node, name).c_str());
    }

    inline void read_x_y(TiXmlNode *node, std::string const &name, int &x, int &y)
    {
      TiXmlElement *node_sup = node->FirstChildElement(name.c_str());
      if (node_sup)
      {
        x = read_int(node_sup, priv::X);
        y = read_int(node_sup, priv::Y);
      }
    }

    template <typename T>
    T read_tuple(TiXmlNode *xml, std::string const &name)
    {
      int x, y;
      xmlhelpers::read_x_y(xml, name, x, y);
      return T(x, y);
    }

    inline std::int64_t read_int64(TiXmlNode *node, std::string const& name)
    {
      std::istringstream o(read(node, name).c_str());
      std::int64_t ret = 0;
      o >> ret;
      return ret;
    }

    inline std::uint64_t read_timestamp(TiXmlNode *node)
    {
      std::istringstream o(read(node, priv::TIMESTAMP).c_str());
      std::uint64_t ret = 0;
      o >> ret;
      return ret;
    }
  } // class Loader
} // eosconfig
} // eos

#endif // SENTINEL_LOADER_H
