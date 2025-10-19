cmake_minimum_required(VERSION 3.12)
project(linux_pad CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("[INFO] install dir: ${CMAKE_INSTALL_PREFIX}")

# ヘッダファイルの変数定義
set(PAD_HEADERS pad.hpp padbase.hpp)
set(PS5_HEADERS ps5/ps5pad.hpp)
set(PROCON_HEADERS nintendo/procon.hpp)

# 各種コントローラも含めて全てのヘッダファイルを1つの変数にする
set(ALL_HEADERS 
  ${PAD_HEADERS} 
  ${PS5_HEADERS} 
  ${PROCON_HEADERS}
)
message("[INFO] All header: ${ALL_HEADERS}")
# ヘッダファイルの install
install(FILES ${ALL_HEADERS} DESTINATION include/pad)

# ソースファイルの変数定義
set(PAD_SRCS pad.hpp padbase.cpp)
set(PS5_SRCS ps5/ps5pad.cpp)
set(PROCON_SRCS nintendo/procon.cpp)

set(ALL_SRCS
  ${PAD_SRCS}
  ${PS5_SRCS}
  ${PROCON_SRCS}
)

add_library(gamepad SHARED ${ALL_SRCS})
install(TARGET gamepad DESTINATION lib)

# ps5 コントローラの処理を共有ライブラリにして install
set(PS5_SRCS padbase.cpp ps5/ps5pad.cpp)
add_library(ps5 SHARED ${PS5_SRCS})
install(TARGETS ps5 DESTINATION lib)

# nintendo プロコンの処理を共有ライブラリにして install
set(PS5_SRCS padbase.cpp nintendo/procon.cpp)
add_library(procon SHARED ${PS5_SRCS})
install(TARGETS procon DESTINATION lib)