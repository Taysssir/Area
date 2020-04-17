#ifndef EOS_EVIGILANTE_PACKET_HPP
#define EOS_EVIGILANTE_PACKET_HPP

#include <string>
#include <vector>
#include "logger.hpp"
namespace eos
{
  namespace sdk
  {
    /**
     * @brief The Packet class
     * used to send data throw the socket
     * the header is composed of the size
     */
    class Packet
    {
    public:
      Packet();

      virtual ~Packet();

      /**
         * @brief append raw data
         * @param data
         * @param sizeInBytes
         */
      void append(const void *data, std::size_t sizeInBytes);

      /**
         * @brief empty the packet
         */
      void clear();

      /**
         * @brief getData
         * @return the pointer to the data
         */
      const void *getData() const;

      /**
         * @brief getDataSize
         * @return data size
         */
      std::size_t getDataSize() const;

      /**
         * @brief endOfPacket it's the end of the packet
         * @return true if the cursor is at the end
         */
      bool endOfPacket() const;
      int32_t getType() const;

      /**
         * @brief static attribut used to return a default value
         */
      static Packet None;

    public:
      Packet &operator>>(int &data);
      Packet &operator>>(unsigned int &data);
      Packet &operator>>(char *data);
      Packet &operator>>(std::string &data);
      Packet &operator>>(std::vector<char> &data);

      Packet &operator<<(int data);
      Packet &operator<<(unsigned int data);
      Packet &operator<<(const char *data);
      Packet &operator<<(const std::string &data);
      Packet &operator<<(const std::vector<char> &data);

    private:
      /**
         * @brief check if the size is correct
         * @param size how many bytes have to be present
         * @return true, enough bytes
         */
      bool checkSize(std::size_t size);

      ///! buffer
      std::vector<char> data_;
      ///! cursor
      std::size_t readPos_;
      ///! valid packet
      bool isValid_;
    };
  }
}

#endif // EOS_EVIGILANTE_PACKET_HPP
