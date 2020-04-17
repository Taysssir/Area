#ifndef ECOMMON_CONSTANTS_H
#define ECOMMON_CONSTANTS_H

template <char... Chars>
inline std::string obfuscation()
{
    return std::string({Chars..., '\0'});
}

namespace constants
{

    //constexpr int MAP_WIDTH = 4096;

    //constexpr int MAP_HEIGHT = 4096;

    namespace map
    {

        inline int & width()
        {
            static int width = 4096;
            return width;
        }

        inline int & height()
        {
            static int height = 4096;
            return height;
        }

    }

}

#endif // ECOMMON_CONSTANTS_H
