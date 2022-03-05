cmake_minimum_required(VERSION 3.21)
project(clx_imgui)

set(SOURCE_FILES clx_imgui.hpp clx_imgui.cpp clx_imgui_opengl.hpp clx_imgui_opengl.cpp clx_imgui_vulkan.hpp clx_imgui_vulkan.cpp imgui_build_common.cpp imgui_build_opengl.cpp imgui_build_vulkan.cpp)
source_group("src" FILES ${SOURCE_FILES})

add_library(${PROJECT_NAME} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} PRIVATE calcium)

find_package(Vulkan)
if(Vulkan_FOUND)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARY})
  target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIR})
  target_compile_definitions(${PROJECT_NAME} PUBLIC CALCIUM_VULKAN_SDK_FOUND=1)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "depend/imgui")
