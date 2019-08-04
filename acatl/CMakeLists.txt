configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/acatl/version.h)

if(NOT APPLE AND UNIX)
    set(ACATL_DETAIL_SOURCES
        detail/posix/filesystem.cpp
        detail/posix/glob.cpp
        detail/posix/uuid.cpp
        detail/linux/stream_wrapper.cpp
    )
elseif(APPLE)
    set(ACATL_DETAIL_SOURCES
        detail/posix/filesystem.cpp
        detail/posix/glob.cpp
        detail/posix/uuid.cpp
        detail/bsd/stream_wrapper.cpp
    )
endif()

source_group("Source Files\\detail" FILES ${ACATL_DETAIL_SOURCES})

add_library(acatl SHARED
    exception.cpp
    filesystem.cpp
    float_helper.cpp
    logging.cpp
    string_helper.cpp
    types.cpp

    any.h
    date_helper.h
    exception.h
    filesystem.h
    float_helper.h
    glob.h
    hash_helper.h
    inc.h
    json.h
    lexer.cpp
    lexer.h
    logging.h
    numeric_cast.h
    stream_wrapper.h
    string_helper.h
    thread_pool.h
    timestamp.h
    tribool.h
    types.h
    uuid.h
    ${ACATL_DETAIL_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/acatl/version.h
)

target_include_directories(acatl SYSTEM PRIVATE ${date_SOURCE_DIR}/include)
target_include_directories(acatl SYSTEM PRIVATE ${nlohmann_SOURCE_DIR}/single_include/nlohmann)
set_target_properties(acatl PROPERTIES DEFINE_SYMBOL LIBACATL VERSION ${ACATL_VERSION} SOVERSION ${ACATL_VERSION_SHORT})
target_link_libraries(acatl PRIVATE ${ACATL_PLATFORM_LIBS})

install(TARGETS acatl LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY . DESTINATION include/acatl FILES_MATCHING PATTERN "*.h")
install(DIRECTORY . DESTINATION include/acatl FILES_MATCHING PATTERN "*.hpp")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/acatl/version.h" DESTINATION include/acatl)
