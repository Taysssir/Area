#ifndef ZIP_EOS_CONFIGURATION_ARCHIVE_H
#define ZIP_EOS_CONFIGURATION_ARCHIVE_H

#include <cstdio>
#include <string>
#include <vector>
#include <cstdint>

struct zip;

namespace eos
{
  namespace eosconfig
  {
    class ZipArchive
    {
    public:
      enum OpenMode
      {
        NotOpen,
        ReadOnly,
        Write,
        New
      };

      enum State
      {
        Original,
        Current
      };

      class Entry
      {
        friend class ZipArchive;

      public:
        Entry()
            : zipFile_(0)
            , index_(0)
            , time_(0)
            , method_(-1)
            , size_(0)
            , sizeComp_(0)
            , crc_(0)
        {
        }

        virtual ~Entry()
        {
        }

        std::string getName() const
        {
          return name_;
        }

        std::uint64_t getIndex() const
        {
          return index_;
        }

        time_t getDate() const
        {
          return time_;
        }

        int getMethod() const
        {
          return method_;
        }

        std::uint64_t getSize() const
        {
          return size_;
        }

        std::uint64_t getInflatedSize() const
        {
          return sizeComp_;
        }

        int getCRC() const
        {
          return crc_;
        }

        bool isDirectory() const
        {
          return ZipArchive::isDirectory(name_);
        }

        bool isFile() const
        {
          return !isDirectory();
        }

        bool isNull() const
        {
          return zipFile_ == 0;
        }

        std::string getComment() const;

        bool setComment(const std::string &str) const;

        std::string readAsText(ZipArchive::State state = ZipArchive::Current) const;

        void *readAsBinary(ZipArchive::State state = ZipArchive::Current) const;

        template <typename T>
        T *readAs(ZipArchive::State state = ZipArchive::Current) const
        {
          return reinterpret_cast<T *>(readAsBinary(state));
        }

      private:
        Entry(const ZipArchive *zipFile,
              std::string const &name,
              std::uint64_t index,
              time_t time,
              int method,
              std::uint64_t size,
              std::uint64_t sizeComp,
              int crc)
            : zipFile_(zipFile)
            , name_(name)
            , index_(index)
            , time_(time)
            , method_(method)
            , size_(size)
            , sizeComp_(sizeComp)
            , crc_(crc)
        {
        }

        const ZipArchive *zipFile_;
        std::string name_;
        std::uint64_t index_;
        time_t time_;
        int method_;
        std::uint64_t size_;
        std::uint64_t sizeComp_;
        int crc_;
      };

      typedef std::vector<Entry> Entries;

      ZipArchive(std::string const &zipPath, std::string const &password = "");

      virtual ~ZipArchive();

      std::string getPath() const
      {
        return path_;
      }

      bool open(OpenMode mode_ = ReadOnly, bool checkConsistency = false);

      void close();

      void discard();

      bool unlink();

      bool isOpen() const
      {
        return zip_handle_ != 0;
      }

      bool isMutable() const
      {
        return isOpen() && mode_ != NotOpen && mode_ != ReadOnly;
      }

      bool isEncrypted() const
      {
        return !password_.empty();
      }

      std::string getComment(State state = Current) const;

      bool setComment(std::string const &comment) const;

      bool removeComment() const
      {
        return setComment(std::string());
      }

      std::int64_t getNbEntries(State state = Current) const;

      std::vector<Entry> getEntries(State state = Current) const;

      bool hasEntry(std::string const &name, bool excludeDirectories = false, bool caseSensitive = true, State state = Current) const;

      Entry getEntry(std::string const &name, bool excludeDirectories = false, bool caseSensitive = true, State state = Current) const;

      Entry getEntry(std::int64_t index, State state = Current) const;

      std::string getEntryComment(const Entry &entry, State state = Current) const;

      bool setEntryComment(const Entry &entry, std::string const &comment) const;

      void *readEntry(const Entry &zipEntry, bool asText = false, State state = Current) const;

      void *readEntry(std::string const &zipEntry, bool asText = false, State state = Current) const;

      int deleteEntry(const Entry &entry) const;

      int deleteEntry(std::string const &entry) const;

      int renameEntry(const Entry &entry, std::string const &newName) const;

      int renameEntry(std::string const &entry, std::string const &newName) const;

      bool addFile(std::string const &entryName, std::string const &file) const;

      bool addData(std::string const &entryName, std::vector<char> const &data, bool freeData = false) const;

      bool addData(std::string const &entryName, const void *data, std::uint32_t length, bool freeData = false) const;

      bool addEntry(const std::string &entryName) const;

      OpenMode getMode() const
      {
        return mode_;
      }

      static bool isDirectory(std::string const &str);

    private:
      Entry createEntry(struct zip_stat *stat) const;

      ZipArchive(const ZipArchive &zf);

      ZipArchive &operator=(const ZipArchive &);

      std::string path_;
      zip *zip_handle_;
      OpenMode mode_;
      std::string password_;
    };
  }
}

#endif // ZIP_EOS_CONFIGURATION_ARCHIVE_H
