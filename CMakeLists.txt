cmake_minimum_required(VERSION 3.5)
project(ground_image)

set(CMAKE_CXX_FLAGS "-Wl,--no-undefined")

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rviz_common REQUIRED)
find_package(rviz_rendering REQUIRED)
find_package(rviz_default_plugins REQUIRED)
find_package(rviz_ogre_vendor REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core)


set(CMAKE_AUTOMOC ON)

set(ground_image_rviz_plugin_headers
        include/ground_image/ground_image_display.hpp
        )


foreach(header "${ground_image_rviz_plugin_headers}")
  qt5_wrap_cpp(ground_image_rviz_plugin_headers_moc_files "${header}")
endforeach()

set(ground_image_rviz_plugin_source_files
        src/ground_image_display.cpp
        src/utils/static_image_texture.cpp include/ground_image/utils/static_image_texture.hpp)

add_library(ground_image SHARED
        ${ground_image_rviz_plugin_source_files}
        ${ground_image_rviz_plugin_headers_moc_files}
        )

include_directories(
        include
)

target_link_libraries(ground_image PUBLIC
        rviz_ogre_vendor::OgreMain
        rviz_ogre_vendor::OgreOverlay
        rviz_common::rviz_common
        rviz_rendering::rviz_rendering
        rviz_default_plugins::rviz_default_plugins
        )

target_include_directories(ground_image PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
        ${Qt5Widgets_INCLUDE_DIRS}
        )

target_compile_definitions(ground_image PRIVATE "GROUND_IMAGE_BUILDING_LIBRARY")
target_compile_definitions(ground_image PUBLIC "PLUGINLIB__DISABLE_BOOST_FUNCTIONS")

ament_target_dependencies(${PROJECT_NAME}
        PUBLIC
        rclcpp
        rviz_common
        rviz_rendering
        )


pluginlib_export_plugin_description_file(rviz_common plugin_description.xml)

install(TARGETS ground_image
        EXPORT export_${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
        DESTINATION include/
)

install(FILES plugin_description.xml
        DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  ament_lint_auto_find_test_dependencies()
endif()

ament_export_libraries(ground_image)

ament_export_targets(export_${PROJECT_NAME})

ament_package()
