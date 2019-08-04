FetchContent_Declare(
  nlohmann
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG        v3.6.1
)

FetchContent_GetProperties(nlohmann)
if(NOT nlohmann_POPULATED)
  FetchContent_Populate(nlohmann)
endif()

