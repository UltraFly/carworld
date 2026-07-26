cmake_minimum_required(VERSION 3.25)

project(
    CarWorld
    VERSION 0.247.0
    DESCRIPTION "Open-source driving simulator and vehicle dynamics experiment"
    LANGUAGES CXX
)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_net CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

if(NOT TARGET OpenGL::GLU)
    message(FATAL_ERROR "CarWorld requires the OpenGL Utility Library (GLU).")
endif()

set(CARWORLD_SOURCES
    src/CarWorld.cpp
    src/CarWorldClasses.cpp
    src/CarWorldClient.cpp
    src/CarWorldMain.cpp
    src/CarWorldNet.cpp
    src/CarWorldServer.cpp
    src/CWCamera.cpp
    src/CWVehicle.cpp
    src/H_Geometry.cpp
    src/H_Graphics.cpp
    src/H_Input.cpp
    src/H_Keys.cpp
    src/H_Object.cpp
    src/H_Prompt.cpp
    src/H_Standard.cpp
    src/H_Texture.cpp
    src/H_Variable.cpp
    src/H_XML.cpp
    src/SDLJoystick.cpp
    src/SDLMain.cpp
    src/Tacho.cpp
    src/WorldBlock.cpp
)

if(WIN32)
    add_executable(carworld WIN32 ${CARWORLD_SOURCES} src/res/carworld.rc)
else()
    add_executable(carworld ${CARWORLD_SOURCES})
endif()

target_compile_features(carworld PRIVATE cxx_std_17)
target_include_directories(carworld PRIVATE src)

target_link_libraries(
    carworld
    PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>
        OpenGL::GL
        OpenGL::GLU
)

if(MSVC)
    target_compile_definitions(carworld PRIVATE WIN32 _CRT_SECURE_NO_WARNINGS)
    target_compile_options(carworld PRIVATE /W3 /EHsc)
    target_link_options(
        carworld
        PRIVATE
            "$<$<CONFIG:Debug>:/DEBUG>"
            "$<$<CONFIG:RelWithDebInfo>:/DEBUG>"
    )
    set_property(
        TARGET carworld
        PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
else()
    target_compile_options(carworld PRIVATE -Wall -Wextra)
endif()

add_custom_command(
    TARGET carworld
    POST_BUILD
    COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:carworld>/data"
    COMMENT "Copying CarWorld runtime data"
)

install(TARGETS carworld RUNTIME DESTINATION . BUNDLE DESTINATION .)
install(DIRECTORY data DESTINATION .)
