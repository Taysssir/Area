#include "eos/sdk/log_interface.hpp"
#include "tcp/logger.hpp"

namespace eos
{

  namespace sdk
  {

    void LogInterface::configure(Verbosity )
    {
        // unused for the moment
    }

    void LogInterface::activateScreenOutput(bool enable)
    {
        Logger::instance()->activateScreenOutput(enable);
    }

    void LogInterface::activateFileOutput(bool enable, std::string const& directory)
    {
        if(enable)
          Logger::instance()->open(directory + "eos_sdk.log");
        else
          Logger::instance()->close();
    }

  } // namespace sdk

} // namespace eos
