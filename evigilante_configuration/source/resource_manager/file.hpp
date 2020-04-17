#ifndef EOS_CONFIGURATION_FILE_HPP
#define EOS_CONFIGURATION_FILE_HPP

#include <fstream>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string>

////////////////////////////////////////////////////////////
/// @brief some useful tools to handle with a file
////////////////////////////////////////////////////////////

namespace eos
{
  namespace file
  {
    ////////////////////////////////////////////////////////////
    /// @brief test the existence of a file
    ///
    /// @param filename path of the file
    ///
    /// @return true if the file exists, else false
    ////////////////////////////////////////////////////////////
    inline bool exist(std::string const &filename)
    {
      return std::ifstream(filename.c_str()).good();
    }

    /**
         * @brief remove a file
         * @param filename
         * @return
         */
    inline bool remove(std::string const &filename)
    {
      return ::remove(filename.c_str()) == 0;
    }

    ////////////////////////////////////////////////////////////
    /// @brief return the size of a file (in byte)
    ///
    /// @param filename path of the file
    ///
    /// @return size of the file if exist, else 0
    ////////////////////////////////////////////////////////////
    inline std::size_t size(std::string const &filename)
    {
      std::ifstream my_file(filename.c_str(), std::ios::binary | std::ios::ate);
      if (!my_file) return 0;
      return static_cast<std::size_t>(my_file.tellg());
    }

    namespace
    {
      ////////////////////////////////////////////////////////////
      /// @brief compare two iterators value
      ///
      /// @param first1 the beginning of values
      /// @param last1 an end iterator
      /// @param first1 the beginning of values (second)
      /// @param last2 an end iterator
      ///
      /// @return false if one value is different, else true
      ////////////////////////////////////////////////////////////
      template <typename InputIterator1, typename InputIterator2>
      inline bool range_equal(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2)
      {
        while (first1 != last1 && first2 != last2)
        {
          if (*first1 != *first2) return false;
          ++first1;
          ++first2;
        }
        return (first1 == last1) && (first2 == last2);
      }
    }

    ////////////////////////////////////////////////////////////
    /// @brief compare two files
    ///
    /// @param filename1 first file to compare
    /// @param filename2 second file to compare
    ///
    /// @return true if file are identic, else return false
    ////////////////////////////////////////////////////////////
    inline bool compare(std::string const &filename1, std::string const &filename2)
    {
      std::ifstream file1(filename1.c_str());
      std::ifstream file2(filename2.c_str());

      std::istreambuf_iterator<char> begin1(file1);
      std::istreambuf_iterator<char> begin2(file2);

      std::istreambuf_iterator<char> end;

      return range_equal(begin1, end, begin2, end);
    }

    inline void copy(std::string const &filename1, std::string const &filename2)
    {
      std::ifstream source(filename1.c_str());
      std::ofstream dest(filename2.c_str());

      dest << source.rdbuf();
    }

    ////////////////////////////////////////////////////////////
    /// @brief return the extension of a filename
    ///
    /// @param filename name of the file
    ///
    /// @return if a dot is found return string after it
    ///         in case of no dot found, return an empty string
    ///         in case of multiple dot return the string after the last dot
    ////////////////////////////////////////////////////////////
    inline std::string extension(std::string const &filename)
    {
      std::size_t lastPoint = filename.rfind('.');
      if (lastPoint == std::string::npos)
      {
        return "";
      }
      std::string data = filename.substr(filename.rfind('.') + 1);
      std::transform(data.begin(), data.end(), data.begin(), ::tolower);
      return data;
    }
  }
}

#endif // EOS_CONFIGURATION_FILE_HPP
