#include <zip.h>
#include <errno.h>
#include <fstream>

#include "ziparchive.hpp"

namespace eos
{
  namespace eosconfig
  {
    const char DIRECTORY_SEPARATOR = '/';

    ZipArchive::ZipArchive(std::string const &zipPath, std::string const &password)
        : path_(zipPath)
        , zip_handle_(NULL)
        , mode_(NotOpen)
        , password_(password)
    {
    }

    ZipArchive::~ZipArchive()
    {
      close();
    }

    bool ZipArchive::open(OpenMode om, bool checkConsistency)
    {
      int zipFlag = 0;
      if (om == ReadOnly)
      {
        zipFlag = 0;
      }
      else if (om == Write)
      {
        zipFlag = ZIP_CREATE;
      }
      else if (om == New)
      {
        zipFlag = ZIP_CREATE | ZIP_TRUNCATE;
      }
      else
      {
        return false;
      }

      if (checkConsistency)
      {
        zipFlag = zipFlag | ZIP_CHECKCONS;
      }

      int errorFlag = 0;
      zip_handle_ = zip_open(path_.c_str(), zipFlag, &errorFlag);

      if (errorFlag != ZIP_ER_OK)
      {
        zip_handle_ = NULL;
        return false;
      }

      if (zip_handle_ != 0)
      {
        if (isEncrypted())
        {
          if (zip_set_default_password(zip_handle_, password_.c_str()) != 0)
          {
            close();
            return false;
          }
        }

        mode_ = om;
        return true;
      }

      return false;
    }

    void ZipArchive::close()
    {
      if (zip_handle_)
      {
        zip_close(zip_handle_);
        zip_handle_ = 0;
        mode_ = NotOpen;
      }
    }

    void ZipArchive::discard()
    {
      if (zip_handle_)
      {
        zip_discard(zip_handle_);
        zip_handle_ = 0;
        mode_ = NotOpen;
      }
    }

    bool ZipArchive::unlink()
    {
      if (isOpen())
      {
        discard();
      }
      return !remove(path_.c_str());
    }

    std::string ZipArchive::getComment(State state) const
    {
      if (!isOpen())
      {
        return std::string();
      }

      int flag = ZIP_FL_ENC_GUESS;
      if (state == Original)
      {
        flag = flag | ZIP_FL_UNCHANGED;
      }

      int length = 0;
      const char *comment = zip_get_archive_comment(zip_handle_, &length, flag);
      if (comment == 0)
      {
        return std::string();
      }
      return std::string(comment, length);
    }

    bool ZipArchive::setComment(std::string const &comment) const
    {
      if (!isOpen())
      {
        return false;
      }

      int size = comment.size();
      const char *data = comment.c_str();
      int result = zip_set_archive_comment(zip_handle_, data, size);
      return result == 0;
    }

    std::int64_t ZipArchive::getNbEntries(State state) const
    {
      if (!isOpen())
        return -1;

      int flag = state == Original ? ZIP_FL_UNCHANGED : 0;
      return zip_get_num_entries(zip_handle_, flag);
    }

    ZipArchive::Entry ZipArchive::createEntry(struct zip_stat *stat) const
    {
      std::string name(stat->name);
      std::uint64_t index = stat->index;
      std::uint64_t size = stat->size;
      int method = stat->comp_method;
      std::uint64_t sizeComp = stat->comp_size;
      int crc = stat->crc;
      time_t time = stat->mtime;

      return Entry(this, name, index, time, method, size, sizeComp, crc);
    }

    std::vector<ZipArchive::Entry> ZipArchive::getEntries(State state) const
    {
      if (!isOpen())
      {
        return std::vector<Entry>();
      }

      struct zip_stat stat;
      zip_stat_init(&stat);

      std::vector<Entry> entries;
      int flag = state == Original ? ZIP_FL_UNCHANGED : 0;
      std::int64_t nbEntries = getNbEntries(state);
      for (std::int64_t i = 0; i < nbEntries; ++i)
      {
        int result = zip_stat_index(zip_handle_, i, flag, &stat);
        if (result == 0)
        {
          Entry entry = createEntry(&stat);
          entries.push_back(entry);
        }
        else
        {
          //TODO handle read error => crash ?
        }
      }
      return entries;
    }

    bool ZipArchive::hasEntry(std::string const &name,
                              bool excludeDirectories,
                              bool caseSensitive,
                              State state) const
    {
      if (!isOpen())
        return false;

      int flags = ZIP_FL_ENC_GUESS;
      if (excludeDirectories)
      {
        flags = flags | ZIP_FL_NODIR;
      }
      if (!caseSensitive)
      {
        flags = flags | ZIP_FL_NOCASE;
      }
      if (state == Original)
      {
        flags = flags | ZIP_FL_UNCHANGED;
      }

      std::int64_t index = zip_name_locate(zip_handle_, name.c_str(), flags);
      return index >= 0;
    }

    ZipArchive::Entry ZipArchive::getEntry(std::string const &name,
                                           bool excludeDirectories,
                                           bool caseSensitive,
                                           State state) const
    {
      if (isOpen())
      {
        int flags = ZIP_FL_ENC_GUESS;
        if (excludeDirectories)
        {
          flags = flags | ZIP_FL_NODIR;
        }
        if (!caseSensitive)
        {
          flags = flags | ZIP_FL_NOCASE;
        }
        if (state == Original)
        {
          flags = flags | ZIP_FL_UNCHANGED;
        }

        std::int64_t index = zip_name_locate(zip_handle_, name.c_str(), flags);
        if (index >= 0)
        {
          return getEntry(index);
        }
        else
        {
          //name not found
        }
      }
      return Entry();
    }

    ZipArchive::Entry ZipArchive::getEntry(std::int64_t index, State state) const
    {
      if (isOpen())
      {
        struct zip_stat stat;
        zip_stat_init(&stat);
        int flag = state == Original ? ZIP_FL_UNCHANGED : 0;
        int result = zip_stat_index(zip_handle_, index, flag, &stat);
        if (result == 0)
        {
          return createEntry(&stat);
        }
        else
        {
          //index not found / invalid index
        }
      }
      return Entry();
    }

    std::string ZipArchive::getEntryComment(const Entry &entry, State state) const
    {
      if (!isOpen())
        return "";
      if (entry.zipFile_ != this)
        return "";

      int flag = ZIP_FL_ENC_GUESS;
      if (state == Original)
      {
        flag = flag | ZIP_FL_UNCHANGED;
      }

      std::uint32_t clen;
      const char *com = zip_file_get_comment(zip_handle_, entry.getIndex(), &clen, flag);
      std::string comment = com == NULL ? std::string() : std::string(com, clen);
      return comment;
    }

    bool ZipArchive::setEntryComment(const Entry &entry, std::string const &comment) const
    {
      if (!isOpen())
        return false;
      if (entry.zipFile_ != this)
        return false;

      return !zip_file_set_comment(zip_handle_, entry.getIndex(), comment.c_str(), comment.size(), ZIP_FL_ENC_GUESS);
    }

    void *ZipArchive::readEntry(const Entry &zipEntry, bool asText, State state) const
    {
      if (!isOpen())
      {
        return NULL;
      }
      if (zipEntry.zipFile_ != this)
      {
        return NULL;
      }

      int flag = state == Original ? ZIP_FL_UNCHANGED : 0;
      struct zip_file *zipFile = zip_fopen_index(zip_handle_, zipEntry.getIndex(), flag);
      if (zipFile)
      {
        std::uint64_t size = zipEntry.getSize();
        std::int64_t isize = (std::int64_t)size; //there will be a warning here, but unavoidable...

        char *data = new char[isize + (asText ? 1 : 0)];
        std::int64_t result = zip_fread(zipFile, data, size);
        zip_fclose(zipFile);

        //avoid buffer copy
        if (asText)
        {
          data[isize] = '\0';
        }

        if (result == isize)
        {
          return data;
        }
        else
        { //unexpected number of bytes read => crash ?
          delete[] data;
        }
      }
      else
      {
        //unable to read the entry => crash ?
      }

      return NULL;
    }

    void *ZipArchive::readEntry(std::string const &zipEntry, bool asText, State state) const
    {
      Entry entry = getEntry(zipEntry);
      if (entry.isNull())
      {
        return NULL;
      }
      return readEntry(entry, asText, state);
    }

    int ZipArchive::deleteEntry(const Entry &entry) const
    {
      if (!isOpen())
      {
        return -3;
      }
      if (entry.zipFile_ != this)
      {
        return -3;
      }
      if (mode_ == ReadOnly)
      {
        return -1;
      } //deletion not allowed

      if (entry.isFile())
      {
        int result = zip_delete(zip_handle_, entry.getIndex());
        if (result == 0)
        {
          return 1;
        }
        return -2; //unable to delete the entry
      }
      else
      {
        int counter = 0;
        std::vector<Entry> allEntries = getEntries();
        std::vector<Entry>::const_iterator eit;
        for (eit = allEntries.begin(); eit != allEntries.end(); ++eit)
        {
          Entry ze = *eit;
          int startPosition = ze.getName().find(entry.getName());
          if (startPosition == 0)
          {
            int result = zip_delete(zip_handle_, ze.getIndex());
            if (result == 0)
            {
              ++counter;
            }
            else
            {
              return -2;
            } //unable to remove the current entry
          }
        }
        return counter;
      }
    }

    int ZipArchive::deleteEntry(std::string const &e) const
    {
      Entry entry = getEntry(e);
      if (entry.isNull())
      {
        return -4;
      }
      return deleteEntry(entry);
    }

    int ZipArchive::renameEntry(const Entry &entry, std::string const &newName) const
    {
      if (!isOpen())
      {
        return -3;
      }
      if (entry.zipFile_ != this)
      {
        return -3;
      }
      if (mode_ == ReadOnly)
      {
        return -1;
      } //renaming not allowed
      if (newName.length() == 0)
      {
        return 0;
      }
      if (newName == entry.getName())
      {
        return 0;
      }

      if (entry.isFile())
      {
        if (isDirectory(newName))
        {
          return 0;
        } //invalid new name

        int lastSlash = newName.rfind(DIRECTORY_SEPARATOR);
        if (lastSlash != 1)
        {
          bool dadded = addEntry(newName.substr(0, lastSlash + 1));
          if (!dadded)
          {
            return 0;
          } //the hierarchy hasn't been created
        }

        int result = zip_file_rename(zip_handle_, entry.getIndex(), newName.c_str(), ZIP_FL_ENC_GUESS);
        if (result == 0)
        {
          return 1;
        }
        return 0; //renaming was not possible (entry already exists ?)
      }
      else
      {
        if (!isDirectory(newName))
        {
          return 0;
        } //invalid new name

        int parentSlash = newName.rfind(DIRECTORY_SEPARATOR, newName.length() - 2);
        if (parentSlash != -1)
        { //updates the dir hierarchy
          std::string parent = newName.substr(0, parentSlash + 1);
          bool dadded = addEntry(parent);
          if (!dadded)
          {
            return 0;
          }
        }

        int counter = 0;
        std::string originalName = entry.getName();
        std::vector<Entry> allEntries = getEntries();
        std::vector<Entry>::const_iterator eit;
        for (eit = allEntries.begin(); eit != allEntries.end(); ++eit)
        {
          Entry ze = *eit;
          std::string currentName = ze.getName();

          int startPosition = currentName.find(originalName);
          if (startPosition == 0)
          {
            if (currentName == originalName)
            {
              int result = zip_file_rename(zip_handle_, entry.getIndex(), newName.c_str(), ZIP_FL_ENC_GUESS);
              if (result == 0)
              {
                ++counter;
              }
              else
              {
                return -2;
              } //unable to rename the folder
            }
            else
            {
              std::string targetName = currentName.replace(0, originalName.length(), newName);
              int result = zip_file_rename(zip_handle_, ze.getIndex(), targetName.c_str(), ZIP_FL_ENC_GUESS);
              if (result == 0)
              {
                ++counter;
              }
              else
              {
                return -2;
              } //unable to rename a sub-entry
            }
          }
          else
          {
            //file not affected by the renaming
          }
        }

        /*
         * Special case for moving a directory a/x to a/x/y to avoid to lose
         * the a/x path in the archive.
         */
        bool newNameIsInsideCurrent = (newName.find(entry.getName()) == 0);
        if (newNameIsInsideCurrent)
        {
          bool dadded = addEntry(newName);
          if (!dadded)
          {
            return 0;
          }
        }

        return counter;
      }
    }

    int ZipArchive::renameEntry(std::string const &e, std::string const &newName) const
    {
      Entry entry = getEntry(e);
      if (entry.isNull())
      {
        return -4;
      }
      return renameEntry(entry, newName);
    }

    bool ZipArchive::addFile(std::string const &entryName, std::string const &file) const
    {
      if (!isOpen())
      {
        return false;
      }
      if (mode_ == ReadOnly)
      {
        return false;
      } //adding not allowed
      if (isDirectory(entryName))
      {
        return false;
      }

      int lastSlash = entryName.rfind(DIRECTORY_SEPARATOR);
      if (lastSlash != -1)
      { //creates the needed parent directories
        std::string dirEntry = entryName.substr(0, lastSlash + 1);
        bool dadded = addEntry(dirEntry);
        if (!dadded)
        {
          return false;
        }
      }

      //retrieves the length of the file
      //http://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c
      const char *filepath = file.c_str();
      std::ifstream in(filepath, std::ifstream::in | std::ifstream::binary);
      in.seekg(0, std::ifstream::end);
      std::streampos end = in.tellg();

      zip_source *source = zip_source_file(zip_handle_, filepath, 0, end);
      if (source != NULL)
      {
        std::int64_t result = zip_file_add(zip_handle_, entryName.c_str(), source, ZIP_FL_OVERWRITE);

        if (result >= 0)
        {
          return true;
        }
        else
        {
          zip_source_free(source);
        } //unable to add the file
      }
      else
      {
        //unable to create the zip_source
      }
      return false;
    }

    bool ZipArchive::addData(std::string const &entryName, std::vector<char> const &data, bool freeData) const
    {
      return addData(entryName, &data[0], data.size(), freeData);
    }

    bool ZipArchive::addData(std::string const &entryName, const void *data, std::uint32_t length, bool freeData) const
    {
      if (!isOpen())
      {
        return false;
      }
      if (mode_ == ReadOnly)
      {
        return false;
      } //adding not allowed
      if (isDirectory(entryName))
      {
        return false;
      }

      int lastSlash = entryName.rfind(DIRECTORY_SEPARATOR);
      if (lastSlash != -1)
      { //creates the needed parent directories
        std::string dirEntry = entryName.substr(0, lastSlash + 1);
        bool dadded = addEntry(dirEntry);
        if (!dadded)
        {
          return false;
        }
      }

      zip_source *source = zip_source_buffer(zip_handle_, data, length, freeData);
      if (source != NULL)
      {
        std::int64_t result = zip_file_add(zip_handle_, entryName.c_str(), source, ZIP_FL_OVERWRITE);
        if (result >= 0)
        {
          return true;
        }
        else
        {
          zip_source_free(source);
        } //unable to add the file
      }
      else
      {
        //unable to create the zip_source
      }
      return false;
    }

    bool ZipArchive::addEntry(std::string const &entryName) const
    {
      if (!isOpen())
      {
        return false;
      }
      if (mode_ == ReadOnly)
      {
        return false;
      } //adding not allowed
      if (!isDirectory(entryName))
      {
        return false;
      }

      int nextSlash = entryName.find(DIRECTORY_SEPARATOR);
      while (nextSlash != -1)
      {
        std::string pathToCreate = entryName.substr(0, nextSlash + 1);
        if (!hasEntry(pathToCreate))
        {
          std::int64_t result = zip_dir_add(zip_handle_, pathToCreate.c_str(), ZIP_FL_ENC_GUESS);
          if (result == -1)
          {
            return false;
          }
        }
        nextSlash = entryName.find(DIRECTORY_SEPARATOR, nextSlash + 1);
      }

      return true;
    }

    bool ZipArchive::isDirectory(std::string const &str)
    {
      return str.length() && str[str.length() - 1] == DIRECTORY_SEPARATOR;
    }

    std::string ZipArchive::Entry::getComment() const
    {
      return zipFile_->getEntryComment(*this);
    }

    bool ZipArchive::Entry::setComment(const std::string &str) const
    {
      return zipFile_->setEntryComment(*this, str);
    }

    std::string ZipArchive::Entry::readAsText(ZipArchive::State state) const
    {
      char *content = (char *)zipFile_->readEntry(*this, true, state);
      if (content == NULL)
      {
        return std::string();
      } //happen if the ZipArchive has been closed

      std::string str(content, size_);
      delete[] content;
      return str;
    }

    void *ZipArchive::Entry::readAsBinary(ZipArchive::State state) const
    {
      return zipFile_->readEntry(*this, false, state);
    }
  }
}
