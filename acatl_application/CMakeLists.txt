set(ACATL_APPLICATION_SOURCES
    application.cpp
    command_line_options.cpp
    console.cpp

    application.h
    command_line_options.h
    console.h
)

set(LINENOISE_SOURCES
    ${linenoise_SOURCE_DIR}/include/linenoise.h
    ${linenoise_SOURCE_DIR}/src/linenoise.cpp
    ${linenoise_SOURCE_DIR}/src/ConvertUTF.h
    ${linenoise_SOURCE_DIR}/src/ConvertUTF.cpp
    ${linenoise_SOURCE_DIR}/src/wcwidth.cpp
)

if(NOT WIN32)
# loosen strict warning handling for external dependencies
set_property(
    SOURCE ${linenoise_SOURCE_DIR}/src/ConvertUTF.cpp
           ${linenoise_SOURCE_DIR}/src/linenoise.cpp
           ${linenoise_SOURCE_DIR}/src/wcwidth.cpp
    APPEND_STRING
    PROPERTY COMPILE_FLAGS
    "-Wno-all -Wno-error -Wno-conversion -Wno-missing-prototypes -Wno-pedantic"
)
endif()

set(ARGTABLE3_SOURCES
    ${argtable_SOURCE_DIR}/src/arg_end.c
    ${argtable_SOURCE_DIR}/src/arg_utils.c
    ${argtable_SOURCE_DIR}/src/arg_dstr.c
    ${argtable_SOURCE_DIR}/src/arg_str.c
    ${argtable_SOURCE_DIR}/src/arg_int.c
    ${argtable_SOURCE_DIR}/src/arg_lit.c
    ${argtable_SOURCE_DIR}/src/argtable3.c
    ${argtable_SOURCE_DIR}/src/argtable3.h
)

if(NOT WIN32)
# loosen strict warning handling for external dependencies
set_property(
    SOURCE ${argtable_SOURCE_DIR}/src/arg_end.c
           ${argtable_SOURCE_DIR}/src/arg_utils.c
           ${argtable_SOURCE_DIR}/src/arg_dstr.c
           ${argtable_SOURCE_DIR}/src/arg_str.c
           ${argtable_SOURCE_DIR}/src/arg_int.c
           ${argtable_SOURCE_DIR}/src/arg_lit.c
           ${argtable_SOURCE_DIR}/src/argtable3.c
           ${argtable_SOURCE_DIR}/src/argtable3.h
    APPEND_STRING
    PROPERTY COMPILE_FLAGS
    "-Wno-all -Wno-error -Wno-conversion -Wno-missing-prototypes -Wno-pedantic"
)
endif()

SOURCE_GROUP("Source Files\\detail" FILES ${LINENOISE_SOURCES} ${ARGTABLE3_SOURCES})

add_library(acatl_application SHARED ${ACATL_APPLICATION_SOURCES} ${LINENOISE_SOURCES} ${ARGTABLE3_SOURCES})
target_include_directories(acatl_application SYSTEM PRIVATE ${linenoise_SOURCE_DIR}/include)
target_include_directories(acatl_application SYSTEM PRIVATE ${argtable_SOURCE_DIR}/src)
set_target_properties(acatl_application PROPERTIES DEFINE_SYMBOL LIBACATL VERSION ${ACATL_VERSION} SOVERSION ${ACATL_VERSION_SHORT})
target_link_libraries(acatl_application PRIVATE ${ACATL_PLATFORM_LIBS} acatl)

install(TARGETS acatl_application LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY . DESTINATION include/acatl_application FILES_MATCHING PATTERN "*.h")
