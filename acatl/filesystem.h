//
//  filesystem.h
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2019, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef acatl_filesystem_hpp
#define acatl_filesystem_hpp

#include <acatl/inc.h>

#include <acatl/exception.h>
#include <acatl/types.h>

#include <iomanip>
#include <memory>
#include <type_traits>

namespace acatl
{
  /// Implements most of the c++17 filesystem std library types. Currently only supports Unix operating systems.
  namespace filesystem
  {
    // 27.10.10, enumerations
    enum class file_type
    {
      none = 0,
      not_found = -1,
      regular = 1,
      directory = 2,
      symlink = 3,
      block = 4,
      character = 5,
      fifo = 6,
      socket = 7,
      unknown = 8
    };

    enum class perms : uint32_t
    {
      none = 0,
      owner_read = 0400,
      owner_write = 0200,
      owner_exec = 0100,
      owner_all = 0700,
      group_read = 040,
      group_write = 020,
      group_exec = 010,
      group_all = 070,
      others_read = 04,
      others_write = 02,
      others_exec = 01,
      others_all = 07,
      all = 0777,
      set_uid = 04000,
      set_gid = 02000,
      sticky_bit = 01000,
      mask = 07777,
      unknown = 0xFFFF,
      add_perms = 0x10000,
      remove_perms = 0x20000,
      symlink_nofollow = 0x40000
    };

    perms operator|(const perms prms1, const perms prms2);
    perms operator&(const perms prms1, const perms prms2);
    perms operator^(const perms prms1, const perms prms2);
    perms operator~(const perms prms);
    perms& operator|=(perms& prms1, const perms prms2);
    perms& operator&=(perms& prms1, const perms prms2);
    perms& operator^=(perms& prms1, const perms prms2);

    enum class directory_options : uint8_t
    {
      none = 0,
      follow_directory_symlink = 1,
      skip_permission_denied = 2
    };

    directory_options operator|(const directory_options opt1, const directory_options opt2);
    directory_options operator&(const directory_options opt1, const directory_options opt2);
    directory_options operator^(const directory_options opt1, const directory_options opt2);
    directory_options operator~(const directory_options opt);
    directory_options& operator|=(directory_options& opt1, const directory_options opt2);
    directory_options& operator&=(directory_options& opt1, const directory_options opt2);
    directory_options& operator^=(directory_options& opt1, const directory_options opt2);

    // 27.10.11, file status
    class file_status
    {
    public:
      // constructors
      explicit file_status(file_type ft = file_type::none, perms prms = perms::unknown) noexcept;
      file_status(const file_status&) noexcept = default;
      file_status(file_status&&) noexcept = default;
      ~file_status();

      file_status& operator=(const file_status&) noexcept = default;
      file_status& operator=(file_status&&) noexcept = default;

      // observers
      file_type type() const noexcept;
      perms permissions() const noexcept;

      // modifiers
      void type(file_type ft) noexcept;
      void permissions(perms prms) noexcept;

    private:
      file_type _ft;
      perms _prms;
    };

    bool operator==(const file_status& lhs, const file_status& rhs) noexcept;

    namespace path_traits
    {
      template<class T>
      struct is_pathable
      {
        static const bool value = false;
      };

      template<>
      struct is_pathable<char*>
      {
        static const bool value = true;
      };
      template<>
      struct is_pathable<const char*>
      {
        static const bool value = true;
      };
      template<>
      struct is_pathable<std::string>
      {
        static const bool value = true;
      };
    }

    // 27.10.8, paths
    class path
    {
    public:
      typedef char value_type;
      typedef std::basic_string<value_type> string_type;
      class iterator;
      typedef iterator const_iterator;
      static constexpr value_type _preferred_separator = '/';
      static constexpr value_type _separator = '/';
      static constexpr value_type _dot = '.';

      path() noexcept;
      path(const path& p);
      path(path&& p) noexcept;

      template<class Source>
      path(Source const& source,
           typename std::enable_if<path_traits::is_pathable<typename std::decay<Source>::type>::value>::type* = 0)
      : _path(source)
      {
      }

      path& operator=(const path& p);
      path& operator=(path&& p) noexcept;

      template<class Source,
               typename std::enable_if<path_traits::is_pathable<typename std::decay<Source>::type>::value>::type = 0>
      path& operator=(const Source& source)
      {
        _path = source;
        return *this;
      }
      template<class Source>
      path&
        assign(const Source& source,
               typename std::enable_if<path_traits::is_pathable<typename std::decay<Source>::type>::value>::type* = 0)
      {
        _path = source;
        return *this;
      }

      path& operator/=(const path& p);

      template<class Source,
               typename std::enable_if<path_traits::is_pathable<typename std::decay<Source>::type>::value>::type = 0>
      path& operator/=(const Source& source)
      {
        this->operator/=(path(source));
        return *this;
      }

      template<class Source>
      path&
        append(const Source& source,
               typename std::enable_if<path_traits::is_pathable<typename std::decay<Source>::type>::value>::type* = 0)
      {
        this->operator/=(path(source));
        return *this;
      }

      path& operator+=(const path& p);
      path& operator+=(const string_type& str);
      path& operator+=(const value_type* ptr);
      path& operator+=(value_type x);

      template<class Source>
      path&
        concat(const Source& source,
               typename std::enable_if<path_traits::is_pathable<typename std::decay<Source>::type>::value>::type* = 0)
      {
        this->operator+=(path(source));
        return *this;
      }

      void clear() noexcept;
      path& make_preferred();
      path& remove_filename();
      path& replace_filename(const path& replacement);
      path& replace_extension(const path& replacement = path());

      void swap(path& other) noexcept;

      const value_type* c_str() const noexcept;
      const string_type& native() const noexcept;
      operator string_type() const;

      std::string string() const;
      std::string generic_string() const;

      int compare(const path& p) const noexcept;
      int compare(const string_type& str) const;
      int compare(const value_type* s) const;

      path root_name() const;
      path root_directory() const;
      path root_path() const;
      path relative_path() const;
      path parent_path() const;
      path filename() const;
      path stem() const;
      path extension() const;

      bool empty() const noexcept;

      bool has_root_path() const;
      bool has_root_name() const;
      bool has_root_directory() const;
      bool has_relative_path() const;
      bool has_parent_path() const;
      bool has_filename() const;
      bool has_stem() const;
      bool has_extension() const;

      bool is_absolute() const;
      bool is_relative() const;

      iterator begin() const;
      iterator end() const;

    private:
      friend class iterator;

      std::string _path;
    };

    // 27.10.9, filesystem errors
    class filesystem_error : public std::system_error
    {
    public:
      filesystem_error(const std::string& what_arg, std::error_code ec);
      filesystem_error(const std::string& what_arg, const path& p1, std::error_code ec);
      filesystem_error(const std::string& what_arg, const path& p1, const path& p2, std::error_code ec);

      const path& path1() const noexcept
      {
        return _path1;
      }

      const path& path2() const noexcept
      {
        return _path2;
      }

      const char* what() const noexcept;

    private:
      const path _path1;
      const path _path2;
    };

    void PrintTo(const path& p, std::ostream* stream);

    // 27.10.8.6, path non-member functions
    void swap(path& lhs, path& rhs) noexcept;
    size_t hash_value(const path& p) noexcept;

    bool operator==(const path& lhs, const path& rhs) noexcept;
    bool operator!=(const path& lhs, const path& rhs) noexcept;
    bool operator<(const path& lhs, const path& rhs) noexcept;
    bool operator<=(const path& lhs, const path& rhs) noexcept;
    bool operator>(const path& lhs, const path& rhs) noexcept;
    bool operator>=(const path& lhs, const path& rhs) noexcept;

    path operator/(const path& lhs, const path& rhs);

    // 27.10.8.6.1, path inserter and extractor
    template<class CharT, class Traits>
    std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const path& p)
    {
      os << std::quoted(p.string());
      return os;
    }

    template<class CharT, class Traits>
    std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, path& p)
    {
      std::basic_string<CharT, Traits> t;
      is >> std::quoted(t);
      p = t;
      return is;
    }

    // 27.10.13, directory iterators
    class path::iterator : public std::iterator<std::bidirectional_iterator_tag, const path>
    {
    public:
      iterator();

      bool operator==(const iterator& other) const;
      bool operator!=(const iterator& other) const;

      reference operator*() const;
      pointer operator->() const;

      iterator& operator--();
      iterator operator--(int);

      iterator& operator++();
      iterator operator++(int);

    private:
      friend class path;

      typedef std::remove_cv<value_type>::type non_const_value_type;

      non_const_value_type _element;
      value_type* _path;
      string_type::size_type _pos;
    };

    // 27.10.15, filesystem operations
    path current_path();
    path current_path(std::error_code& ec) noexcept;
    void current_path(const path& p);
    void current_path(const path& p, std::error_code& ec) noexcept;

    path absolute(const path& p, const path& base = current_path());

    path canonical(const path& p, const path& base = current_path());
    path canonical(const path& p, std::error_code& ec);
    path canonical(const path& p, const path& base, std::error_code& ec);

    bool create_directories(const path& p);
    bool create_directories(const path& p, std::error_code& ec) noexcept;
    bool create_directory(const path& p);
    bool create_directory(const path& p, std::error_code& ec) noexcept;
    bool create_directory(const path& p, const path& existing_p);
    bool create_directory(const path& p, const path& existing_p, std::error_code& ec) noexcept;

    bool equivalent(const path& p1, const path& p2);
    bool equivalent(const path& p1, const path& p2, std::error_code& ec) noexcept;

    void create_symlink(const path& to, const path& new_symlink);
    void create_symlink(const path& to, const path& new_symlink, std::error_code& ec) noexcept;

    bool exists(file_status s) noexcept;
    bool exists(const path& p);
    bool exists(const path& p, std::error_code& ec) noexcept;

    uintmax_t file_size(const path& p);
    uintmax_t file_size(const path& p, std::error_code& ec) noexcept;

    bool is_block_file(file_status s) noexcept;
    bool is_block_file(const path& p);
    bool is_block_file(const path& p, std::error_code& ec) noexcept;

    bool is_character_file(file_status s) noexcept;
    bool is_character_file(const path& p);
    bool is_character_file(const path& p, std::error_code& ec) noexcept;

    bool is_directory(file_status s) noexcept;
    bool is_directory(const path& p);
    bool is_directory(const path& p, std::error_code& ec) noexcept;

    bool is_empty(const path& p);
    bool is_empty(const path& p, std::error_code& ec) noexcept;

    bool is_fifo(file_status s) noexcept;
    bool is_fifo(const path& p);
    bool is_fifo(const path& p, std::error_code& ec) noexcept;

    bool is_other(file_status s) noexcept;
    bool is_other(const path& p);
    bool is_other(const path& p, std::error_code& ec) noexcept;

    bool is_regular_file(file_status s) noexcept;
    bool is_regular_file(const path& p);
    bool is_regular_file(const path& p, std::error_code& ec) noexcept;

    bool is_socket(file_status s) noexcept;
    bool is_socket(const path& p);
    bool is_socket(const path& p, std::error_code& ec) noexcept;

    bool is_symlink(file_status s) noexcept;
    bool is_symlink(const path& p);
    bool is_symlink(const path& p, std::error_code& ec) noexcept;

    void permissions(const path& p, perms prms);
    void permissions(const path& p, perms prms, std::error_code& ec);

    path read_symlink(const path& p);
    path read_symlink(const path& p, std::error_code& ec);

    bool remove(const path& p);
    bool remove(const path& p, std::error_code& ec);
    std::uintmax_t remove_all(const path& p);
    std::uintmax_t remove_all(const path& p, std::error_code& ec);

    file_status status(const path& p);
    file_status status(const path& p, std::error_code& ec) noexcept;

    bool status_known(file_status s) noexcept;

    file_status symlink_status(const path& p);
    file_status symlink_status(const path& p, std::error_code& ec) noexcept;

    path system_complete(const path& p);
    path system_complete(const path& p, std::error_code& ec);

    path temp_directory_path();
    path temp_directory_path(std::error_code& ec);

    // 27.10.12, directory entries
    class directory_entry
    {
    public:
      // constructors and destructor
      directory_entry() noexcept = default;
      directory_entry(const directory_entry&) = default;
      directory_entry(directory_entry&&) noexcept = default;
      explicit directory_entry(const path& p);
      ~directory_entry();

      // modifiers
      directory_entry& operator=(const directory_entry&) = default;
      directory_entry& operator=(directory_entry&&) noexcept = default;
      void assign(const path& p);
      void replace_filename(const path& p);

      // observers
      operator const class path&() const noexcept;
      const class path& path() const noexcept;
      file_status status() const;
      file_status status(std::error_code& ec) const noexcept;
      file_status symlink_status() const;
      file_status symlink_status(std::error_code& ec) const noexcept;

      bool operator<(const directory_entry& rhs) const noexcept;
      bool operator==(const directory_entry& rhs) const noexcept;
      bool operator!=(const directory_entry& rhs) const noexcept;
      bool operator<=(const directory_entry& rhs) const noexcept;
      bool operator>(const directory_entry& rhs) const noexcept;
      bool operator>=(const directory_entry& rhs) const noexcept;

    private:
      class path _path;
    };

    // 27.10.13, directory iterators
    class directory_iterator : public std::iterator<std::input_iterator_tag, const directory_entry>
    {
    public:
      // 27.10.13.1, member functions
      directory_iterator() noexcept;
      explicit directory_iterator(const path& p);
      directory_iterator(const path& p, directory_options options);
      directory_iterator(const path& p, std::error_code& ec) noexcept;
      directory_iterator(const path& p, directory_options options, std::error_code& ec) noexcept;
      directory_iterator(const directory_iterator& rhs);
      directory_iterator(directory_iterator&& rhs) noexcept;
      ~directory_iterator();

      iterator& operator=(const directory_iterator& rhs);
      iterator& operator=(directory_iterator&& rhs) noexcept;

      reference operator*() const;
      pointer operator->() const;
      iterator& operator++();
      iterator& increment(std::error_code& ec) noexcept;

      // other members as required by 24.2.3, input iterators
      bool operator==(const directory_iterator& other) const;
      bool operator!=(const directory_iterator& other) const;

    private:
      struct impl;
      std::shared_ptr<impl> _impl;
    };

    // 27.10.13.2, range access for directory iterators
    directory_iterator begin(directory_iterator iter) noexcept;
    directory_iterator end(const directory_iterator&) noexcept;

    // 27.10.14, recursive directory iterators
    class recursive_directory_iterator : public std::iterator<std::input_iterator_tag, const directory_entry>
    {
    public:
      // 27.10.14.1, constructors and destructor
      recursive_directory_iterator() noexcept;
      explicit recursive_directory_iterator(const path& p);
      recursive_directory_iterator(const path& p, directory_options options);
      recursive_directory_iterator(const path& p, directory_options options, std::error_code& ec) noexcept;
      recursive_directory_iterator(const path& p, std::error_code& ec) noexcept;
      recursive_directory_iterator(const recursive_directory_iterator& rhs);
      recursive_directory_iterator(recursive_directory_iterator&& rhs) noexcept;
      ~recursive_directory_iterator();

      // 27.10.14.1, observers
      directory_options options() const;
      int depth() const;
      bool recursion_pending() const;
      const directory_entry& operator*() const;
      const directory_entry* operator->() const;

      // 27.10.14.1, modifiers
      recursive_directory_iterator& operator=(const recursive_directory_iterator& rhs);
      recursive_directory_iterator& operator=(recursive_directory_iterator&& rhs) noexcept;
      recursive_directory_iterator& operator++();
      recursive_directory_iterator& increment(std::error_code& ec) noexcept;
      void pop();
      void pop(std::error_code& ec);
      void disable_recursion_pending();

      // other members as required by 24.2.3, input iterators
      bool operator==(const recursive_directory_iterator& other) const;
      bool operator!=(const recursive_directory_iterator& other) const;

    private:
      struct impl;
      std::shared_ptr<impl> _impl;
    };

    // 27.10.14.2, range access for recursive directory iterators
    recursive_directory_iterator begin(recursive_directory_iterator iter) noexcept;
    recursive_directory_iterator end(const recursive_directory_iterator&) noexcept;

    /// Creates a uniqely named temporary directory.
    path unique_temp_directory_path();

    /// Creates a uniquely named temporary directory.
    path unique_temp_directory_path(std::error_code& ec);

    /// Creates a temporary directory and removes it upon destruction.
    ACATL_EXPORT class TemporaryDirectoryGuard : public NonCopyable
    {
    public:
      /// Creates a temporary directory, that is valid until destruction of the guard.
      TemporaryDirectoryGuard();

      /// Returns the path of the newly created temporary directory.
      /// @return Path to the newly created temporary directory
      const path& temporaryDirectoryPath() const;

      /// Removes the temporary directory.
      ~TemporaryDirectoryGuard();

    private:
      path _path;
    };
  }
}

#endif
