#ifndef EOS_EVIGILANTE_CHANNEL_HPP
#define EOS_EVIGILANTE_CHANNEL_HPP

#include "msgs.pb.h"

namespace eos
{
  namespace sdk
  {
    namespace channel
    {
      typedef enum : int
      {
        Command = 3,
        Data
      } Type;
    }
  }
}

#endif // EOS_EVIGILANTE_CHANELL_HPP
