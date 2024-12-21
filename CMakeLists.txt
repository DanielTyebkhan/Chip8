cmake_minimum_required(VERSION 3.28)
project(emulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} src/main.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_WARNING_AS_ERROR ON)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_sources(
    ${PROJECT_NAME} PRIVATE
        src/Emulator.cpp 
        src/InstructionError.cpp 
        src/Screen.cpp
        src/SoundTimer.cpp
        src/Random.cpp
        src/Keyboard.cpp
        src/Interpreter.cpp
        src/UI.cpp
        src/DelayTimer.cpp
)

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
