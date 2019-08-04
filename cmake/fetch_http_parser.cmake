FetchContent_Declare(
  http_parser
  GIT_REPOSITORY https://github.com/nodejs/http-parser
  GIT_TAG        v2.9.2
)

FetchContent_GetProperties(http_parser)
if(NOT http_parser_POPULATED)
  FetchContent_Populate(http_parser)
endif()

