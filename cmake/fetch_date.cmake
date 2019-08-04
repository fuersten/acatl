FetchContent_Declare(
  date
  GIT_REPOSITORY https://github.com/HowardHinnant/date.git
  GIT_TAG        v2.4.1
)

FetchContent_GetProperties(date)
if(NOT date_POPULATED)
  FetchContent_Populate(date)
endif()

