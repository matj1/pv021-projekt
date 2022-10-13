cmake_minimum_required(VERSION 3.16)
project(pv021_projekt C)

set(CMAKE_C_STANDARD 11)

add_executable(pv021_projekt
        src/neuronka.c
        src/input.c)

target_link_libraries(${PROJECT_NAME} m)
