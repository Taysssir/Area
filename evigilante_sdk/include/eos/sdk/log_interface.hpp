#ifndef EVIGILANTE_SDK_LOG_INTERFACE_HPP
#define EVIGILANTE_SDK_LOG_INTERFACE_HPP

#include <string>

namespace eos
{

    namespace sdk
    {

        class LogInterface
        {
            public:
                typedef enum
                {
                   Nothing,
                   Warning,
                   Info,
                   Debug,
                } Verbosity;

                /**
                 * @brief configure the level of verbosity
                 * @param verbosity
                 */
                void configure(Verbosity verbosity);

                /**
                 * @brief activate console output by default : false
                 * @param enable
                 */
                void activateScreenOutput(bool enable);

                /**
                 * @brief activate file output by default : false
                 * @param enable
                 * @param directory
                 * @return
                 */
                void activateFileOutput(bool enable, std::string const& directory = "");
        };

    } // namespace sdk

} // namespace eos

#endif // EVIGILANTE_SDK_LOG_INTERFACE_HPP
