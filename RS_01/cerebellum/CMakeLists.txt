cmake_minimum_required(VERSION 3.16)
project(rushi_cerebellum VERSION 0.1.0)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找ROS2包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# 查找websocketpp（需要手动安装或使用vcpkg）
find_path(WEBSOCKETPP_INCLUDE_DIR websocketpp/config/asio_client.hpp
    PATHS
    /usr/include
    /usr/local/include
    ${CMAKE_SOURCE_DIR}/third_party/websocketpp
)

find_path(ASIO_INCLUDE_DIR asio.hpp
    PATHS
    /usr/include
    /usr/local/include
    ${CMAKE_SOURCE_DIR}/third_party/asio
)

find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
    PATHS
    /usr/include
    /usr/local/include
    ${CMAKE_SOURCE_DIR}/third_party/json
)

# 包含目录
include_directories(
    ${WEBSOCKETPP_INCLUDE_DIR}
    ${ASIO_INCLUDE_DIR}
    ${NLOHMANN_JSON_INCLUDE_DIR}
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 添加可执行文件
add_executable(cerebellum_node
    src/main.cpp
)

# 链接库
target_link_libraries(cerebellum_node
    ${rclcpp_LIBRARIES}
    ${std_msgs_LIBRARIES}
    ${sensor_msgs_LIBRARIES}
    ${geometry_msgs_LIBRARIES}
    pthread
    rt  # 实时库
)

# 安装目标
install(TARGETS cerebellum_node
    RUNTIME DESTINATION lib/${PROJECT_NAME}
)

# 安装启动文件
install(DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
)

# 安装配置文件
install(DIRECTORY config
    DESTINATION share/${PROJECT_NAME}
)

# 导出依赖
ament_export_dependencies(rclcpp std_msgs sensor_msgs geometry_msgs)
ament_export_include_directories(include)
ament_export_libraries(cerebellum_node)

ament_package()