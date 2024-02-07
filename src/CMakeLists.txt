# Generic test that uses conan libs

file(GLOB SRCS *.cpp)
file(GLOB HEADER_FILES *.h)

find_package(Boost REQUIRED)

add_executable(pts ${SRCS} ${HEADER_FILES})
target_link_libraries(
  pts PRIVATE
  #project_options
  #project_warnings
  CLI11::CLI11
  spdlog::spdlog
  ${Boost_LIBRARIES}
)

target_include_directories(pts PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
