//
//  command_line_options.h
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

#ifndef acatl_command_line_options_h
#define acatl_command_line_options_h

#include <acatl/exception.h>
#include <acatl/filesystem.h>
#include <acatl/stream_wrapper.h>
#include <acatl/string_helper.h>
#include <acatl/types.h>

#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

struct arg_hdr;
struct arg_lit;
struct arg_str;
struct arg_end;


namespace acatl
{
  ACATL_DECLARE_EXCEPTION(BadOption, acatl::Exception);

  /// Converts a string into the parametrized type. An OptionConverter is used by the TypedOption to convert the
  /// intern type into the one requested by the user. If you have to convert to a type that is currently not
  /// supported, you can just implement your own converter.
  template<typename T>
  struct OptionConverter
  {
    static T convert(const char* s);
  };

  /// Converts a string into the float type
  template<>
  struct OptionConverter<float>
  {
    static float convert(const char* s)
    {
      return static_cast<float>(std::stod(s));
    }
  };

  /// Converts a string into the double type
  template<>
  struct OptionConverter<double>
  {
    static double convert(const char* s)
    {
      return std::stod(s);
    }
  };

  /// Converts a string into the uint8_t type
  template<>
  struct OptionConverter<uint8_t>
  {
    static uint8_t convert(const char* s)
    {
      return static_cast<uint8_t>(std::stoul(s));
    }
  };

  /// Converts a string into the uint16_t type
  template<>
  struct OptionConverter<uint16_t>
  {
    static uint16_t convert(const char* s)
    {
      return static_cast<uint16_t>(std::stoul(s));
    }
  };

  /// Converts a string into the uint32_t type
  template<>
  struct OptionConverter<uint32_t>
  {
    static uint32_t convert(const char* s)
    {
      return static_cast<uint32_t>(std::stoul(s));
    }
  };

  /// Converts a string into the uint64_t type
  template<>
  struct OptionConverter<uint64_t>
  {
    static uint64_t convert(const char* s)
    {
      return static_cast<uint64_t>(std::stoul(s));
    }
  };

  /// Converts a string into the int8_t type
  template<>
  struct OptionConverter<int8_t>
  {
    static int8_t convert(const char* s)
    {
      return static_cast<int8_t>(std::stoi(s));
    }
  };

  /// Converts a string into the int16_t type
  template<>
  struct OptionConverter<int16_t>
  {
    static int16_t convert(const char* s)
    {
      return static_cast<int16_t>(std::stoi(s));
    }
  };

  /// Converts a string into the int32_t type
  template<>
  struct OptionConverter<int32_t>
  {
    static int32_t convert(const char* s)
    {
      return static_cast<int32_t>(std::stol(s));
    }
  };

  /// Converts a string into the int64_t type
  template<>
  struct OptionConverter<int64_t>
  {
    static int64_t convert(const char* s)
    {
      return static_cast<int64_t>(std::stoull(s));
    }
  };

  /// Converts a string into the string type
  template<>
  struct OptionConverter<std::string>
  {
    static std::string convert(const char* s)
    {
      return std::string(s);
    }
  };

  /// Converts a string into the filesystem::path type
  template<>
  struct OptionConverter<acatl::filesystem::path>
  {
    static acatl::filesystem::path convert(const char* s)
    {
      return acatl::filesystem::path(s);
    }
  };

  /// Base class for all option types
  class OptionBase
  {
  public:
    typedef std::shared_ptr<OptionBase> Ptr;

    /// Create an option
    /// @param shortOpt One character option name (like -p)
    /// @param longOpt Long option name (like --port)
    /// @param min Minimum number of options that have to be specified on the command line
    /// @param max Maximum number of options that can be specified on the command line
    /// @param desc Description that will be shown in the help glossary
    explicit OptionBase(const std::string& shortOpt,
                        const std::string& longOpt,
                        int min,
                        int max,
                        const std::string& desc);

    virtual ~OptionBase();

    /// Add an option to the command line option parser
    /// @param argtable The argument table to add this option to
    virtual void add(std::vector<void*>& argtable) = 0;

    /// Count the number of this option on the command line
    /// @return Number of found options
    virtual int count() const = 0;

  protected:
    friend class OptionGroup;

    struct arg_hdr* _hdr{nullptr};

    const std::string _shortOpt;
    const std::string _longOpt;
    const int _min;
    const int _max;
    const std::string _desc;
  };

  /// Option class for literals (i.e. argumentless options)
  class LiteralOption : public OptionBase
  {
  public:
    /// Creates a LiteralOption
    /// @param shortOpt One character option name (like -p)
    /// @param longOpt Long option name (like --port)
    /// @param min Minimum number of options that have to be specified on the command line
    /// @param max Maximum number of options that can be specified on the command line
    /// @param desc Description that will be shown in the help glossary
    explicit LiteralOption(const std::string& shortOpt,
                           const std::string& longOpt,
                           int min,
                           int max,
                           const std::string& desc);

    void add(std::vector<void*>& argtable) override;

    int count() const override;

  private:
    struct arg_lit* _arg{nullptr};
  };

  /// Option class for typed options with arguments
  class TypedOption : public OptionBase
  {
  public:
    /// Creates a TypedOption
    /// @param shortOpt One character option name (like -p)
    /// @param longOpt Long option name (like --port)
    /// @param argument Description of the argument type to specify on the command line
    /// @param min Minimum number of options that have to be specified on the command line
    /// @param max Maximum number of options that can be specified on the command line
    /// @param desc Description that will be shown in the help glossary
    explicit TypedOption(const std::string& shortOpt,
                         const std::string& longOpt,
                         const std::string argument,
                         int min,
                         int max,
                         const std::string& desc);

    void add(std::vector<void*>& argtable) override;

    int count() const override;

    /// Read out the options argument value with the given type. The argument will be converted to the given type
    /// using a converter. If there is no standard converter, you can implement your own.
    /// Will throw a BadOption exception if the index is out of range.
    /// @tparam T The type to convert the argument into
    /// @param idx The index of the argument. Use count to find out how many arguments where specfied
    /// @return The converter value
    template<typename T>
    T value(int idx = 0) const
    {
      return OptionConverter<T>::convert(value(idx));
    }

  private:
    const char* value(int idx) const;

    struct arg_str* _arg{nullptr};

    const std::string _argument;
  };

  /// Wraps an option in order to discriminate between literal and typed options. This class should
  /// normally not be used directly by the user, as it serves only as an abstraction layer.
  /// A wrapper should not be stored for later use.
  class OptionWrapper
  {
  public:
    /// Create an OptionWrapper
    /// @param opt The option to wrap
    OptionWrapper(const OptionBase& opt);

    int count() const;

    template<typename T>
    T value(int idx = 0) const
    {
      const TypedOption* gen = dynamic_cast<const TypedOption*>(&_opt);
      if(!gen) {
        ACATL_THROW(BadOption, "Option is not of correct type");
      }
      return gen->value<T>(idx);
    }

  private:
    const OptionBase& _opt;
  };

  /// Option creator. This class is used by the CommandLineOptions to hide the option specific details from
  /// the user and make the interface easy to use.
  class Option
  {
  public:
    /// Creates a LiteralOption.
    /// @param shortOpt One character option name (like -p)
    /// @param longOpt Long option name (like --port)
    /// @param min Minimum number of options that have to be specified on the command line
    /// @param max Maximum number of options that can be specified on the command line
    /// @param desc Description that will be shown in the help glossary
    Option(const std::string& shortOpt, const std::string& longOpt, int min, int max, const std::string& desc);

    /// Creates a LiteralOption. The min is set to 0 and the max to 1.
    /// @param shortOpt One character option name (like -p)
    /// @param longOpt Long option name (like --port)
    /// @param desc Description that will be shown in the help glossary
    Option(const std::string& shortOpt, const std::string& longOpt, const std::string& desc);

    /// Creates a TypedOption.
    /// @param shortOpt One character option name (like -p)
    /// @param longOpt Long option name (like --port)
    /// @param argument Description of the argument type to specify on the command line
    /// @param min Minimum number of options that have to be specified on the command line
    /// @param max Maximum number of options that can be specified on the command line
    /// @param desc Description that will be shown in the help glossary
    Option(const std::string& shortOpt,
           const std::string& longOpt,
           const std::string argument,
           int min,
           int max,
           const std::string& desc);

    /// Creates a TypedOption. The min is set to 0 and the max to 1.
    /// @param shortOpt One character option name (like -p)
    /// @param longOpt Long option name (like --port)
    /// @param argument Description of the argument type to specify on the command line
    /// @param desc Description that will be shown in the help glossary
    Option(const std::string& shortOpt, const std::string& longOpt, const std::string argument, const std::string& desc);

  private:
    friend class OptionGroup;

    OptionBase::Ptr _option;
  };

  /// The OptionGroup groups options that belong together. Each group shall be a consistent argument set of the
  /// command line. Options can be associated to various groups, but each group should have a unique set of options.
  class OptionGroup
  {
  public:
    /// Create an OptionGroup.
    /// @param name Name of this option group. The name will be displayed in the usage as group name.
    /// @param options The actual options to configure for this group
    OptionGroup(const std::string& name, std::initializer_list<Option> options);

    ~OptionGroup();

    OptionGroup(const OptionGroup& rhs);

    /// Returns the name of this option group.
    /// @return Name of this option group
    const std::string& name() const;

    /// Prints the usage syntax of this command line group to the given stream.
    /// @param stream Stream to output the syntax to
    void usageSyntax(std::ostream& stream);

    /// Prints the usage glossary of this command line group to the given stream.
    /// @param stream Stream to output the gloassary to
    void usageGlossary(std::ostream& stream);

    /// Parses the command line and tries to match this option group to the given arguments.
    /// @param program The name of the program these options are for. The name of the program will be used
    ///        for the output of parse errors.
    /// @param args The command line options as array
    /// @param errorStream The stream to output errors to
    /// @return true upon success, false if the command line arguments don't match the option group
    bool parse(const std::string& program, const acatl::StringVector& args, std::ostream& errorStream);

    /// Count the number of occurances of the option with the given name found on the command line.
    /// @param name The name of the option to count
    /// @return The number of occurances. Will be 0 if the argument does not appear on the command line.
    int count(const std::string& name) const;

    /// Return the option with the given name. Will only find the option, if the count of the option
    /// is greater than zero. Will throw a BadOption exception if the option is not found.
    /// @param name Name of the option to find
    /// @return The option if found
    OptionWrapper option(const std::string& name) const;

    /// Will return the number of untagged options found on the command line.
    /// @return The number of untagged options found. Will return zero if no untagged options where found.
    int untaggedOptionsCount() const;

    /// Returns the number of elements found for the given untagged option.
    /// @param index Index of untagged option
    /// @return  Number of elements found for the given untagged option. Will return zero if no
    ///          untagged option with the given index exists.
    int count(int index) const;

    /// Returns the untagged option with the given index. Will thrown a BadOption if there is no
    /// untagged option with this index.
    /// @param index The index of the untagged option to find
    /// @return The untagged option if found
    OptionWrapper untaggedOption(int index) const;

  private:
    std::string _name;
    std::vector<OptionBase::Ptr> _options;
    std::vector<void*> _argtable;
    struct arg_end* _end{nullptr};
  };

  /// The CommandLineOptions class wraps the argtable3 command line parser ( http://www.argtable.org )
  /// into an easy to use C++ interface.
  /// OptionGroups will be added and parsed with the given command line. The matching OptionGroup will be used
  /// for all option finding methods. Both tagged and untagged arguments are supported, as are multiple
  /// instances of each argument. If need be, the usage of the command line will be streamed to the user in a
  /// user friendly way.
  class CommandLineOptions
  {
  public:
    /// Creates a CommandLineOptions instance.
    /// @param program The name of the program for which the command line shall be parsed. Will be used
    ///                for error and usage output.
    /// @param groups The actual option groups for the command line
    explicit CommandLineOptions(const std::string& program, std::initializer_list<OptionGroup> groups);

    /// Streams the usage of the command line options for user output. The usage will be formatted in a
    /// nice and comprehensive way with each group in an own section.
    /// @param stream The sream to output the usage to
    void usage(std::ostream& stream) const;

    /// Parses the given command line arguments and tries to match each group. The first group that matches
    /// will be used for all other option finding calls.
    /// @param args The command line arguments to parse
    /// @param errorStream The stream to output errors to
    /// @return true if the parsing matches one of the groups, false if no group matches. If no group matches,
    ///         parsing error messages will be streamed to the errorStream.
    bool parse(const acatl::StringVector& args, std::ostream& errorStream);

    /// Count the number of occurances of the option with the given name found on the command line.
    /// @param name The name of the option to count
    /// @return The number of occurances. Will be 0 if the argument does not appear on the command line.
    int count(const std::string& name) const;

    /// Returns the number of elements found for the given untagged option.
    /// @param index Index of untagged option
    /// @return  Number of elements found for the given untagged option. Will return zero if no
    ///          untagged option with the given index exists.
    int count(int index) const;

    /// Will return the number of untagged options found on the command line.
    /// @return The number of untagged options found. Will return zero if no untagged options where found.
    int untaggedOptionsCount() const;

    /// Return the option with the given name. Will only find the option, if the count of the option
    /// is greater zero. Will throw a BadOption exception if the option is not found.
    /// @param name Name of the option to find
    /// @return The option if found
    OptionWrapper option(const std::string& name) const;

    /// Returns the untagged option with the given index. Will thrown a BadOption if there is no
    /// untagged option with this index.
    /// @param index The index of the untagged option to find
    /// @return The untagged option if found
    OptionWrapper untaggedOption(int index) const;

    /// Returns the name of the option group that was parsed correctly.
    /// @return Name of the active group
    const std::string& groupName() const;

  private:
    std::string _program;
    std::vector<OptionGroup> _options;
    OptionGroup* _activeGroup{nullptr};
  };
}

#endif
