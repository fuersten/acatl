//
//  mqtt_fixed_header_parser.h
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

#ifndef acatl_mqtt_fixed_header_parser_h
#define acatl_mqtt_fixed_header_parser_h

#include <acatl_mqtt/mqtt_control_packets.h>
#include <acatl_mqtt/mqtt_utils.h>

#include <acatl/tribool.h>


namespace acatl
{
    namespace mqtt
    {
        
        class FixedHeaderParser
        {
        public:
            enum class Status
            {
                Start,
                ReadLength,
                Ready
            };
            
            FixedHeaderParser()
            : _status(Status::Start)
            {}
            
            void reset()
            {
                _status = Status::Start;
                _header = FixedHeader();
                _decoder.reset();
            }
            
            acatl::Tribool parse(uint8_t byte, std::error_code& ec)
            {
                switch(_status) {
                    case Status::Start:
                        _header._controlPacketType = acatl::mqtt::ControlPacketType(byte & 0xF0);
                        _header._flags = byte & 0x0F;
                        if(_header.validate(ec)) {
                            _status = Status::ReadLength;
                        } else {
                            _header = FixedHeader();
                            _status = Status::Ready;
                            return acatl::Tribool(false);
                        }
                        break;
                    case Status::ReadLength: {
                        std::tuple<int, uint32_t> ret = _decoder.decode(byte, ec);
                        switch(std::get<0>(ret)) {
                            case 0:
                                // not ready, continue decoding
                                return acatl::Tribool();
                            case -1:
                                // error, stop processing
                                _status = Status::Ready;
                                return acatl::Tribool(false);
                            case 1:
                                // ready
                                _status = Status::Ready;
                                _header._length = std::get<1>(ret);
                                return acatl::Tribool(true);
                        }
                        break;
                    }
                    case Status::Ready:
                        return acatl::Tribool(true);
                        break;
                }
                
                return acatl::Tribool();
            }
            
            const FixedHeader& header() const
            {
                return _header;
            }
            
        private:
            Status _status;
            FixedHeader _header;
            acatl::mqtt::RunlengthDecoder _decoder;
        };
        
    }
}

#endif
