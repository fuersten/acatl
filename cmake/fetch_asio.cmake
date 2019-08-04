FetchContent_Declare(
  asio
  URL https://downloads.sourceforge.net/project/asio/asio/1.12.2%20%28Stable%29/asio-1.12.2.tar.gz?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fasio%2Ffiles%2Flatest%2Fdownload&ts=1564927130
)

FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
  FetchContent_Populate(asio)
endif()

