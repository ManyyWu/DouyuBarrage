cmake_minimum_required(VERSION 3.10)
project(dy_barrage)

set(CMAKE_CXX_STANDARD 11)

include_directories(./dy_barrage_client
                    .)

aux_source_directory(dy_barrage_client SRC_DIR)
aux_source_directory(./ DY_BARRAGE_CLIENT)

add_executable(dy_barrage ${SRC_DIR} ${DY_BARRAGE_CLIENT})
