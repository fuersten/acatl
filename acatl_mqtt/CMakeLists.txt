set(LIBACATL_MQTT_SOURCES
    mqtt_connack_parser.h
    mqtt_connect_parser.h
    mqtt_control_packets.h
    mqtt_error.h
    mqtt_fixed_header_parser.h
    mqtt_packet_identifier_parser.h
    mqtt_packet_sender.h
    mqtt_parser.h
    mqtt_processor.h
    mqtt_publish_parser.h
    mqtt_serializer.h
    mqtt_session_manager.h
    mqtt_session_store.h
    mqtt_session.h
    mqtt_string_parser.h
    mqtt_suback_parser.h
    mqtt_subscription_handler.h
    mqtt_subscribe_parser.h
    mqtt_subscription_tree_manager.h
    mqtt_subscription_tree.h
    mqtt_topic.h
    mqtt_types.h
    mqtt_utils.h
)

add_library(acatl_mqtt SHARED ${LIBACATL_MQTT_SOURCES})
target_include_directories(acatl_mqtt SYSTEM PRIVATE ${nlohmann_SOURCE_DIR}/single_include/nlohmann)
set_target_properties(acatl_mqtt PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(acatl_mqtt PROPERTIES DEFINE_SYMBOL LIBACATL VERSION ${ACATL_VERSION} SOVERSION ${ACATL_VERSION_SHORT})
target_link_libraries(acatl_mqtt ${ACATL_PLATFORM_LIBS} acatl)

install(TARGETS acatl_mqtt LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
