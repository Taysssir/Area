#ifndef EOS_EVIGILANTE_IPADDRESS_HPP
#define EOS_EVIGILANTE_IPADDRESS_HPP

#include <istream>
#include <ostream>
#include <string>

namespace eos
{
  namespace sdk
  {
    /**
     * @brief The IpAddress class
     * handle ipaddress in unsigned int format
     */
    class IpAddress
    {
    public:
      /**
         * @brief create empty address
         */
      IpAddress();

      /**
         * @brief create address from string
         * accept hostname
         * @param address
         */
      IpAddress(std::string const &address);

      /**
         * @brief create address from unsigned int format
         * @param address
         */
      explicit IpAddress(unsigned int address);

      /**
         * @brief export address to a string
         * @return
         */
      std::string toString() const;

      /**
         * @brief export address to a unsigned int
         * @return
         */
      unsigned int toInteger() const;

      /**
         * @brief return the local address
         * @return
         */
      static IpAddress getLocalAddress();

      /**
         * @brief return the public address
         * @return
         */
      static IpAddress getPublicAddress();

      static IpAddress None;

    private:
      ///! internal address representation
      unsigned int address_;
    };

    bool operator==(const IpAddress &left, const IpAddress &right);

    bool operator!=(const IpAddress &left, const IpAddress &right);
  }
}

#endif // EOS_EVIGILANTE_IPADDRESS_HPP
