//
//  mqtt_string_parser.h
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

#ifndef acatl_mqtt_string_parser_h
#define acatl_mqtt_string_parser_h

#include <acatl_mqtt/mqtt_error.h>

#include <acatl/tribool.h>


namespace acatl
{
    namespace mqtt
    {
        
        class StringParser
        {
        public:
            enum class Status
            {
                Start,
                ReadLength,
                Stringdata,
                Ready
            };

            StringParser()
            : _status(Status::Start)
            {}
            
            void reset()
            {
                _status = Status::Start;
                _length = 0;
                _ss.str("");
                _ss.clear();
            }
            
            acatl::Tribool parse(uint8_t byte, std::error_code& ec)
            {
                switch(_status) {
                    case Status::Start:
                        _length = static_cast<uint32_t>(byte << 8);
                        _status = Status::ReadLength;
                        break;
                    case Status::ReadLength:
                        _length |= static_cast<uint32_t>(byte);
                        if(_length == 0) {
                            _status = Status::Ready;
                            return acatl::Tribool(true);
                        } else {
                            _status = Status::Stringdata;
                        }
                        break;
                    case Status::Stringdata:
                        _ss << static_cast<char>(byte);
                        --_length;
                        if(_length == 0) {
                            _status = Status::Ready;
                            return acatl::Tribool(true);
                        }
                        break;
                    case Status::Ready:
                        ec = mqtt_error::string_length_violation;
                        return acatl::Tribool(false);
                }
                
                return acatl::Tribool();
            }
            
            std::string string() const { return _ss.str(); }

        private:
            Status _status;
            uint32_t _length;
            std::stringstream _ss;
        };
        
    }
}

#endif
