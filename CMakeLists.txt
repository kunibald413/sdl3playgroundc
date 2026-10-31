cmake_minimum_required(VERSION 3.10)

project(MyCProject C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- SDL3 ----
set(SDL3_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3/cmake)
find_package(SDL3 REQUIRED)

# ---- SDL3_image ----
set(SDL3_image_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3_image/cmake)
find_package(SDL3_image REQUIRED CONFIG)

# ---- SDL3_ttf ----
set(SDL3_ttf_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3_ttf/cmake)
find_package(SDL3_ttf REQUIRED CONFIG)

# ---- SDL3_mixer ----
set(SDL3_mixer_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL3_mixer/cmake)
find_package(SDL3_mixer REQUIRED CONFIG)

add_executable(hello src/main.c)

# Copy DLLs after building (Windows)
add_custom_command(TARGET hello POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:hello>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE_DIR:hello>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE_DIR:hello>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
        $<TARGET_FILE_DIR:hello>
    COMMENT "Copying SDL3, SDL3_image, SDL3_ttf and SDL3_mixer DLLs"
)

target_link_libraries(hello PRIVATE 
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
)
