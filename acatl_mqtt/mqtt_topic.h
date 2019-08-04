//
//  mqtt_topic.h
//  acatl_mqtt
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

#ifndef acatl_mqtt_topic_h
#define acatl_mqtt_topic_h

#include <acatl_mqtt/mqtt_types.h>


namespace acatl
{
    namespace mqtt
    {
        
        class TopicHierarchyIterator : public std::iterator<std::input_iterator_tag, const std::string>
        {
        public:
            TopicHierarchyIterator() noexcept
            {}
            
            TopicHierarchyIterator(const TopicHierarchyIterator& rhs)
            : _impl(std::move(rhs._impl))
            {}
            
            TopicHierarchyIterator(TopicHierarchyIterator&& rhs) noexcept
            : _impl(std::move(rhs._impl))
            {}
            
            ~TopicHierarchyIterator()
            {}
            
            TopicHierarchyIterator& operator=(const TopicHierarchyIterator& rhs)
            {
                _impl = std::move(rhs._impl);
                return *this;
            }
            
            TopicHierarchyIterator& operator=(TopicHierarchyIterator&& rhs) noexcept
            {
                _impl = std::move(rhs._impl);
                return *this;
            }
            
            reference operator*() const
            {
                return _impl->_nextTopic;
            }
            
            pointer operator->() const
            {
                return &(_impl->_nextTopic);
            }
            
            TopicHierarchyIterator& operator++()
            {
                if(!_impl->nextTopic()) {
                    _impl.reset();
                }
                return *this;
            }
            
            TopicHierarchyIterator& increment(std::error_code& ec) noexcept
            {
                if(!_impl->nextTopic()) {
                    _impl.reset();
                }
                return *this;
            }
            
            bool operator==(const TopicHierarchyIterator& other) const
            {
                if(this == &other) {
                    return true;
                }
                if(!_impl && !other._impl) {
                    return true;
                }
                if(!_impl || !other._impl) {
                    return false;
                }
                return *_impl == *(other._impl);
            }
            
            bool operator!=(const TopicHierarchyIterator& other) const
            {
                if(this == &other) {
                    return false;
                }
                if(!_impl && !other._impl) {
                    return false;
                }
                if(!_impl || !other._impl) {
                    return true;
                }
                return *_impl != *(other._impl);
            }
            
        private:
            friend struct TopicName;
            friend struct TopicFilter;
            
            explicit TopicHierarchyIterator(const std::string& topic)
            : _impl(new impl(topic))
            {}
            
            struct impl
            {
                impl(const std::string& topic)
                : _start(topic.cbegin())
                , _pos(topic.cbegin())
                , _end(topic.cend())
                {
                    nextTopic();
                }
                
                bool operator==(const impl& rhs)
                {
                    return _start == rhs._start;
                }
                
                bool operator!=(const impl& rhs)
                {
                    return _start != rhs._start;
                }
                
                bool nextTopic()
                {
                    if(_pos == _end) {
                        return false;
                    }
                    
                    if(_pos == _start && *_pos == '/') {
                        // the first topic level separator is a level without name
                        ++_pos;
                        _nextTopic = "";
                    } else {
                        std::string::const_iterator it = _pos;
                        while(_pos != _end && *_pos != '/') {
                            ++_pos;
                        }
                        _nextTopic = std::string(it, _pos);
                        if(*_pos == '/') {
                            ++_pos;
                        }
                    }
                    return true;
                }
                
                std::string _nextTopic;
                std::string::const_iterator _start;
                std::string::const_iterator _pos;
                std::string::const_iterator _end;
            };
            
            std::shared_ptr<impl> _impl;
        };
        
        struct TopicFilter
        {
            TopicFilter() = default;

            TopicFilter(const std::string& filter, QoSLevel qos = QoSLevel::AtMostOnce)
            : _filter(filter)
            , _qos(qos)
            {}
            
            bool operator==(const TopicFilter& rhs) const
            {
                return _filter == rhs._filter && _qos == rhs._qos;
            }
            
            bool operator!=(const TopicFilter& rhs) const
            {
                return _filter != rhs._filter || _qos != rhs._qos;
            }
            
            bool operator<(const TopicFilter& rhs) const
            {
                return _filter < rhs._filter || _qos < rhs._qos;
            }
            
            bool validate(std::error_code& ec)
            {
                std::string::value_type previous = '\0';
                
                for(auto iter = _filter.begin(); iter != _filter.end(); ) {
                    auto c = *iter;
                    ++iter;
                    if(c == '+' && ((previous != '\0' && previous != '/') || (iter != _filter.end() && *iter != '/'))) {
                        ec = mqtt_error::invalid_topic_filter;
                        return false;
                    } else if(c == '#') {
                        if((previous != '\0' && previous != '/') || iter != _filter.end()) {
                            ec = mqtt_error::invalid_topic_filter;
                            return false;
                        }
                    }
                    
                    previous = c;
                }
                return true;
            }
            
            TopicHierarchyIterator begin() const
            {
                return TopicHierarchyIterator(_filter);
            }
            
            TopicHierarchyIterator end() const
            {
                return TopicHierarchyIterator();
            }
            
            std::string _filter;
            QoSLevel _qos;
        };
        
        template<class CharT, class Traits>
        std::basic_ostream<CharT,Traits>&
        operator<<(std::basic_ostream<CharT,Traits>& os, const TopicFilter& filter)
        {
            os << std::quoted(filter._filter) << ", " << filter._qos;
            return os;
        }
        
        typedef std::vector<TopicFilter> TopicFilters;
        
        
        struct TopicFilterHelper
        {
            static TopicFilters findDifference(const TopicFilters& filtersToAdd, const TopicFilters& currentFilters)
            {
                TopicFilters result;
                
                std::set_difference(filtersToAdd.begin(), filtersToAdd.end(), currentFilters.begin(), currentFilters.end(), std::inserter(result, result.begin()));
                result.shrink_to_fit();
                return result;
            }
        };
        
        
        struct TopicName
        {
            TopicName() = default;
            
            TopicName(const std::string& name)
            : _name(name)
            {}
            
            TopicName(std::string&& name)
            : _name(name)
            {}
            
            TopicName& operator=(std::string&& rhs)
            {
                _name = std::move(rhs);
                return *this;
            }

            TopicName& operator=(const std::string& rhs)
            {
                _name = rhs;
                return *this;
            }
            
            bool validate(std::error_code& ec)
            {
                if(_name.find_first_of("+#") != std::string::npos) {
                    ec = mqtt_error::invalid_wildcard_in_topic;
                    return false;
                }
                return true;
            }
            
            TopicHierarchyIterator begin() const
            {
                return TopicHierarchyIterator(_name);
            }
            
            TopicHierarchyIterator end() const
            {
                return TopicHierarchyIterator();
            }
            
            std::string _name;
        };
        
        template<class CharT, class Traits>
        std::basic_ostream<CharT,Traits>&
        operator<<(std::basic_ostream<CharT,Traits>& os, const TopicName& topic)
        {
            os << std::quoted(topic._name);
            return os;
        }
        
    }
}

#endif
