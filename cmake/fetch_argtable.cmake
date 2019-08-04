FetchContent_Declare(
  argtable
  GIT_REPOSITORY https://github.com/argtable/argtable3
  GIT_TAG        v3.1.2.bb37058
)

FetchContent_GetProperties(argtable)
if(NOT argtable_POPULATED)
  FetchContent_Populate(argtable)
endif()

