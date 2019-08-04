configure_file(test_helper.h.in ${CMAKE_CURRENT_BINARY_DIR}/test_helper.h)
add_executable(acatltest
    main.cpp

    any_test.cpp
    date_test.cpp
    exception_test.cpp
    filesystem_test.cpp
    glob_test.cpp
    lexer_test.cpp
    logging_test.cpp
    numeric_cast_test.cpp
    stream_wrapper_test.cpp
    string_helper_test.cpp
    thread_pool_test.cpp
    time_test.cpp
    timestamp_test.cpp
    tribool_test.cpp
    type_test.cpp
    uuid_test.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/test_helper.h
)
target_include_directories(acatltest SYSTEM PRIVATE "${date_SOURCE_DIR}/include")
target_include_directories(acatltest SYSTEM PRIVATE ${nlohmann_SOURCE_DIR}/single_include/nlohmann)
target_link_libraries(acatltest PRIVATE ${ACATL_PLATFORM_LIBS} ${GTEST_LIBRARIES} acatl)

add_test(NAME acatl-unit-test COMMAND $<TARGET_FILE:acatltest>)
