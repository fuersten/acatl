add_executable(acatlapplicationtest
    main.cpp

    application_test.cpp
    commandline_test.cpp
)
target_link_libraries(acatlapplicationtest PRIVATE ${ACATL_PLATFORM_LIBS} ${GTEST_LIBRARIES} acatl acatl_application)

add_test(NAME acatlapplication-unit-test COMMAND $<TARGET_FILE:acatlapplicationtest>)
