//
//  console.h
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

#ifndef acatl_console_h
#define acatl_console_h

#include <acatl/filesystem.h>

#include <map>
#include <memory>

namespace fs = acatl::filesystem;

namespace acatl
{
  /// A console class to support interactive shells. The implementation is based on the linenoise-ng library. It
  /// supports most of readlines keystrokes. See http://readline.kablamo.org/emacs.html for a cheat sheet. See
  /// https://github.com/arangodb/linenoise-ng for more information on linenoise-ng.
  class Console
  {
  public:
    typedef std::function<bool(const std::vector<std::string>&)> CommandFunction;
    typedef std::function<bool(const std::string&)> DefaultCommandFunction;

    /// Constructs a console instance.
    /// @param prompt The prompt to display for user input
    /// @param historyPath Path to load and store the history to
    Console(const std::string& prompt, const fs::path& historyPath);

    /// Destroys the console instance and saves the command line history.
    ~Console();

    /// Start prompting the user for input. If the user hits return, the console searches for a command that matches
    /// the first word of the user input. If it is found, the rest of the command will be split into words and given
    /// to the command function as string vector of user input. The command itself will not be part of the vector.
    /// If no command function was found and a default command specified, the default command function will be
    /// called. If no command was found, a new prompt will be displayed.
    void run();

    /// Stop prompting the user for input. Needs a signal SIGINT to actually stop processing.
    void stop();

    /// Adds a command function to the console for processing. If the first word of the user input matches the
    /// command, the command function will be called. If the command function returns true, the current user input
    /// line will be added to the command line history.
    void addCommand(const std::string& command, CommandFunction function);

    /// Adds a default command, which is called if no other command was found.
    /// @param function Function to be called if no other command was found. The complete command string will be
    /// given to the function as
    ///                 parameter.
    void addDefault(DefaultCommandFunction function);

    /// Throws away the currently accumulated command history.
    void clearHistory();

  private:
    typedef std::map<std::string, CommandFunction> Commands;

    fs::path _historyPath;
    Commands _commands;
    DefaultCommandFunction _defaultCommand;
    std::string _prompt;
    bool _stop;
  };
}

#endif
