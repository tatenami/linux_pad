cmake_minimum_required(VERSION 3.12)
project(linux_pad CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("[INFO] install dir: ${CMAKE_INSTALL_PREFIX}")

# ヘッダファイルの変数定義
set(PAD_HEADERS 
  event_types.hpp
  event_reader.hpp
  event_handler.hpp
  gamepad.hpp
)

set(PS5_HEADER ps5/ps5.hpp)
set(PS5_IMPL ps5/impl/ps5_impl.ipp)
set(PROCON_HEADER nintendo/procon.hpp)
set(PROCON_IMPL nintendo/impl/procon_impl.ipp)

# ソースファイルの変数定義
set(PAD_SRCS 
  event_reader.cpp
  event_handler.cpp
  gamepad.cpp
)

add_library(gamepad SHARED ${PAD_SRCS})
target_include_directories(gamepad PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
install(FILES ${PAD_HEADERS} DESTINATION include/pad)
install(FILES ${PS5_HEADER} DESTINATION include/pad)
install(FILES ${PS5_IMPL} DESTINATION include/pad/impl)
install(FILES ${PROCON_HEADER} DESTINATION include/pad)
install(FILES ${PROCON_IMPL} DESTINATION include/pad/impl)
install(TARGETS gamepad DESTINATION lib)
