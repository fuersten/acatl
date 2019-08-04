//
//  mqtt_error.h
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

#ifndef acatl_mqtt_errors_hpp
#define acatl_mqtt_errors_hpp

#include <string>
#include <system_error>


namespace acatl
{
    namespace mqtt
    {
        
        enum class mqtt_error
        {
            invalid_control_packet_type = 1,
            malformed_remaining_length = 2,
            string_length_violation = 3,
            duplicate_connect_protocol_violation = 4,
            connect_protocol_violation = 5,
            protocol_name_violation = 6,
            unacceptable_protocol_level = 7,
            connect_flag_protocol_violation = 8,
            will_message_protocol_violation = 9,
            authorization_protocol_violation = 10,
            control_packet_length = 11,
            malformed_control_packet = 12,
            packet_identifier_length_violation = 13,
            subscribe_protocol_violation = 14,
            invalid_topic_filter = 15,
            connect_acknowledge_flag_violation = 16,
            invalid_connect_return_code = 17,
            feature_not_implemented = 18,
            control_packet_not_allowed = 19,
            dup_flag_violation = 20,
            invalid_qos_level = 21,
            publish_protocol_violation = 22,
            not_connected = 23,
            session_in_use = 24,
            session_not_found = 25,
            no_packet_sender = 26,
            invalid_wildcard_in_topic = 27,
            clean_session_not_set_for_empty_client_id = 28
        };
        
        class mqtt_error_category_t : public std::error_category
        {
        public:
            virtual const char* name() const noexcept
            {
                return "mqtt_error";
            }
            
            virtual std::string message(int ev) const
            {
                switch(mqtt_error(ev)) {
                    case mqtt_error::invalid_control_packet_type:
                        return "Invalid control packet type";
                    case mqtt_error::malformed_remaining_length:
                        return "Malformed Remaining Length";
                    case mqtt_error::string_length_violation:
                        return "String length violation";
                    case mqtt_error::duplicate_connect_protocol_violation:
                        return "Duplicate connect protocol violation";
                    case mqtt_error::connect_protocol_violation:
                        return "Connect protocol violation";
                    case mqtt_error::protocol_name_violation:
                        return "Protocol name violation";
                    case mqtt_error::unacceptable_protocol_level:
                        return "Unacceptable protocol level";
                    case mqtt_error::connect_flag_protocol_violation:
                        return "Connect flag not set to zero";
                    case mqtt_error::will_message_protocol_violation:
                        return "Will message protocol violation";
                    case mqtt_error::authorization_protocol_violation:
                        return "Authorization protocol violation";
                    case mqtt_error::control_packet_length:
                        return "Control packet length error";
                    case mqtt_error::malformed_control_packet:
                        return "Malformed control packet";
                    case mqtt_error::packet_identifier_length_violation:
                        return "Packet identifier length violation";
                    case mqtt_error::subscribe_protocol_violation:
                        return "Subscribe protocol violation";
                    case mqtt_error::invalid_topic_filter:
                        return "Invalid topic filter";
                    case mqtt_error::connect_acknowledge_flag_violation:
                        return "Connect acknowledge flag violation";
                    case mqtt_error::invalid_connect_return_code:
                        return "Invalid connect return code";
                    case mqtt_error::feature_not_implemented:
                        return "Feature not implemented";
                    case mqtt_error::control_packet_not_allowed:
                        return "Control packet not allowed";
                    case mqtt_error::dup_flag_violation:
                        return "Dup flag violation";
                    case mqtt_error::invalid_qos_level:
                        return "Invalid QoS level";
                    case mqtt_error::publish_protocol_violation:
                        return "Publish protocol violation";
                    case mqtt_error::not_connected:
                        return "Not connected";
                    case mqtt_error::session_in_use:
                        return "Session is in use for client id";
                    case mqtt_error::session_not_found:
                        return "Cannot find session for client id";
                    case mqtt_error::no_packet_sender:
                        return "No packet sender";
                    case mqtt_error::invalid_wildcard_in_topic:
                        return "Invalid wildcard in topic name";
                    case mqtt_error::clean_session_not_set_for_empty_client_id:
                        return "Clean session not set for empty client id";
                    default:
                        throw std::runtime_error("unknown error code");
                }
            }
        };
        
        static const std::error_category& mqtt_error_category()
        {
            static mqtt_error_category_t instance;
            return instance;
        }
        
        inline std::error_code make_error_code(acatl::mqtt::mqtt_error e)
        {
            const std::error_category& cat = acatl::mqtt::mqtt_error_category();
            return std::error_code(static_cast<int>(e), cat);
        }
    }
}

namespace std
{
    template<>
    struct is_error_code_enum<acatl::mqtt::mqtt_error> : public true_type {};
}

#endif
