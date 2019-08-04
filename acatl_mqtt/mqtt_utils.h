//
//  mqtt_utils.h
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

#ifndef acatl_mqtt_utils_h
#define acatl_mqtt_utils_h

#include <acatl_mqtt/mqtt_error.h>

#include <tuple>


namespace acatl
{
    namespace mqtt
    {

        class RunlengthDecoder
        {
        public:
            RunlengthDecoder()
            : _multiplier(1)
            , _value(0)
            {}
            
            void reset()
            {
                _multiplier = 1;
                _value = 0;
            }
            
            std::tuple<int, uint32_t> decode(uint8_t encodedByte, std::error_code& ec)
            {
                _value += (encodedByte & 127) * _multiplier;
                if(_multiplier > 128*128*128) {
                    ec = acatl::mqtt::mqtt_error::malformed_remaining_length;
                    return std::make_tuple(-1, 0);
                }
                _multiplier *= 128;
                if((encodedByte & 128) != 0) {
                    return std::make_tuple(0, _value);
                }
                return std::make_tuple(1, _value);
            }
            
        private:
            uint32_t _multiplier;
            uint32_t _value;
        };
        
        
        class RunlengthEncoder
        {
        public:
            void encode(uint32_t value, std::vector<uint8_t>& buffer, size_t& index)
            {
                uint8_t encodedByte = 0;
                
                do {
                    encodedByte = value % 128;
                    value = value / 128;
                    // if there are more data to encode, set the top bit of this byte
                    if(value > 0) {
                        encodedByte = encodedByte | 128;
                    }
                    buffer[index++] = encodedByte;
                } while (value > 0);
            }
        };
        
        
        class StringEncoder
        {
        public:
            void encode(const std::string& value, std::vector<uint8_t>& buffer, size_t& index)
            {
                uint16_t length = static_cast<uint16_t>(value.size());
                buffer[index++] = (length & 0xFF00) >> 8;
                buffer[index++] = (length & 0x00FF);
                for(const auto& c : value) {
                    buffer[index++] = static_cast<uint8_t>(c);
                }
            }
        };

    }
}

#endif 
