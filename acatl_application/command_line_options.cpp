//
//  command_line_options.cpp
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

#include "command_line_options.h"

#include <argtable3.h>


namespace acatl
{
    OptionBase::OptionBase(const std::string& shortOpt, const std::string& longOpt, int min, int max, const std::string& desc)
    : _hdr(nullptr)
    , _shortOpt(shortOpt)
    , _longOpt(longOpt)
    , _min(min)
    , _max(max)
    , _desc(desc)
    {
    }

    OptionBase::~OptionBase()
    {
    }


    LiteralOption::LiteralOption(const std::string& shortOpt, const std::string& longOpt, int min, int max, const std::string& desc)
    : OptionBase(shortOpt, longOpt, min, max, desc)
    , _arg(nullptr)
    {
    }

    void LiteralOption::add(std::vector<void*>& argtable)
    {
        _arg =
        arg_litn(_shortOpt.size() ? _shortOpt.c_str() : nullptr, _longOpt.size() ? _longOpt.c_str() : nullptr, _min, _max, _desc.c_str());
        argtable.emplace_back(_arg);
        _hdr = reinterpret_cast<struct arg_hdr*>(_arg);
    }

    int LiteralOption::count() const
    {
        return _arg->count;
    }


    TypedOption::TypedOption(const std::string& shortOpt, const std::string& longOpt, const std::string argument, int min, int max, const std::string& desc)
    : OptionBase(shortOpt, longOpt, min, max, desc)
    , _arg(nullptr)
    , _argument(argument)
    {
    }

    void TypedOption::add(std::vector<void*>& argtable)
    {
        _arg = arg_strn(_shortOpt.size() ? _shortOpt.c_str() : nullptr,
                        _longOpt.size() ? _longOpt.c_str() : nullptr,
                        _argument.c_str(),
                        _min,
                        _max,
                        _desc.c_str());
        argtable.emplace_back(_arg);
        _hdr = reinterpret_cast<struct arg_hdr*>(_arg);
    }

    int TypedOption::count() const
    {
        return _arg->count;
    }

    const char* TypedOption::value(int idx) const
    {
        if(idx > count()) {
            ACATL_THROW(BadOption, "Index out of range");
        }
        return _arg->sval[idx];
    }


    OptionWrapper::OptionWrapper(const OptionBase& opt)
    : _opt(opt)
    {
    }

    int OptionWrapper::count() const
    {
        return _opt.count();
    }


    Option::Option(const std::string& shortOpt, const std::string& longOpt, int min, int max, const std::string& desc)
    : _option(new LiteralOption(shortOpt, longOpt, min, max, desc))
    {
    }

    Option::Option(const std::string& shortOpt, const std::string& longOpt, const std::string& desc)
    : _option(new LiteralOption(shortOpt, longOpt, 0, 1, desc))
    {
    }

    Option::Option(const std::string& shortOpt, const std::string& longOpt, const std::string argument, int min, int max, const std::string& desc)
    : _option(new TypedOption(shortOpt, longOpt, argument, min, max, desc))
    {
    }

    Option::Option(const std::string& shortOpt, const std::string& longOpt, const std::string argument, const std::string& desc)
    : _option(new TypedOption(shortOpt, longOpt, argument, 0, 1, desc))
    {
    }


    OptionGroup::OptionGroup(const std::string& name, std::initializer_list<Option> options)
    : _name(name)
    {
        for(auto& opt : options) {
            opt._option->add(_argtable);
            _options.push_back(std::move(opt._option));
        }
        _argtable.emplace_back(_end = arg_end(20));
    }

    OptionGroup::~OptionGroup()
    {
        arg_freetable(&_argtable[0], _argtable.size());
    }

    OptionGroup::OptionGroup(const OptionGroup& rhs)
    : _name(rhs._name)
    {
        for(auto& opt : rhs._options) {
            opt->add(_argtable);
            _options.push_back(opt);
        }
        _argtable.emplace_back(_end = arg_end(20));
    }

    const std::string& OptionGroup::name() const
    {
        return _name;
    }

    void OptionGroup::usageSyntax(std::ostream& stream)
    {
        FILE* fp = acatl::open_ostream(stream);
        arg_print_syntax(fp, &_argtable[0], "");
        ::fclose(fp);
    }

    void OptionGroup::usageGlossary(std::ostream& stream)
    {
        stream << _name << ":\n";
        FILE* fp = acatl::open_ostream(stream);
        arg_print_glossary(fp, &_argtable[0], "  %-35s %s\n");
        ::fclose(fp);
    }

    bool OptionGroup::parse(const std::string& program, const acatl::StringVector& args, std::ostream& errorStream)
    {
        if(args.empty()) {
            return false;
        }

        if(arg_nullcheck(&_argtable[0]) != 0) {
            return false;
        }

        std::vector<const char*> argv;
        for(const auto& a : args) {
            argv.push_back(a.c_str());
        }

        int nerrors = arg_parse(static_cast<int>(args.size()), const_cast<char**>(&argv[0]), &_argtable[0]);
        if(nerrors > 0) {
            FILE* fp = acatl::open_ostream(errorStream);
            arg_print_errors(fp, _end, program.c_str());
            ::fclose(fp);

            return false;
        }

        return true;
    }

    int OptionGroup::count(const std::string& name) const
    {
        for(const auto& opt : _options) {
            if((opt->_hdr->longopts && opt->_hdr->longopts == name) || (opt->_hdr->shortopts && opt->_hdr->shortopts == name)) {
                return opt->count();
            }
        }
        return 0;
    }

    OptionWrapper OptionGroup::option(const std::string& name) const
    {
        for(const auto& opt : _options) {
            if((opt->_hdr->longopts && opt->_hdr->longopts == name) || (opt->_hdr->shortopts && opt->_hdr->shortopts == name)) {
                return OptionWrapper(*opt);
            }
        }
        ACATL_THROW(BadOption, "Option '" << name << "' not found");
    }

    int OptionGroup::untaggedOptionsCount() const
    {
        int count{ 0 };
        for(const auto& opt : _options) {
            if(!opt->_hdr->longopts && !opt->_hdr->shortopts) {
                ++count;
            }
        }
        return count;
    }

    int OptionGroup::count(int index) const
    {
        int tmp{ 0 };
        for(const auto& opt : _options) {
            if(!opt->_hdr->longopts && !opt->_hdr->shortopts && tmp++ == index) {
                return opt->count();
            }
        }
        return 0;
    }

    OptionWrapper OptionGroup::untaggedOption(int index) const
    {
        int tmp{ 0 };
        for(const auto& opt : _options) {
            if(!opt->_hdr->longopts && !opt->_hdr->shortopts && tmp++ == index) {
                return OptionWrapper(*opt);
            }
        }
        ACATL_THROW(BadOption, "Untagged option with index '" << index << "' not found");
    }


    CommandLineOptions::CommandLineOptions(const std::string& program, std::initializer_list<OptionGroup> groups)
    : _program(program)
    , _activeGroup(nullptr)
    {
        for(auto& group : groups) {
            _options.push_back(group);
        }
    }

    void CommandLineOptions::usage(std::ostream& stream) const
    {
        stream << "Usage: " << _program;
        if(_options.size() > 1) {
            stream << " [";
        }
        bool first{ true };
        for(auto& group : _options) {
            if(first) {
                first = false;
            } else {
                if(_options.size() > 1) {
                    stream << " ] | [";
                }
            }
            const_cast<OptionGroup&>(group).usageSyntax(stream);
        }
        if(_options.size() > 1) {
            stream << " ]\n";
        }
        for(auto& group : _options) {
            const_cast<OptionGroup&>(group).usageGlossary(stream);
            stream << "\n";
        }
    }

    bool CommandLineOptions::parse(const acatl::StringVector& args, std::ostream& errorStream)
    {
        _activeGroup = nullptr;

        if(args.empty()) {
            return false;
        }

        std::stringstream ss;
        for(auto& group : _options) {
            if(group.parse(_program, args, ss)) {
                _activeGroup = &group;
                break;
            }
        }
        if(!_activeGroup) {
            acatl::stream_copy(ss, errorStream);
        }

        return _activeGroup != nullptr;
    }

    int CommandLineOptions::count(const std::string& name) const
    {
        if(!_activeGroup) {
            ACATL_THROW(BadOption, "The commandline wasn't parsed or parsed with error");
        }
        return _activeGroup->count(name);
    }

    int CommandLineOptions::count(int index) const
    {
        if(!_activeGroup) {
            ACATL_THROW(BadOption, "The commandline wasn't parsed or parsed with error");
        }
        return _activeGroup->count(index);
    }

    int CommandLineOptions::untaggedOptionsCount() const
    {
        if(!_activeGroup) {
            ACATL_THROW(BadOption, "The commandline wasn't parsed or parsed with error");
        }
        return _activeGroup->untaggedOptionsCount();
    }

    OptionWrapper CommandLineOptions::option(const std::string& name) const
    {
        if(!_activeGroup) {
            ACATL_THROW(BadOption, "The commandline wasn't parsed or parsed with error");
        }
        return _activeGroup->option(name);
    }

    OptionWrapper CommandLineOptions::untaggedOption(int index) const
    {
        if(!_activeGroup) {
            ACATL_THROW(BadOption, "The commandline wasn't parsed or parsed with error");
        }
        return _activeGroup->untaggedOption(index);
    }

    const std::string& CommandLineOptions::groupName() const
    {
        if(!_activeGroup) {
            ACATL_THROW(BadOption, "The commandline wasn't parsed or parsed with error");
        }
        return _activeGroup->name();
    }
}    // namespace acatl
