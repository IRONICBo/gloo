find_package(OpenSSL 1.1 REQUIRED EXACT)

set(GLOO_TCP_BENCHMARK_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/tcp_benchmark.cc"
)

add_executable(tcp_benchmark ${GLOO_TCP_BENCHMARK_SRCS})

target_link_libraries(tcp_benchmark gloo)

if(GLOO_INSTALL)
  install(TARGETS tcp_benchmark DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()
