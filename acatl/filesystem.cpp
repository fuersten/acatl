//
//  filesystem.cpp
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

#include <acatl/filesystem.h>

#include <acatl/hash_helper.h>

#include <cstring>

namespace acatl
{
  namespace filesystem
  {
    file_status::file_status(file_type ft, perms prms) noexcept
    : _ft(ft)
    , _prms(prms)
    {
    }

    file_status::~file_status()
    {
    }

    file_type file_status::type() const noexcept
    {
      return _ft;
    }

    perms file_status::permissions() const noexcept
    {
      return _prms;
    }

    void file_status::type(file_type ft) noexcept
    {
      _ft = ft;
    }

    void file_status::permissions(perms prms) noexcept
    {
      _prms = prms;
    }

    bool operator==(const file_status& lhs, const file_status& rhs) noexcept
    {
      return lhs.type() == rhs.type() && lhs.permissions() == rhs.permissions();
    }

    perms operator|(const perms prms1, const perms prms2)
    {
      return perms(static_cast<std::underlying_type<perms>::type>(prms1)
                   | static_cast<std::underlying_type<perms>::type>(prms2));
    }

    perms operator&(const perms prms1, const perms prms2)
    {
      return perms(static_cast<std::underlying_type<perms>::type>(prms1)
                   & static_cast<std::underlying_type<perms>::type>(prms2));
    }

    perms operator^(const perms prms1, const perms prms2)
    {
      return perms(static_cast<std::underlying_type<perms>::type>(prms1)
                   ^ static_cast<std::underlying_type<perms>::type>(prms2));
    }

    perms operator~(const perms prms)
    {
      return perms(~static_cast<std::underlying_type<perms>::type>(prms));
    }

    perms& operator|=(perms& prms1, const perms prms2)
    {
      prms1 = perms(static_cast<std::underlying_type<perms>::type>(prms1)
                    | static_cast<std::underlying_type<perms>::type>(prms2));
      return prms1;
    }

    perms& operator&=(perms& prms1, const perms prms2)
    {
      prms1 = perms(static_cast<std::underlying_type<perms>::type>(prms1)
                    & static_cast<std::underlying_type<perms>::type>(prms2));
      return prms1;
    }

    perms& operator^=(perms& prms1, const perms prms2)
    {
      prms1 = perms(static_cast<std::underlying_type<perms>::type>(prms1)
                    ^ static_cast<std::underlying_type<perms>::type>(prms2));
      return prms1;
    }

    directory_options operator|(const directory_options opt1, const directory_options opt2)
    {
      return directory_options(static_cast<std::underlying_type<directory_options>::type>(opt1)
                               | static_cast<std::underlying_type<directory_options>::type>(opt2));
    }

    directory_options operator&(const directory_options opt1, const directory_options opt2)
    {
      return directory_options(static_cast<std::underlying_type<directory_options>::type>(opt1)
                               & static_cast<std::underlying_type<directory_options>::type>(opt2));
    }

    directory_options operator^(const directory_options opt1, const directory_options opt2)
    {
      return directory_options(static_cast<std::underlying_type<directory_options>::type>(opt1)
                               ^ static_cast<std::underlying_type<directory_options>::type>(opt2));
    }

    directory_options operator~(const directory_options opt)
    {
      return directory_options(~static_cast<std::underlying_type<directory_options>::type>(opt));
    }

    directory_options& operator|=(directory_options& opt1, const directory_options opt2)
    {
      opt1 = directory_options(static_cast<std::underlying_type<directory_options>::type>(opt1)
                               | static_cast<std::underlying_type<directory_options>::type>(opt2));
      return opt1;
    }

    directory_options& operator&=(directory_options& opt1, const directory_options opt2)
    {
      opt1 = directory_options(static_cast<std::underlying_type<directory_options>::type>(opt1)
                               & static_cast<std::underlying_type<directory_options>::type>(opt2));
      return opt1;
    }

    directory_options& operator^=(directory_options& opt1, const directory_options opt2)
    {
      opt1 = directory_options(static_cast<std::underlying_type<directory_options>::type>(opt1)
                               ^ static_cast<std::underlying_type<directory_options>::type>(opt2));
      return opt1;
    }

    path::path() noexcept
    {
    }

    path::path(const path& p)
    : _path(p._path)
    {
    }

    path::path(path&& p) noexcept
    : _path(std::move(p))
    {
    }

    path& path::operator=(const path& p)
    {
      _path = p._path;
      return *this;
    }

    path& path::operator=(path&& p) noexcept
    {
      _path = std::move(p._path);
      return *this;
    }

    path& path::operator/=(const path& p)
    {
      if(!(_path.empty() || p._path.front() == _separator || _path.back() == _separator || p.empty())) {
        _path += _separator;
      }
      _path += p._path;
      return *this;
    }

    path& path::operator+=(const path& p)
    {
      _path += p._path;
      return *this;
    }

    path& path::operator+=(const string_type& str)
    {
      _path += str;
      return *this;
    }

    path& path::operator+=(const value_type* ptr)
    {
      _path += ptr;
      return *this;
    }

    path& path::operator+=(value_type x)
    {
      _path += x;
      return *this;
    }

    void path::clear() noexcept
    {
      _path.clear();
    }

    path& path::make_preferred()
    {
      return *this;
    }

    path& path::remove_filename()
    {
      if(has_filename()) {
        *this = parent_path();
      }
      return *this;
    }

    path& path::replace_filename(const path& replacement)
    {
      remove_filename();
      operator/=(replacement);

      return *this;
    }

    path& path::replace_extension(const path& replacement)
    {
      if(has_extension()) {
        _path = _path.substr(0, _path.length() - extension()._path.length());
      }
      if(!replacement.empty()) {
        if(replacement._path.at(0) != _dot) {
          operator+=(string_type(1, _dot));
        }
        operator+=(replacement);
      }
      return *this;
    }

    void path::swap(path& other) noexcept
    {
      _path.swap(other._path);
    }

    const path::value_type* path::c_str() const noexcept
    {
      return _path.c_str();
    }

    const path::string_type& path::native() const noexcept
    {
      return _path;
    }

    path::operator path::string_type() const
    {
      return _path;
    }

    std::string path::string() const
    {
      return _path;
    }

    std::string path::generic_string() const
    {
      return _path;
    }

    int path::compare(const path& p) const noexcept
    {
      return ::strcoll(_path.c_str(), p._path.c_str());
    }

    int path::compare(const string_type& str) const
    {
      return compare(path(str));
    }

    int path::compare(const value_type* s) const
    {
      return compare(path(s));
    }

    path path::root_name() const
    {
      if(!empty()) {
        const char* s = _path.c_str();
        while(*s != '\0' && *s == _separator) {
          ++s;
        }
        if(std::distance(_path.c_str(), s) == 2) {
          // we have two slashes
          while(*s != '\0' && *s != _separator) {
            ++s;
          }
          return path{_path.substr(0, static_cast<size_t>(std::distance(_path.c_str(), s)))};
        }
      }
      return path{};
    }

    path path::root_directory() const
    {
      if(!_path.empty() && _path.at(0) == _separator) {
        path root{root_name()};
        if(root.empty() || _path.length() > root._path.length()) {
          return string_type(1, _separator);
        }
      }
      return path();
    }

    path path::root_path() const
    {
      return root_name() / root_directory();
    }

    path path::relative_path() const
    {
      if(empty()) {
        return path{};
      }
      if(_path.at(0) == _separator) {
        const char* s = _path.c_str();
        while(*s == _separator) {
          ++s;
        }
        ssize_t dist = std::distance(_path.c_str(), s);
        if(dist == 2) {
          while(*s != _separator && *s != '\0') {
            ++s;
          }
          if(*s == _separator) {
            ++s;
          }
          return path{s};
        } else {
          return path{s};
        }
      }
      return _path;
    }

    path path::parent_path() const
    {
      if(!empty()) {
        const char* begin = _path.c_str();
        const char* s = _path.c_str() + (_path.length() - 1);

        if(std::distance(begin, s) > 0) {
          while(*s != _separator && s != begin) {
            --s;
          }
          const char* sepStart = s;
          while(*s == _separator && s != begin) {
            --s;
          }
          ssize_t dist = std::distance(begin, s);
          if(dist > 0) {
            return path{_path.substr(0, static_cast<size_t>(dist) + 1)};
          }
          if(*s == _separator && std::distance(s, sepStart) == 0) {
            return path{string_type(1, _separator)};
          }
        }
      }
      return path{};
    }

    path path::filename() const
    {
      if(!empty()) {
        if(_path == string_type(1, _separator)) {
          return path{_path};
        }

        const char* begin = _path.c_str();
        const char* s = _path.c_str() + (_path.length() - 1);
        if(*s == _separator) {
          return path{string_type(1, _dot)};
        }
        while(*s != _separator && s != begin) {
          --s;
        }
        if(*s == _separator && std::distance(begin, s) == 1) {
          if(*(s - 1) == _separator) {
            return path{begin};
          }
        }
        if(*s != _separator && s == begin) {
          return path{begin};
        }
        return path{s + 1};
      }
      return path{};
    }

    path path::stem() const
    {
      path fn = filename();
      if(!fn.empty()) {
        const char* begin = fn._path.c_str();
        const char* s = fn._path.c_str() + (fn._path.length() - 1);
        ssize_t dist = std::distance(begin, s);
        if((dist == 0 && *s == _dot) || (dist == 1 && *s == _dot && *(s - 1) == _dot)) {
          return path{};
        }
        while(*s != _dot && s != begin) {
          --s;
        }
        if(*s == _dot) {
          return path{fn._path.substr(0, static_cast<size_t>(std::distance(begin, s)))};
        }
      }
      return path{};
    }

    path path::extension() const
    {
      path fn = filename();
      if(!fn.empty()) {
        const char* begin = fn._path.c_str();
        const char* s = fn._path.c_str() + (fn._path.length() - 1);
        ssize_t dist = std::distance(begin, s);
        if((dist == 0 && *s == _dot) || (dist == 1 && *s == _dot && *(s - 1) == _dot)) {
          return path{};
        }
        while(*s != _dot && s != begin) {
          --s;
        }
        if(*s == _dot) {
          return path{s};
        }
      }
      return path{};
    }

    bool path::empty() const noexcept
    {
      return _path.empty();
    }

    bool path::has_root_path() const
    {
      return !root_path().empty();
    }

    bool path::has_root_name() const
    {
      return !root_name().empty();
    }

    bool path::has_root_directory() const
    {
      return !root_directory().empty();
    }

    bool path::has_relative_path() const
    {
      return !relative_path().empty();
    }

    bool path::has_parent_path() const
    {
      return !parent_path().empty();
    }

    bool path::has_filename() const
    {
      return !filename().empty();
    }

    bool path::has_stem() const
    {
      return !stem().empty();
    }

    bool path::has_extension() const
    {
      return !extension().empty();
    }

    bool path::is_absolute() const
    {
      return has_root_directory();
    }

    bool path::is_relative() const
    {
      return !is_absolute();
    }

    path::iterator path::begin() const
    {
      path::iterator iter{};

      iter._path = this;

      if(!empty()) {
        iter._pos = 0;
        const char* s = _path.c_str();
        const char* begin = s;

        while(*s != '\0' && *s == _separator) {
          ++s;
        }

        ssize_t dist = std::distance(begin, s);
        if(dist == 2 && *s != '\0') {
          // we have exactly two slashes, this is a path with a root-name, just find the next slash
        } else if(dist == 1) {
          // we have one slash and are ready
          iter._element = string_type(1, _separator);
          iter._pos = 1;
          return iter;
        } else if(dist > 2) {
          // we have one slash and are ready
          iter._element = string_type(1, _separator);
          iter._pos = 1;
          return iter;
        }

        while(*s != '\0' && *s != _separator) {
          ++s;
        }
        dist = std::distance(begin, s);
        iter._element = _path.substr(iter._pos, static_cast<string_type::size_type>(dist));
        iter._pos = static_cast<string_type::size_type>(dist);
      }

      return iter;
    }

    path::iterator path::end() const
    {
      path::iterator iter;

      iter._path = this;
      // one after the end
      iter._pos = string_type::npos;

      return iter;
    }

    size_t hash_value(const path& p) noexcept
    {
      return std::hash<const char*>()(p.string().c_str());
    }

    bool operator==(const path& lhs, const path& rhs) noexcept
    {
      return lhs.compare(rhs) == 0;
    }

    bool operator!=(const path& lhs, const path& rhs) noexcept
    {
      return lhs.compare(rhs) != 0;
    }

    bool operator<(const path& lhs, const path& rhs) noexcept
    {
      return lhs.compare(rhs) < 0;
    }

    bool operator<=(const path& lhs, const path& rhs) noexcept
    {
      return lhs.compare(rhs) <= 0;
    }

    bool operator>(const path& lhs, const path& rhs) noexcept
    {
      return lhs.compare(rhs) > 0;
    }

    bool operator>=(const path& lhs, const path& rhs) noexcept
    {
      return lhs.compare(rhs) >= 0;
    }

    path operator/(const path& lhs, const path& rhs)
    {
      return path{lhs} /= rhs;
    }

    void swap(path& lhs, path& rhs) noexcept
    {
      lhs.swap(rhs);
    }

    path::iterator::iterator()
    : _path(nullptr)
    , _pos(string_type::npos)
    {
    }

    bool path::iterator::operator==(const iterator& other) const
    {
      return _path == other._path && _pos == other._pos;
    }

    bool path::iterator::operator!=(const iterator& other) const
    {
      return !(*this == other);
    }

    path::iterator::reference path::iterator::operator*() const
    {
      return _element;
    }

    path::iterator::pointer path::iterator::operator->() const
    {
      return &_element;
    }

    path::iterator& path::iterator::operator--()
    {
      if(_pos == string_type::npos) {
        _element.clear();
        _pos = _path->_path.length();
        if(_path->_path.at(_pos - 1) == _separator) {
          if(_pos > 1) {
            _element = path{string_type(1, _dot)};
          } else {
            _element = path{string_type(1, _separator)};
            _pos = 1;
          }
          return *this;
        }
      }

      const char* begin = _path->_path.c_str();
      const char* s = _path->_path.c_str() + _pos;
      ssize_t dist = 0;
      if(!_element.empty()) {
        s -= _element._path.length();
      }
      const char* end = s;
      --s;

      while(s != begin && *s == _separator) {
        --s;
      }

      dist = std::distance(s, end) - 1;
      if(dist > 0) {
        end -= dist;
      }

      if(s == begin) {
        // only slashes left
        _element = path{string_type(1, _separator)};
        _pos = 1;
        return *this;
      }

      while(s != begin && *s != _separator) {
        --s;
      }

      const char* tmp = *s == _separator ? s + 1 : s;
      // eat all remaining separators
      while(s != begin && *s == _separator) {
        --s;
      }

      dist = std::distance(s, tmp);
      if(dist == 2 && s == begin) {
        // net path
        tmp = s;
      }
      dist = std::distance(tmp, end);
      _element = _path->_path.substr(static_cast<string_type::size_type>(std::distance(begin, tmp)),
                                     static_cast<string_type::size_type>(dist));
      _pos = static_cast<string_type::size_type>(std::distance(begin, end));

      return *this;
    }

    path::iterator path::iterator::operator--(int)
    {
      const iterator old(*this);
      --(*this);
      return old;
    }

    path::iterator& path::iterator::operator++()
    {
      const char* s = _path->_path.c_str() + _pos;
      if(*s == '\0') {
        // at end of path, aka one behind end
        _pos = string_type::npos;
        return *this;
      }

      while(*s != '\0' && *s == _separator) {
        // eat all separator instances
        ++s;
        ++_pos;
      }

      const char* begin = s;
      ssize_t dist = 0;
      if(*s == '\0') {
        // separator at the end of the path
        _element = string_type(1, _dot);
      } else {
        // look for next separator or the end
        while(*s != '\0' && *s != _separator) {
          ++s;
        }
        dist = std::distance(begin, s);
        _element = _path->_path.substr(_pos, static_cast<string_type::size_type>(dist));
      }
      _pos += static_cast<string_type::size_type>(dist);

      return *this;
    }

    path::iterator path::iterator::operator++(int)
    {
      const iterator old{*this};
      ++(*this);
      return old;
    }

    directory_iterator begin(directory_iterator iter) noexcept
    {
      return iter;
    }

    directory_iterator end(const directory_iterator&) noexcept
    {
      return directory_iterator{};
    }

    filesystem_error::filesystem_error(const std::string& what_arg, std::error_code ec)
    : std::system_error(ec, what_arg)
    {
    }

    filesystem_error::filesystem_error(const std::string& what_arg, const path& p1, std::error_code ec)
    : std::system_error(ec, what_arg)
    , _path1(p1)
    {
    }

    filesystem_error::filesystem_error(const std::string& what_arg, const path& p1, const path& p2, std::error_code ec)
    : std::system_error(ec, what_arg)
    , _path1(p1)
    , _path2(p2)
    {
    }

    const char* filesystem_error::what() const noexcept
    {
      return std::system_error::what();
    }

    directory_entry::directory_entry(const class path& p)
    : _path(p)
    {
    }

    directory_entry::~directory_entry()
    {
    }

    void directory_entry::assign(const class path& p)
    {
      _path = p;
    }

    void directory_entry::replace_filename(const class path& p)
    {
      _path.replace_filename(p);
    }

    directory_entry::operator const class path&() const noexcept
    {
      return _path;
    }

    const path& directory_entry::path() const noexcept
    {
      return _path;
    }

    file_status directory_entry::status() const
    {
      std::error_code ec;
      return status(ec);
    }

    file_status directory_entry::status(std::error_code& ec) const noexcept
    {
      return filesystem::status(_path, ec);
    }

    file_status directory_entry::symlink_status() const
    {
      std::error_code ec;
      return symlink_status(ec);
    }

    file_status directory_entry::symlink_status(std::error_code& ec) const noexcept
    {
      return filesystem::symlink_status(_path, ec);
    }

    bool directory_entry::operator<(const directory_entry& rhs) const noexcept
    {
      return _path < rhs._path;
    }

    bool directory_entry::operator==(const directory_entry& rhs) const noexcept
    {
      return _path == rhs._path;
    }

    bool directory_entry::operator!=(const directory_entry& rhs) const noexcept
    {
      return _path != rhs._path;
    }

    bool directory_entry::operator<=(const directory_entry& rhs) const noexcept
    {
      return _path <= rhs._path;
    }

    bool directory_entry::operator>(const directory_entry& rhs) const noexcept
    {
      return _path > rhs._path;
    }

    bool directory_entry::operator>=(const directory_entry& rhs) const noexcept
    {
      return _path >= rhs._path;
    }

    bool is_block_file(file_status s) noexcept
    {
      return s.type() == file_type::block;
    }

    bool is_block_file(const path& p)
    {
      return is_block_file(status(p));
    }

    bool is_block_file(const path& p, std::error_code& ec) noexcept
    {
      return is_block_file(status(p, ec));
    }

    bool is_character_file(file_status s) noexcept
    {
      return s.type() == file_type::character;
    }

    bool is_character_file(const path& p)
    {
      return is_character_file(status(p));
    }

    bool is_character_file(const path& p, std::error_code& ec) noexcept
    {
      return is_character_file(status(p, ec));
    }

    bool is_directory(file_status s) noexcept
    {
      return s.type() == file_type::directory;
    }

    bool is_directory(const path& p)
    {
      return is_directory(status(p));
    }

    bool is_directory(const path& p, std::error_code& ec) noexcept
    {
      return is_directory(status(p, ec));
    }

    bool is_empty(const path& p)
    {
      std::error_code ec;
      bool ret = is_empty(p, ec);
      if(ec) {
        throw filesystem_error{"Cannot determine status of path", p, ec};
      }
      return ret;
    }

    bool is_empty(const path& p, std::error_code& ec) noexcept
    {
      file_status s = status(p, ec);
      if(ec) {
        return false;
      }

      if(is_directory(s)) {
        directory_iterator itr{p, ec};
        if(ec) {
          return false;
        }
        return itr == directory_iterator{};
      }
      uintmax_t sz = file_size(p, ec);
      if(ec) {
        return false;
      }
      return sz == 0;
    }

    bool is_fifo(file_status s) noexcept
    {
      return s.type() == file_type::fifo;
    }

    bool is_fifo(const path& p)
    {
      return is_fifo(status(p));
    }

    bool is_fifo(const path& p, std::error_code& ec) noexcept
    {
      return is_fifo(status(p, ec));
    }

    bool is_other(file_status s) noexcept
    {
      return exists(s) && !is_regular_file(s) && !is_directory(s) && !is_symlink(s);
    }

    bool is_other(const path& p)
    {
      return is_other(status(p));
    }

    bool is_other(const path& p, std::error_code& ec) noexcept
    {
      return is_other(status(p, ec));
    }

    bool is_regular_file(file_status s) noexcept
    {
      return s.type() == file_type::regular;
    }

    bool is_regular_file(const path& p)
    {
      return is_regular_file(status(p));
    }

    bool is_regular_file(const path& p, std::error_code& ec) noexcept
    {
      return is_regular_file(status(p, ec));
    }

    bool is_socket(file_status s) noexcept
    {
      return s.type() == file_type::socket;
    }

    bool is_socket(const path& p)
    {
      return is_socket(status(p));
    }

    bool is_socket(const path& p, std::error_code& ec) noexcept
    {
      return is_socket(status(p, ec));
    }

    bool is_symlink(file_status s) noexcept
    {
      return s.type() == file_type::symlink;
    }

    bool is_symlink(const path& p)
    {
      return is_symlink(symlink_status(p));
    }

    bool is_symlink(const path& p, std::error_code& ec) noexcept
    {
      return is_symlink(symlink_status(p, ec));
    }

    path absolute(const path& p, const path& base)
    {
      if(p.has_root_directory()) {
        if(p.has_root_name()) {
          return p;
        }
        if(base.has_root_name()) {
          return base.root_name() / p;
        }
        return p;
      }
      if(p.has_root_name()) {
        return p.root_name() / absolute(base).root_directory() / absolute(base).relative_path() / p.relative_path();
      }
      return absolute(base) / p;
    }

    path canonical(const path& p, const path& base)
    {
      std::error_code ec;
      path c = canonical(p, base, ec);
      if(ec) {
        throw filesystem_error{"Cannot produce canonical path", p, ec};
      }
      return c;
    }

    path canonical(const path& p, std::error_code& ec)
    {
      return canonical(p, current_path(), ec);
    }

    path current_path()
    {
      std::error_code ec;
      path p(current_path(ec));
      if(ec) {
        throw filesystem_error{"Cannot determine current path", ec};
      }
      return p;
    }

    void current_path(const path& p)
    {
      std::error_code ec;
      current_path(p, ec);
      if(ec) {
        throw filesystem_error{"Cannot set current path", p, ec};
      }
    }

    bool create_directories(const path& p)
    {
      std::error_code ec;
      if(!create_directories(p, ec)) {
        throw filesystem_error{"Cannot create directory", p, ec};
      }
      return true;
    }

    bool create_directories(const path& p, std::error_code& ec) noexcept
    {
      if(p.empty()) {
        ec = std::make_error_code(std::errc::not_a_directory);
        return false;
      }

      path dir;
      std::error_code ec1;
      for(auto& entry : p) {
        dir /= entry;
        file_status s = status(dir, ec1);
        if(exists(s)) {
          if(!is_directory(s)) {
            ec = std::make_error_code(std::errc::not_a_directory);
            return false;
          }
        } else {
          if(!create_directory(dir, ec)) {
            return false;
          }
        }
      }

      return true;
    }

    bool create_directory(const path& p)
    {
      std::error_code ec;
      if(!create_directory(p, ec)) {
        throw filesystem_error{"Cannot create directory", p, ec};
      }
      return true;
    }

    bool create_directory(const path& p, const path& existing_p)
    {
      std::error_code ec;
      if(!create_directory(p, existing_p, ec)) {
        throw filesystem_error{"Cannot create directory", p, ec};
      }
      return true;
    }

    bool equivalent(const path& p1, const path& p2)
    {
      std::error_code ec;
      bool ret = equivalent(p1, p2, ec);
      if(ec) {
        throw filesystem_error{"Cannot determine if paths are equivalent", p1, p2, ec};
      }
      return ret;
    }

    void create_symlink(const path& to, const path& new_symlink)
    {
      std::error_code ec;
      create_symlink(to, new_symlink, ec);
      if(ec) {
        throw acatl::filesystem::filesystem_error{"Cannot create symlink", to, new_symlink, ec};
      }
    }

    bool exists(file_status s) noexcept
    {
      return status_known(s) && s.type() != file_type::not_found;
    }

    bool exists(const path& p)
    {
      std::error_code ec;
      if(!exists(p, ec)) {
        if(ec) {
          throw filesystem_error{"Path does not exist", p, ec};
        } else {
          throw filesystem_error{"Cannot validate path", p, ec};
        }
      }
      return true;
    }

    bool exists(const path& p, std::error_code& ec) noexcept
    {
      file_status fs = status(p, ec);
      if(ec && status_known(fs)) {
        ec.clear();
      }
      return exists(fs);
    }

    uintmax_t file_size(const path& p)
    {
      std::error_code ec;
      uintmax_t size = file_size(p, ec);
      if(ec) {
        throw filesystem_error{"Cannot determine file size", p, ec};
      }
      return size;
    }

    void permissions(const path& p, perms prms)
    {
      std::error_code ec;
      permissions(p, prms, ec);
      if(ec) {
        throw filesystem_error{"Cannot set permissions on path", p, ec};
      }
    }

    path read_symlink(const path& p)
    {
      std::error_code ec;
      path result = read_symlink(p, ec);
      if(ec) {
        throw filesystem_error{"Cannot read symlink", p, ec};
      }
      return result;
    }

    bool remove(const path& p)
    {
      std::error_code ec;
      if(!remove(p, ec)) {
        throw filesystem_error{"Cannot remove path", p, ec};
      }
      return true;
    }

    std::uintmax_t remove_all(const path& p)
    {
      std::error_code ec;
      std::uintmax_t ret = remove_all(p, ec);
      if(ec) {
        throw filesystem_error{"Cannot remove path", p, ec};
      }
      return ret;
    }

    std::uintmax_t remove_all(const path& p, std::error_code& ec)
    {
      std::uintmax_t sum = 0;

      if(exists(symlink_status(p, ec))) {
        if(is_directory(p)) {
          directory_iterator iter{p};
          for(const auto& entry : iter) {
            sum += remove_all(entry, ec);
            if(ec) {
              sum = static_cast<uintmax_t>(-1);
              break;
            }
          }
          if(!remove(p, ec)) {
            sum = static_cast<uintmax_t>(-1);
          } else {
            ++sum;
          }
        } else if(remove(p, ec)) {
          sum = 1;
        }
      }
      return sum;
    }

    file_status status(const path& p)
    {
      std::error_code ec;
      file_status st = status(p, ec);
      if(st.type() == file_type::none) {
        throw filesystem_error{"Cannot get status of path", p, ec};
      }

      return st;
    }

    bool status_known(file_status s) noexcept
    {
      return s.type() != file_type::none;
    }

    file_status symlink_status(const path& p)
    {
      std::error_code ec;
      file_status st = symlink_status(p, ec);
      if(st.type() == file_type::none) {
        throw filesystem_error{"Cannot get status of path", p, ec};
      }

      return st;
    }

    path system_complete(const path& p)
    {
      std::error_code ec;
      path result = system_complete(p, ec);
      if(ec) {
        throw filesystem_error{"Cannot complete path", p, ec};
      }
      return result;
    }

    path system_complete(const path& p, std::error_code& ec)
    {
      return absolute(p, current_path(ec));
    }

    path temp_directory_path()
    {
      std::error_code ec;
      path result = temp_directory_path(ec);
      if(ec) {
        throw filesystem_error{"Cannot deduce temporary path", ec};
      }
      return result;
    }

    void PrintTo(const path& p, std::ostream* stream)
    {
      *stream << p.string();
    }

    path unique_temp_directory_path()
    {
      std::error_code ec;
      path p = unique_temp_directory_path(ec);
      if(ec) {
        throw filesystem_error{"Cannot create temporary directory", ec};
      }
      return p;
    }

    TemporaryDirectoryGuard::TemporaryDirectoryGuard()
    : _path(unique_temp_directory_path())
    {
    }

    const path& TemporaryDirectoryGuard::temporaryDirectoryPath() const
    {
      return _path;
    }

    TemporaryDirectoryGuard::~TemporaryDirectoryGuard()
    {
      std::error_code ec;
      if(equivalent(current_path(), _path, ec)) {
        current_path("..");
      }

      if(!ec) {
        remove_all(_path, ec);
      }
    }
  }
}
