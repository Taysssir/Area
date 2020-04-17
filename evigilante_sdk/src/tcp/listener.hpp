#ifndef EOS_EVIGILANTE_LISTENER_HPP
#define EOS_EVIGILANTE_LISTENER_HPP

#include "socket.hpp"

namespace eos
{
  namespace sdk
  {
    /**
     * @brief The Listener class
     *  tcp server
     */
    class Listener
        : public Socket
    {
    public:
      Listener();

      /**
             * @brief listen
             * @param port
             * @return
             */
      Status listen(Port port, size_t nbMax = 100);

      /**
             * @brief accept
             * @param socket
             * @return
             */
      Status accept(Socket &socket);
    };
  }
}

#endif // EOS_EVIGILANTE_LISTENER_HPP
