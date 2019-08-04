add_executable(acatlnetworktest
    main.cpp

    http_cookie_test.cpp
    http_parser_test.cpp
    http_query_parameter_test.cpp
    http_session_manager_test.cpp
    http_types_test.cpp
    http_url_parser_test.cpp
)
target_link_libraries(acatlnetworktest PRIVATE ${ACATL_PLATFORM_LIBS} ${GTEST_LIBRARIES} acatl_network)

add_test(NAME acatlnetwork-unit-test COMMAND $<TARGET_FILE:acatlnetworktest>)
