set(ACATL_MQTT_TEST_SOURCES
    main.cpp

    mqtt_connack_parser_test.cpp
    mqtt_connect_parser_test.cpp
    mqtt_fixed_header_parser_test.cpp
    mqtt_message_test.cpp
    mqtt_packet_identifier_parser_test.cpp
    mqtt_parser_test.cpp
    mqtt_processor_test.cpp
    mqtt_publish_parser_test.cpp
    mqtt_serializer_test.cpp
    mqtt_session_test.cpp
    mqtt_string_parser_test.cpp
    mqtt_suback_parser_test.cpp
    mqtt_subscribe_parser_test.cpp
    mqtt_subscription_tree_manager_test.cpp
    mqtt_subscription_tree_test.cpp
    mqtt_topic_filter_test.cpp
    mqtt_utils_test.cpp
)

add_executable(acatlmqtttest ${ACATL_MQTT_TEST_SOURCES})
target_include_directories(acatlmqtttest SYSTEM PRIVATE ${date_SOURCE_DIR}/include)
target_include_directories(acatlmqtttest SYSTEM PRIVATE ${nlohmann_SOURCE_DIR}/single_include/nlohmann)
target_link_libraries(acatlmqtttest ${ACATL_MQTT_PLATFORM_LIBS} ${GTEST_LIBRARIES} acatl acatl_mqtt)

add_test(NAME acatlmqtt-unit-test COMMAND $<TARGET_FILE:acatlmqtttest>)
