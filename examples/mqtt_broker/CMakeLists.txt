add_executable(mqtt_broker
    main.cpp

    connection.h
)

target_include_directories(mqtt_broker SYSTEM PRIVATE "${asio_SOURCE_DIR}/include")
target_include_directories(mqtt_broker SYSTEM PRIVATE "${date_SOURCE_DIR}/include")
target_include_directories(mqtt_broker SYSTEM PRIVATE ${nlohmann_SOURCE_DIR}/single_include/nlohmann)
target_compile_definitions(mqtt_broker PRIVATE -DASIO_STANDALONE)
target_link_libraries(mqtt_broker ${ACATL_PLATFORM_LIBS} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} acatl acatl_application acatl_network acatl_mqtt)

