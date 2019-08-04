//
//  filesystem.cpp
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2016, Lars-Christian Fürstenberg
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

#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <stack>
#include <vector>

namespace acatl
{
  namespace filesystem
  {
    static void setStatus(file_status& st, const struct stat& buffer)
    {
      if(S_ISREG(buffer.st_mode)) {
        st.type(file_type::regular);
      } else if(S_ISDIR(buffer.st_mode)) {
        st.type(file_type::directory);
      } else if(S_ISBLK(buffer.st_mode)) {
        st.type(file_type::block);
      } else if(S_ISCHR(buffer.st_mode)) {
        st.type(file_type::character);
      } else if(S_ISFIFO(buffer.st_mode)) {
        st.type(file_type::fifo);
      } else if(S_ISSOCK(buffer.st_mode)) {
        st.type(file_type::socket);
      } else if(S_ISLNK(buffer.st_mode)) {
        st.type(file_type::symlink);
      } else {
        st.type(file_type::unknown);
      }

      perms prms{perms(buffer.st_mode) & perms::mask};
      st.permissions(prms);
    }

    static file_status status(struct stat& buffer, int statReturn, std::error_code& ec) noexcept
    {
      file_status st;

      if(statReturn == -1) {
        if(errno == ENOENT || errno == ENOTDIR) {
          st.type(file_type::not_found);
        } else if(errno == EACCES || errno == EOVERFLOW) {
          st.type(file_type::unknown);
        } else {
          st.type(file_type::none);
        }
        ec.assign(errno, std::system_category());
        st.permissions(perms::unknown);
      } else {
        ec.clear();
        setStatus(st, buffer);
      }

      return st;
    }

    path canonical(const path& p, const path& base, std::error_code& ec)
    {
      path c;
      char* buf = ::realpath(absolute(p, base).c_str(), nullptr);
      if(buf == nullptr) {
        ec.assign(errno, std::system_category());
        return path();
      }
      c = buf;
      ::free(buf);

      return c;
    }

    path current_path(std::error_code& ec) noexcept
    {
      char* buf = ::getcwd(nullptr, 0);

      if(buf == nullptr) {
        ec.assign(errno, std::system_category());
        return path();
      }

      path p{buf};
      ::free(buf);

      if(p.c_str()[0] == '(') {
        // path is not reachable
        ec = std::make_error_code(std::errc::permission_denied);
        return path();
      }

      ec.clear();
      return p;
    }

    void current_path(const path& p, std::error_code& ec) noexcept
    {
      if(::chdir(p.c_str()) < 0) {
        ec.assign(errno, std::system_category());
        return;
      }
      ec.clear();
    }

    bool create_directory(const path& p, std::error_code& ec) noexcept
    {
      if(p.empty()) {
        ec = std::make_error_code(std::errc::not_a_directory);
        return false;
      }
      if(!is_directory(p.parent_path())) {
        ec = std::make_error_code(std::errc::not_a_directory);
        return false;
      }
      std::error_code statusec;
      file_status s = status(p, statusec);
      if(exists(s) && !is_directory(s)) {
        ec = std::make_error_code(std::errc::not_a_directory);
        return false;
      }
      if(::mkdir(p.c_str(), static_cast<int>(perms::all)) == -1) {
        if(errno != EEXIST) {
          ec.assign(errno, std::system_category());
          return false;
        }
      }
      return true;
    }

    bool create_directory(const path& p, const path& existing_p, std::error_code& ec) noexcept
    {
      if(p.empty()) {
        ec = std::make_error_code(std::errc::not_a_directory);
        return false;
      }
      if(!is_directory(p.parent_path())) {
        ec = std::make_error_code(std::errc::not_a_directory);
        return false;
      }
      file_status s = status(p);
      if(exists(s) && !is_directory(s)) {
        ec = std::make_error_code(std::errc::not_a_directory);
        return false;
      }

      struct stat attributes_stat;
      if(::stat(existing_p.c_str(), &attributes_stat) == -1) {
        ec.assign(errno, std::system_category());
        return false;
      }
      if(::mkdir(p.c_str(), attributes_stat.st_mode) == -1) {
        if(errno != EEXIST) {
          ec.assign(errno, std::system_category());
          return false;
        }
      }

      return true;
    }

    void create_symlink(const path& to, const path& new_symlink, std::error_code& ec) noexcept
    {
      if(::symlink(to.c_str(), new_symlink.c_str())) {
        ec.assign(errno, std::system_category());
      }
    }

    bool equivalent(const path& p1, const path& p2, std::error_code& ec) noexcept
    {
      struct stat buffer;
      if(::stat(p1.c_str(), &buffer) == -1) {
        ec.assign(errno, std::system_category());
        return false;
      }

      file_status s1 = status(buffer, 0, ec);
      if(ec) {
        return false;
      }

      struct stat buffer2;
      if(::stat(p2.c_str(), &buffer2) == -1) {
        ec.assign(errno, std::system_category());
        return false;
      }
      file_status s2 = status(buffer2, 0, ec);
      if(ec) {
        return false;
      }

      if((!exists(s1) && !exists(s2)) || (is_other(s1) && is_other(s2))) {
        ec = std::make_error_code(std::errc::invalid_argument);
      }

      return s1 == s2 && buffer.st_dev == buffer2.st_dev && buffer.st_ino == buffer2.st_ino;
    }

    uintmax_t file_size(const path& p, std::error_code& ec) noexcept
    {
      struct stat buffer;
      if(::stat(p.c_str(), &buffer) == -1) {
        ec.assign(errno, std::system_category());
        return static_cast<uintmax_t>(-1);
      }

      file_status fs = status(buffer, 0, ec);
      if(ec) {
        return static_cast<uintmax_t>(-1);
      }
      if(!exists(fs) || !is_regular_file(fs)) {
        return static_cast<uintmax_t>(-1);
      }
      return static_cast<uintmax_t>(buffer.st_size);
    }

    void permissions(const path& p, perms prms, std::error_code& ec)
    {
      if(!((prms & perms::add_perms) != perms::none && (prms & perms::remove_perms) != perms::none)) {
        return;
      }

      path f{p};
      if((prms & perms::symlink_nofollow) != perms::none) {
        f = read_symlink(p, ec);
        if(ec) {
          return;
        }
      }

      file_status s = (prms & perms::symlink_nofollow) != perms::none ? symlink_status(p) : status(p);
      perms newPerms;
      if((prms & perms::add_perms) != perms::none) {
        newPerms = s.permissions() | (prms & perms::mask);
      } else {
        newPerms = s.permissions() & ~(prms & perms::mask);
      }

      if(::chmod(f.c_str(), static_cast<mode_t>(newPerms)) == -1) {
        ec.assign(errno, std::system_category());
      }
    }

    path read_symlink(const path& p, std::error_code& ec)
    {
      struct stat sb;
      std::vector<char> linkname;
      size_t bufsize;

      if(::lstat(p.c_str(), &sb) == -1) {
        ec.assign(errno, std::system_category());
        return path();
      }

      bufsize = static_cast<size_t>(sb.st_size + 1);
      if(sb.st_size == 0) {
        bufsize = PATH_MAX;
      }

      linkname.resize(bufsize);

      ssize_t result = ::readlink(p.c_str(), &linkname[0], bufsize);
      if(result == -1) {
        ec.assign(errno, std::system_category());
        return path();
      }
      if(static_cast<size_t>(result) >= bufsize) {
        ec = std::make_error_code(std::errc::filename_too_long);
        return path();
      }
      linkname[static_cast<size_t>(result)] = '\0';

      return path(&linkname[0]);
    }

    bool remove(const path& p, std::error_code& ec)
    {
      if(exists(symlink_status(p, ec))) {
        if(::remove(p.c_str()) < 0) {
          ec.assign(errno, std::system_category());
          return false;
        }
        return true;
      }
      return false;
    }

    file_status status(const path& p, std::error_code& ec) noexcept
    {
      struct stat buffer;
      return status(buffer, ::stat(p.c_str(), &buffer), ec);
    }

    file_status symlink_status(const path& p, std::error_code& ec) noexcept
    {
      file_status st;

      struct stat buffer;
      int status;
      status = ::lstat(p.c_str(), &buffer);
      if(status == -1) {
        if(errno == ENOENT || errno == ENOTDIR) {
          st.type(file_type::not_found);
        } else if(errno == EACCES || errno == EOVERFLOW) {
          st.type(file_type::unknown);
        } else {
          st.type(file_type::none);
        }
        ec.assign(errno, std::system_category());
        st.permissions(perms::unknown);
      } else {
        ec.clear();
        setStatus(st, buffer);
      }

      return st;
    }

    path temp_directory_path(std::error_code& ec)
    {
      char* value = nullptr;

      (value = ::getenv("TMPDIR")) || (value = ::getenv("TMP")) || (value = ::getenv("TEMP"))
        || (value = ::getenv("TEMPDIR"));

      path p{value ? value : "/tmp"};
      if(p.empty() || !is_directory(p, ec)) {
        ec = std::make_error_code(std::errc::not_a_directory);
      }

      return p;
    }

    struct directory_iterator::impl
    {
      impl()
      : _dir(nullptr)
      , _options(directory_options::none)
      {
      }

      impl(const path& p)
      : _path(p)
      , _dir(nullptr)
      , _options(directory_options::none)
      {
        std::error_code ec;
        open(p, ec);
        if(ec) {
          throw filesystem_error{"Cannot open path", p, ec};
        }
      }

      impl(const path& p, std::error_code& ec)
      : _path(p)
      , _dir(nullptr)
      , _options(directory_options::none)
      {
        open(p, ec);
      }

      impl(const path& p, directory_options options)
      : _path(p)
      , _dir(nullptr)
      , _options(options)
      {
        std::error_code ec;
        open(p, ec);
        if(ec) {
          if((options & directory_options::skip_permission_denied) != directory_options::none
             && ec == std::errc::permission_denied) {
            close();
          } else {
            throw filesystem_error{"Cannot open path", p, ec};
          }
        }
      }

      impl(const path& p, directory_options options, std::error_code& ec)
      : _path(p)
      , _dir(nullptr)
      , _options(options)
      {
        open(p, ec);
      }

      ~impl()
      {
        close();
      }

      bool operator==(const impl& other) const
      {
        return _dir == other._dir;
      }

      bool operator!=(const impl& other) const
      {
        return _dir != other._dir;
      }

      bool isEnd() const
      {
        return !_dir;
      }

      void open(const path& p, std::error_code& ec)
      {
        _dir = ::opendir(p.c_str());
        if(!_dir) {
          ec.assign(errno, std::system_category());
        } else {
          readdir(ec);
        }
      }

      void close()
      {
        if(_dir) {
          ::closedir(_dir);
          _dir = nullptr;
        }
      }

      void readdir(std::error_code& ec)
      {
        while(!internalReaddir(ec))
          ;
      }

      bool internalReaddir(std::error_code& ec)
      {
        errno = 0;
        struct dirent* res = ::readdir(_dir);
        if(res == nullptr && errno != 0) {
          ec.assign(errno, std::system_category());
        } else if(res == nullptr) {
          // end
          ec.clear();
          close();
        } else {
          if(::strcmp(".", res->d_name) == 0 || ::strcmp("..", res->d_name) == 0) {
            return false;
          }
          _dirEntry = directory_entry{_path / res->d_name};
        }
        return true;
      }

      path _path;
      DIR* _dir;
      directory_options _options;
      directory_entry _dirEntry;
    };

    directory_iterator::directory_iterator() noexcept
    : _impl(new impl)
    {
    }

    directory_iterator::directory_iterator(const path& p)
    : _impl(new impl(p))
    {
    }

    directory_iterator::directory_iterator(const path& p, directory_options options)
    : _impl(new impl(p, options))
    {
    }

    directory_iterator::directory_iterator(const path& p, std::error_code& ec) noexcept
    : _impl(new impl(p, ec))
    {
    }

    directory_iterator::directory_iterator(const path& p, directory_options options, std::error_code& ec) noexcept
    : _impl(new impl(p, options, ec))
    {
    }

    directory_iterator::directory_iterator(const directory_iterator& rhs)
    : _impl(rhs._impl)
    {
    }

    directory_iterator::directory_iterator(directory_iterator&& rhs) noexcept
    : _impl(std::move(rhs._impl))
    {
    }

    directory_iterator::~directory_iterator()
    {
    }

    directory_iterator::iterator& directory_iterator::operator=(const directory_iterator& rhs)
    {
      _impl = rhs._impl;
      return *this;
    }

    directory_iterator::iterator& directory_iterator::operator=(directory_iterator&& rhs) noexcept
    {
      _impl = std::move(rhs._impl);
      return *this;
    }

    bool directory_iterator::operator==(const directory_iterator& other) const
    {
      return *_impl == *(other._impl);
    }

    bool directory_iterator::operator!=(const directory_iterator& other) const
    {
      return *_impl != *(other._impl);
    }

    directory_iterator::reference directory_iterator::operator*() const
    {
      return _impl->_dirEntry;
    }

    directory_iterator::pointer directory_iterator::operator->() const
    {
      return &(_impl->_dirEntry);
    }

    directory_iterator::iterator& directory_iterator::operator++()
    {
      std::error_code ec;
      _impl->readdir(ec);
      return *this;
    }

    directory_iterator::iterator& directory_iterator::increment(std::error_code& ec) noexcept
    {
      _impl->readdir(ec);
      return *this;
    }

    struct recursive_directory_iterator::impl
    {
      impl()
      : _options(directory_options::none)
      , _recursionPending(true)
      {
      }

      impl(const path& p)
      : _options(directory_options::none)
      , _recursionPending(true)
      {
        std::error_code ec;
        open(p, _options, ec);
      }

      impl(const path& p, std::error_code& ec)
      : _options(directory_options::none)
      , _recursionPending(true)
      {
        open(p, _options, ec);
      }

      impl(const path& p, directory_options options)
      : _options(options)
      , _recursionPending(true)
      {
        std::error_code ec;
        open(p, _options, ec);
      }

      impl(const path& p, directory_options options, std::error_code& ec)
      : _options(options)
      , _recursionPending(true)
      {
        open(p, _options, ec);
      }

      void open(const path& p, directory_options options, std::error_code& ec)
      {
        directory_iterator iter{p, options, ec};
        if(!ec) {
          _iteratorStack.push(iter);
          if(_iteratorStack.top() == directory_iterator{}) {
            _iteratorStack.pop();
          }
        }
      }

      bool operator==(const impl& other) const
      {
        return _iteratorStack == other._iteratorStack;
      }

      bool operator!=(const impl& other) const
      {
        return _iteratorStack != other._iteratorStack;
      }

      void increment()
      {
        std::error_code ec;
        increment(ec);
        if(ec) {
          throw filesystem_error{"Cannot increment iterator", ec};
        }
      }

      void findNextEntry()
      {
        while(_iteratorStack.top() == directory_iterator()) {
          _iteratorStack.pop();
          if(_iteratorStack.empty()) {
            break;
          }
          if(_iteratorStack.top() != directory_iterator()) {
            ++(_iteratorStack.top());
          }
        }
      }

      void increment(std::error_code& ec)
      {
        if(_iteratorStack.size()) {
          if(_recursionPending && is_directory(_iteratorStack.top()->status())
             && (!is_symlink(_iteratorStack.top()->symlink_status())
                 || (_options & directory_options::follow_directory_symlink) != directory_options::none)) {
            directory_iterator iter(_iteratorStack.top()->path(), ec);
            if(!ec) {
              _iteratorStack.push(iter);
              findNextEntry();
            }
          } else {
            ++(_iteratorStack.top());
            findNextEntry();
          }
        }
      }

      std::stack<directory_iterator> _iteratorStack;
      directory_options _options;
      bool _recursionPending;
    };

    recursive_directory_iterator::recursive_directory_iterator() noexcept
    : _impl(new impl)
    {
    }

    recursive_directory_iterator::recursive_directory_iterator(const path& p)
    : _impl(new impl(p))
    {
    }

    recursive_directory_iterator::recursive_directory_iterator(const path& p, directory_options options)
    : _impl(new impl(p, options))
    {
    }

    recursive_directory_iterator::recursive_directory_iterator(const path& p,
                                                               directory_options options,
                                                               std::error_code& ec) noexcept
    : _impl(new impl(p, options, ec))
    {
    }

    recursive_directory_iterator::recursive_directory_iterator(const path& p, std::error_code& ec) noexcept
    : _impl(new impl(p, ec))
    {
    }

    recursive_directory_iterator::recursive_directory_iterator(const recursive_directory_iterator& rhs)
    : _impl(rhs._impl)
    {
    }

    recursive_directory_iterator::recursive_directory_iterator(recursive_directory_iterator&& rhs) noexcept
    : _impl(std::move(rhs._impl))
    {
    }

    recursive_directory_iterator::~recursive_directory_iterator()
    {
    }

    directory_options recursive_directory_iterator::options() const
    {
      return _impl->_options;
    }

    int recursive_directory_iterator::depth() const
    {
      return static_cast<int>(_impl->_iteratorStack.size());
    }

    bool recursive_directory_iterator::recursion_pending() const
    {
      return _impl->_recursionPending;
    }

    const directory_entry& recursive_directory_iterator::operator*() const
    {
      if(_impl->_iteratorStack.empty()) {
        std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);
        throw filesystem_error{"Trying to dereference an end recursive directory iterator", ec};
      }

      return *_impl->_iteratorStack.top();
    }

    const directory_entry* recursive_directory_iterator::operator->() const
    {
      if(_impl->_iteratorStack.empty()) {
        std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);
        throw filesystem_error{"Trying to dereference an end recursive directory iterator", ec};
      }

      return _impl->_iteratorStack.top().operator->();
    }

    recursive_directory_iterator& recursive_directory_iterator::operator=(const recursive_directory_iterator& rhs)
    {
      if(this != &rhs) {
        _impl = rhs._impl;
      }
      return *this;
    }

    recursive_directory_iterator& recursive_directory_iterator::operator=(recursive_directory_iterator&& rhs) noexcept
    {
      if(this != &rhs) {
        _impl = std::move(rhs._impl);
      }
      return *this;
    }

    recursive_directory_iterator& recursive_directory_iterator::operator++()
    {
      _impl->increment();
      return *this;
    }

    recursive_directory_iterator& recursive_directory_iterator::increment(std::error_code& ec) noexcept
    {
      _impl->increment(ec);
      return *this;
    }

    void recursive_directory_iterator::pop()
    {
      std::error_code ec;
      pop(ec);
      if(ec) {
        throw filesystem_error{"Cannot pop end iterator", ec};
      }
    }

    void recursive_directory_iterator::pop(std::error_code& ec)
    {
      if(depth() == 0) {
        *this = recursive_directory_iterator();
        return;
      }
      _impl->_iteratorStack.pop();
    }

    void recursive_directory_iterator::disable_recursion_pending()
    {
      _impl->_recursionPending = false;
    }

    bool recursive_directory_iterator::operator==(const recursive_directory_iterator& other) const
    {
      return *_impl == *(other._impl);
    }

    bool recursive_directory_iterator::operator!=(const recursive_directory_iterator& other) const
    {
      return *_impl != *(other._impl);
    }

    recursive_directory_iterator begin(recursive_directory_iterator iter) noexcept
    {
      return iter;
    }

    recursive_directory_iterator end(const recursive_directory_iterator&) noexcept
    {
      return recursive_directory_iterator();
    }

    path unique_temp_directory_path(std::error_code& ec)
    {
      path p{temp_directory_path(ec)};
      if(ec) {
        return path();
      }

      char tmpl[PATH_MAX];
      ::strncpy(tmpl, (p / "acatl_XXXXXX").c_str(), PATH_MAX);

      char* tmpDir = ::mkdtemp(tmpl);
      if(tmpDir == nullptr) {
        ec.assign(errno, std::system_category());
      }

      return path{tmpDir};
    }
  }
}
