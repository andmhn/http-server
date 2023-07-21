cmake_minimum_required(VERSION 3.5.1)
project(http-server VERSION 0.1.0
		DESCRIPTION "HTTP server from scrach"
                    LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BIN_NAME http-server)

file(GLOB SRC src/*.c)

add_executable(${BIN_NAME} ${SRC})
file(COPY res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
