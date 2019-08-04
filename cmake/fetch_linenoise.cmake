FetchContent_Declare(
  linenoise
  GIT_REPOSITORY https://github.com/arangodb/linenoise-ng
  GIT_TAG        v1.0.1
)

FetchContent_GetProperties(linenoise)
if(NOT linenoise_POPULATED)
  FetchContent_Populate(linenoise)
endif()

