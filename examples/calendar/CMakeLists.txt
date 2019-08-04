add_executable(calendar main.cpp)
target_include_directories(calendar SYSTEM PRIVATE "${date_SOURCE_DIR}/include")
target_link_libraries(calendar PRIVATE ${ACATL_PLATFORM_LIBS} acatl acatl_application)
