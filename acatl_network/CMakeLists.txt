set(ACATL_NETWORK_DETAIL_SOURCES
    ${http_parser_SOURCE_DIR}/http_parser.c
    ${http_parser_SOURCE_DIR}/http_parser.h
    detail/posix/network_helper.cpp
)

if(NOT WIN32)
# loosen strict warning handling for external dependencies
set_property(
    SOURCE ${http_parser_SOURCE_DIR}/http_parser.c
           ${http_parser_SOURCE_DIR}/http_parser.h
    APPEND_STRING
    PROPERTY COMPILE_FLAGS
    "-Wno-all -Wno-error -Wno-conversion -Wno-missing-prototypes -Wno-pedantic"
)
endif()

source_group("Source Files\\detail" FILES ${ACATL_NETWORK_DETAIL_SOURCES})

add_library(acatl_network SHARED
    http_cookie.cpp
    http_errors.cpp
    http_parser.cpp
    http_query_parameter.cpp
    http_session_manager.cpp
    http_session.cpp
    http_types.cpp
    http_url_parser.cpp

    async_server.h
    http_cookie.h
    http_errors.h
    http_parser.h
    http_query_parameter.h
    http_session_manager.h
    http_session.h
    http_types.h
    http_url_parser.h
    io_context_pool.h
    network_helper.h
    socket_type.h
    ${ACATL_NETWORK_DETAIL_SOURCES}
)
target_include_directories(acatl_network SYSTEM PRIVATE "${http_parser_SOURCE_DIR}/")
target_include_directories(acatl_network SYSTEM PRIVATE "${asio_SOURCE_DIR}/")
set_target_properties(acatl_network PROPERTIES VERSION ${ACATL_VERSION} SOVERSION ${ACATL_VERSION_SHORT})
target_link_libraries(acatl_network PRIVATE ${ACATL_PLATFORM_LIBS} ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} acatl)

install(TARGETS acatl_network LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY . DESTINATION include/acatl_network FILES_MATCHING PATTERN "*.h")
install(DIRECTORY . DESTINATION include/acatl_network FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY . DESTINATION include/acatl_network FILES_MATCHING PATTERN "*.ipp")
