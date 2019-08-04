add_executable(http_server
    main.cpp

    connection.h
    mime_types.h
    types.h
)

target_include_directories(http_server SYSTEM PRIVATE "${asio_SOURCE_DIR}/include")
target_include_directories(http_server SYSTEM PRIVATE "${date_SOURCE_DIR}/include")
target_include_directories(http_server SYSTEM PRIVATE ${nlohmann_SOURCE_DIR}/single_include/nlohmann)
target_compile_definitions(http_server PRIVATE -DASIO_STANDALONE)
target_link_libraries(http_server PRIVATE ${ACATL_PLATFORM_LIBS} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} acatl acatl_application acatl_network)
