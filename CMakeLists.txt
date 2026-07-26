cmake_minimum_required(VERSION 3.25)

project(
    CarWorld
    VERSION 0.247.0
    DESCRIPTION "Open-source driving simulator and vehicle dynamics experiment"
    LANGUAGES CXX
)

option(
    CARWORLD_ENABLE_NETWORKING
    "Build CarWorld networking support with SDL3_net"
    ON
)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
if(CARWORLD_ENABLE_NETWORKING)
    find_package(SDL3_net CONFIG REQUIRED)
endif()
find_package(OpenGL REQUIRED)

if(NOT TARGET OpenGL::GLU)
    message(FATAL_ERROR "CarWorld requires the OpenGL Utility Library (GLU).")
endif()

set(CARWORLD_SOURCES
    src/CarWorld.cpp
    src/CarWorldClasses.cpp
    src/CarWorldClient.cpp
    src/CarWorldMain.cpp
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

if(CARWORLD_ENABLE_NETWORKING)
    list(APPEND CARWORLD_SOURCES
        src/CarWorldNet.cpp
        src/CarWorldServer.cpp
    )
endif()

if(WIN32)
    add_executable(carworld WIN32 ${CARWORLD_SOURCES} src/res/carworld.rc)
else()
    add_executable(carworld ${CARWORLD_SOURCES})
endif()

target_compile_features(carworld PRIVATE cxx_std_17)
target_include_directories(carworld PRIVATE src)
target_compile_definitions(
    carworld
    PRIVATE
        CARWORLD_ENABLE_NETWORKING=$<BOOL:${CARWORLD_ENABLE_NETWORKING}>
)

target_link_libraries(
    carworld
    PRIVATE
        SDL3::SDL3
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image>,SDL3_image::SDL3_image,SDL3_image::SDL3_image-static>
        OpenGL::GL
        OpenGL::GLU
)

if(CARWORLD_ENABLE_NETWORKING)
    target_link_libraries(
        carworld
        PRIVATE
            $<IF:$<TARGET_EXISTS:SDL3_net::SDL3_net>,SDL3_net::SDL3_net,SDL3_net::SDL3_net-static>
    )
endif()

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
if(WIN32)
    install(FILES
        "$<TARGET_FILE_DIR:carworld>/SDL3.dll"
        "$<TARGET_FILE_DIR:carworld>/SDL3_image.dll"
        "$<TARGET_FILE_DIR:carworld>/libpng16.dll"
        "$<TARGET_FILE_DIR:carworld>/z.dll"
        DESTINATION .
    )
    if(CARWORLD_ENABLE_NETWORKING)
        install(FILES
            "$<TARGET_FILE_DIR:carworld>/SDL3_net.dll"
            DESTINATION .
        )
    endif()
endif()
install(DIRECTORY data DESTINATION .)
install(FILES README.md CHANGELOG.md LICENSE DESTINATION .)

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_NAME "carworld")
    set(CPACK_PACKAGE_VENDOR "Marcus Hewat")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
    set(CPACK_PACKAGE_FILE_NAME "carworld-${PROJECT_VERSION}-windows-x64")
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
    include(CPack)
endif()
