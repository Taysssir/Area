#include <cstring>
#include <cwchar>

#include "packet.hpp"
#include "socket.hpp"

namespace eos
{
  namespace sdk
  {
    Packet Packet::None;

    Packet::Packet()
        : readPos_(0)
        , isValid_(true)
    {
    }

    Packet::~Packet()
    {
    }

    void Packet::append(const void *data, std::size_t sizeInBytes)
    {
      if (data && (sizeInBytes > 0))
      {
        std::size_t start = data_.size();
        data_.resize(start + sizeInBytes);
        std::memcpy(&data_[start], data, sizeInBytes);
      }
    }

    void Packet::clear()
    {
      data_.clear();
      readPos_ = 0;
      isValid_ = true;
    }

    const void *Packet::getData() const
    {
      return !data_.empty() ? &data_[0] : NULL;
    }

    std::size_t Packet::getDataSize() const
    {
      return data_.size();
    }

    bool Packet::endOfPacket() const
    {
      return readPos_ >= data_.size();
    }

    Packet &Packet::operator>>(int &data)
    {
      if (checkSize(sizeof(data)))
      {
        data = ntohl(*reinterpret_cast<const unsigned int *>(&data_[readPos_]));
        readPos_ += sizeof(data);
      }

      return *this;
    }

    Packet &Packet::operator>>(unsigned int &data)
    {
      if (checkSize(sizeof(data)))
      {
        data = ntohl(*reinterpret_cast<const unsigned int *>(&data_[readPos_]));
        readPos_ += sizeof(data);
      }

      return *this;
    }

    Packet &Packet::operator>>(char *data)
    {
      unsigned int length = 0;
      *this >> length;

      if ((length > 0) && checkSize(length))
      {
        std::memcpy(data, &data_[readPos_], length);
        data[length] = '\0';

        readPos_ += length;
      }

      return *this;
    }

    Packet &Packet::operator>>(std::string &data)
    {
      unsigned int length = 0;
      *this >> length;

      data.clear();
      if ((length > 0) && checkSize(length))
      {
        data.assign(&data_[readPos_], length);

        readPos_ += length;
      }

      return *this;
    }

    Packet &Packet::operator>>(std::vector<char> &data)
    {
      unsigned int length = 0;
      *this >> length;

      data.clear();
      if ((length > 0) && checkSize(length))
      {
        data.resize(length);
        std::memcpy(&data[0], &data_[readPos_], length);

        readPos_ += length;
      }

      return *this;
    }

    Packet &Packet::operator<<(int data)
    {
      unsigned int toWrite = htonl(data);
      append(&toWrite, sizeof(toWrite));
      return *this;
    }

    Packet &Packet::operator<<(unsigned int data)
    {
      unsigned int toWrite = htonl(data);
      append(&toWrite, sizeof(toWrite));
      return *this;
    }

    Packet &Packet::operator<<(const char *data)
    {
      unsigned int length = static_cast<unsigned int>(std::strlen(data));
      *this << length;

      append(data, length * sizeof(char));

      return *this;
    }

    Packet &Packet::operator<<(const std::string &data)
    {
      unsigned int length = static_cast<unsigned int>(data.size());
      *this << length;

      if (length > 0)
        append(data.c_str(), length * sizeof(std::string::value_type));

      return *this;
    }

    Packet &Packet::operator<<(const std::vector<char> &data)
    {
      unsigned int length = static_cast<unsigned int>(data.size());
      *this << length;

      if (length > 0)
        append(&data[0], length * sizeof(char));

      return *this;
    }

    int32_t Packet::getType() const
    {
        if (data_.size() > 0)
        {
          return ntohl(*reinterpret_cast<const int32_t *>(&data_[0]));
        }
        return -1;
    }

    bool Packet::checkSize(std::size_t size)
    {
      isValid_ = isValid_ && (readPos_ + size <= data_.size());

      return isValid_;
    }
  }
}
