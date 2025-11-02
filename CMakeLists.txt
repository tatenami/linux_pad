cmake_minimum_required(VERSION 3.12)
project(linux_pad CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("[INFO] install dir: ${CMAKE_INSTALL_PREFIX}")

# ヘッダファイルの変数定義
set(PAD_HEADERS gamepad.hpp)
set(PS5_HEADERS ps5/ps5pad.hpp)
set(PROCON_HEADERS nintendo/procon.hpp)

# 各種コントローラも含めて全てのヘッダファイルを1つの変数にする
set(ALL_HEADERS 
  ${PAD_HEADERS} 
  ${PS5_HEADERS} 
  ${PROCON_HEADERS}
)

# ソースファイルの変数定義
set(PAD_SRCS gamepad.cpp)
set(PS5_SRCS ps5/ps5pad.cpp)
set(PROCON_SRCS nintendo/procon.cpp)

set(ALL_SRCS
  ${PAD_SRCS}
  ${PS5_SRCS}
  ${PROCON_SRCS}
)

add_library(gamepad SHARED ${ALL_SRCS})
target_include_directories(gamepad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(FILES ${ALL_HEADERS} DESTINATION include/pad)
install(TARGETS gamepad DESTINATION lib)
