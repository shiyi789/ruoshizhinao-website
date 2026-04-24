// 如是具身 RS_01型 - 小脑系统主程序
// 如是心（无锡）智能科技有限公司

#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>
#include <string>
#include <map>

// ROS2头文件
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

// WebSocket客户端（用于连接大脑）
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"

// JSON库
#include "nlohmann/json.hpp"
using json = nlohmann::json;

namespace rushi {

// 机器人状态
struct RobotState {
    std::vector<double> joint_positions;
    std::vector<double> joint_velocities;
    std::vector<double> joint_torques;
    
    struct IMUData {
        double accel_x, accel_y, accel_z;
        double gyro_x, gyro_y, gyro_z;
        double quat_w, quat_x, quat_y, quat_z;
    } imu;
    
    double battery_level;
    std::map<std::string, double> temperatures;
    std::string status;
    
    std::chrono::system_clock::time_point timestamp;
};

// 控制命令
struct ControlCommand {
    enum class Type {
        STOP,
        WALK,
        TURN,
        STAND,
        SIT,
        CUSTOM
    };
    
    Type type;
    std::map<std::string, double> parameters;
    int priority;
    std::string task_id;
};

// 简化平衡控制器
class SimpleBalanceController {
public:
    SimpleBalanceController() {
        // 初始化控制参数
        kp_ = 80.0;
        kd_ = 8.0;
        target_pitch_ = 0.0;
        target_roll_ = 0.0;
    }
    
    std::vector<double> computeCorrections(const RobotState& state) {
        std::vector<double> corrections(28, 0.0); // 假设28个自由度
        
        // 简单的PD控制，基于IMU数据调整
        double pitch_error = target_pitch_ - estimatePitch(state.imu);
        double roll_error = target_roll_ - estimateRoll(state.imu);
        
        // 生成简单的修正量
        // 这里应该根据机器人模型计算具体的关节修正
        // 暂时使用简化版本
        
        return corrections;
    }
    
    void setTarget(double pitch, double roll) {
        target_pitch_ = pitch;
        target_roll_ = roll;
    }
    
private:
    double kp_, kd_;
    double target_pitch_, target_roll_;
    
    double estimatePitch(const RobotState::IMUData& imu) {
        // 简化估计俯仰角
        return std::atan2(imu.accel_y, imu.accel_z);
    }
    
    double estimateRoll(const RobotState::IMUData& imu) {
        // 简化估计横滚角
        return std::atan2(imu.accel_x, imu.accel_z);
    }
};

// 基础步态生成器
class BasicGaitGenerator {
public:
    BasicGaitGenerator() {
        walking_ = false;
        gait_phase_ = 0.0;
        gait_speed_ = 0.5; // m/s
    }
    
    std::vector<double> generateWalk(double speed, double turn_rate) {
        std::vector<double> positions(28, 0.0);
        
        if (!walking_) {
            // 站立姿势
            return getStandingPose();
        }
        
        // 简单的步行模式生成
        // 这里应该实现完整的步态算法
        // 暂时返回基础位置
        
        gait_phase_ += 0.01; // 更新步态相位
        if (gait_phase_ > 1.0) gait_phase_ = 0.0;
        
        // 根据相位计算关节位置
        // 简化实现
        for (size_t i = 0; i < positions.size(); ++i) {
            positions[i] = 0.1 * std::sin(2 * M_PI * gait_phase_ + i * 0.1);
        }
        
        return positions;
    }
    
    void startWalking(double speed) {
        walking_ = true;
        gait_speed_ = speed;
        gait_phase_ = 0.0;
    }
    
    void stopWalking() {
        walking_ = false;
    }
    
    std::vector<double> getStandingPose() {
        // 返回站立姿势
        std::vector<double> pose(28, 0.0);
        // 设置基础站立姿势
        // 简化实现
        return pose;
    }
    
private:
    bool walking_;
    double gait_phase_;
    double gait_speed_;
};

// 硬件抽象接口
class HardwareInterface {
public:
    virtual ~HardwareInterface() = default;
    
    virtual bool initialize() = 0;
    virtual RobotState readSensors() = 0;
    virtual bool sendCommands(const std::vector<double>& commands) = 0;
    virtual void emergencyStop() = 0;
    
    virtual std::string getPlatformName() const = 0;
};

// 松延动力适配器（模拟）
class NoetixAdapter : public HardwareInterface {
public:
    NoetixAdapter() {
        // 初始化模拟数据
        simulated_joints_ = std::vector<double>(28, 0.0);
        simulated_imu_ = {0.0, 0.0, 9.8, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0};
    }
    
    bool initialize() override {
        std::cout << "[NoetixAdapter] 初始化松延动力硬件接口" << std::endl;
        // 这里应该初始化真实的硬件连接
        // CAN/EtherCAT通信等
        return true;
    }
    
    RobotState readSensors() override {
        RobotState state;
        
        // 模拟传感器数据
        state.joint_positions = simulated_joints_;
        state.joint_velocities = std::vector<double>(28, 0.0);
        state.joint_torques = std::vector<double>(28, 0.0);
        state.imu = simulated_imu_;
        state.battery_level = 85.5;
        state.temperatures = {{"cpu", 45.0}, {"motor_avg", 38.0}};
        state.status = "operational";
        state.timestamp = std::chrono::system_clock::now();
        
        // 更新模拟数据（简单振荡）
        static double phase = 0.0;
        phase += 0.01;
        simulated_imu_.accel_x = 0.1 * std::sin(phase);
        simulated_imu_.accel_y = 0.1 * std::cos(phase);
        
        return state;
    }
    
    bool sendCommands(const std::vector<double>& commands) override {
        if (commands.size() != simulated_joints_.size()) {
            std::cerr << "[NoetixAdapter] 命令数量不匹配" << std::endl;
            return false;
        }
        
        // 模拟命令执行
        simulated_joints_ = commands;
        
        // 添加一些噪声和延迟模拟
        for (auto& pos : simulated_joints_) {
            pos += (rand() % 100 - 50) * 0.0001; // 小噪声
        }
        
        return true;
    }
    
    void emergencyStop() override {
        std::cout << "[NoetixAdapter] 紧急停止！" << std::endl;
        // 发送紧急停止命令到硬件
        simulated_joints_ = std::vector<double>(28, 0.0);
    }
    
    std::string getPlatformName() const override {
        return "Noetix RS_01 Platform";
    }
    
private:
    std::vector<double> simulated_joints_;
    RobotState::IMUData simulated_imu_;
};

// WebSocket客户端（连接大脑）
class BrainWebSocketClient {
public:
    BrainWebSocketClient(const std::string& url) : url_(url), connected_(false) {
        // 初始化WebSocket客户端
        client_.init_asio();
        client_.set_open_handler([this](auto hdl) {
            std::cout << "[WebSocket] 连接到大脑服务器" << std::endl;
            connected_ = true;
            connection_handle_ = hdl;
        });
        
        client_.set_close_handler([this](auto hdl) {
            std::cout << "[WebSocket] 与大脑服务器断开连接" << std::endl;
            connected_ = false;
        });
        
        client_.set_message_handler([this](auto hdl, auto msg) {
            this->onMessage(hdl, msg);
        });
        
        client_.set_fail_handler([this](auto hdl) {
            std::cerr << "[WebSocket] 连接失败" << std::endl;
            connected_ = false;
        });
    }
    
    bool connect() {
        try {
            websocketpp::lib::error_code ec;
            auto con = client_.get_connection(url_, ec);
            
            if (ec) {
                std::cerr << "[WebSocket] 创建连接失败: " << ec.message() << std::endl;
                return false;
            }
            
            client_.connect(con);
            
            // 启动IO线程
            ws_thread_ = std::thread([this]() {
                client_.run();
            });
            
            // 等待连接建立
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            return connected_;
            
        } catch (const std::exception& e) {
            std::cerr << "[WebSocket] 连接异常: " << e.what() << std::endl;
            return false;
        }
    }
    
    void disconnect() {
        if (connected_ && connection_handle_.lock()) {
            client_.close(connection_handle_, websocketpp::close::status::normal, "Disconnecting");
        }
        connected_ = false;
        
        if (ws_thread_.joinable()) {
            ws_thread_.join();
        }
    }
    
    bool send(const json& message) {
        if (!connected_) {
            return false;
        }
        
        try {
            websocketpp::lib::error_code ec;
            client_.send(connection_handle_, message.dump(), websocketpp::frame::opcode::text, ec);
            
            if (ec) {
                std::cerr << "[WebSocket] 发送失败: " << ec.message() << std::endl;
                return false;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "[WebSocket] 发送异常: " << e.what() << std::endl;
            return false;
        }
    }
    
    void onMessage(websocketpp::connection_hdl hdl, 
                   websocketpp::config::asio_client::message_type::ptr msg) {
        try {
            auto j = json::parse(msg->get_payload());
            std::cout << "[WebSocket] 收到消息: " << j.dump() << std::endl;
            
            // 处理消息
            processBrainMessage(j);
            
        } catch (const std::exception& e) {
            std::cerr << "[WebSocket] 消息解析错误: " << e.what() << std::endl;
        }
    }
    
    void processBrainMessage(const json& message) {
        std::string msg_type = message.value("type", "");
        
        if (msg_type == "task") {
            // 处理任务命令
            std::string task_type = message.value("task_type", "");
            std::string task_id = message.value("task_id", "");
            
            std::cout << "[Brain] 收到任务: " << task_type << " (ID: " << task_id << ")" << std::endl;
            
            // 这里应该触发任务执行
            // 暂时只是记录
            
            // 发送任务接收确认
            json ack = {
                {"type", "task_ack"},
                {"task_id", task_id},
                {"status", "received"}
            };
            send(ack);
            
        } else if (msg_type == "ping") {
            // 响应ping
            json pong = {
                {"type", "pong"},
                {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
            };
            send(pong);
        }
    }
    
    bool isConnected() const { return connected_; }
    
private:
    std::string url_;
    bool connected_;
    websocketpp::client<websocketpp::config::asio_client> client_;
    websocketpp::connection_hdl connection_handle_;
    std::thread ws_thread_;
};

// 小脑系统主节点
class CerebellumNode : public rclcpp::Node {
public:
    CerebellumNode() : Node("cerebellum_node") {
        // 初始化参数
        this->declare_parameter("control_frequency", 500.0);
        this->declare_parameter("brain_ws_url", "ws://localhost:8000/ws/cerebellum");
        this->declare_parameter("hardware_platform", "noetix");
        
        control_frequency_ = this->get_parameter("control_frequency").as_double();
        brain_ws_url_ = this->get_parameter("brain_ws_url").as_string();
        hardware_platform_ = this->get_parameter("hardware_platform").as_string();
        
        // 初始化硬件接口
        hardware_ = createHardwareInterface(hardware_platform_);
        if (!hardware_->initialize()) {
            RCLCPP_ERROR(this->get_logger(), "硬件接口初始化失败");
            throw std::runtime_error("硬件初始化失败");
        }
        
        // 初始化控制器
        balance_controller_ = std::make_unique<SimpleBalanceController>();
        gait_generator_ = std::make_unique<BasicGaitGenerator>();
        
        // 初始化WebSocket客户端
        ws_client_ = std::make_unique<BrainWebSocketClient>(brain_ws_url_);
        
        // 创建ROS2发布者
        state_publisher_ = this->create_publisher<sensor_msgs::msg::JointState>(
            "/rs_01/joint_states", 10);
        
        imu_publisher_ = this->create_publisher<sensor_msgs::msg::Imu>(
            "/rs_01/imu", 10);
        
        // 创建订阅者（用于直接控制命令）
        command_subscriber_ = this->create_subscription<std_msgs::msg::String>(
            "/rs_01/command", 10,
            std::bind(&CerebellumNode::commandCallback, this, std::placeholders::_1));
        
        // 启动控制线程
        running_ = true;
        control_thread_ = std::thread(&CerebellumNode::controlLoop, this);
        
        // 启动状态发布定时器
        publish_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100),  // 10Hz发布
            std::bind(&CerebellumNode::publishState, this));
        
        // 连接大脑服务器
        connectToBrain();
        
        RCLCPP_INFO(this->get_logger(), "如是具身 RS_01型 小脑系统启动成功");
        RCLCPP_INFO(this->get_logger(), "硬件平台: %s", hardware_->getPlatformName().c_str());
        RCLCPP_INFO(this->get_logger(), "控制频率: %.0f Hz", control_frequency_);
    }
    
    ~CerebellumNode() {
        running_ = false;
        if (control_thread_.joinable()) {
            control_thread_.join();
        }
        
        if (ws_client_) {
            ws_client_->disconnect();
        }
    }
    
private:
    std::unique_ptr<HardwareInterface> createHardwareInterface(const std::string& platform) {
        if (platform == "noetix") {
            return std::make_unique<NoetixAdapter>();
        } else {
            RCLCPP_WARN(this->get_logger(), "未知硬件平台: %s，使用模拟适配器", platform.c_str());
            return std::make_unique<NoetixAdapter>(); // 默认使用Noetix模拟
        }
    }
    
    void connectToBrain() {
        RCLCPP_INFO(this->get_logger(), "连接大脑服务器: %s", brain_ws_url_.c_str());
        
        if (ws_client_->connect()) {
            RCLCPP_INFO(this->get_logger(), "大脑服务器连接成功");
            
            // 发送连接确认
            json connect_msg = {
                {"type", "connect"},
                {"node_type", "cerebellum"},
                {"platform", hardware_platform_},
                {"version", "0.1.0"}
            };
            ws_client_->send(connect_msg);
            
        } else {
            RCLCPP_ERROR(this->get_logger(), "大脑服务器连接失败");
        }
    }
    
    void controlLoop() {
        auto control_interval = std::chrono::microseconds(
            static_cast<int>(1000000.0 / control_frequency_));
        
        auto next_control_time = std::chrono::steady_clock::now();
        
        while (running_) {
            // 1. 读取传感器数据
            RobotState state = hardware_->readSensors();
            current_state_ = state;
            
            // 2. 计算控制命令
            std::vector<double> commands;
            
            if (current_task_.type == ControlCommand::Type::WALK) {
                // 步行模式
                double speed = current_task_.parameters.value("speed", 0.3);
                double turn = current_task_.parameters.value("turn", 0.0);
                
                gait_generator_->startWalking(speed);
                commands = gait_generator_->generateWalk(speed, turn);
                
            } else if (current_task_.type == ControlCommand::Type::STAND) {
                // 站立姿势
                commands = gait_generator_->getStandingPose();
                
            } else if (current_task_.type == ControlCommand::Type::STOP) {
                // 停止
                gait_generator_->stopWalking();
                commands = gait_generator_->getStandingPose();
                
            } else {
                // 默认站立姿势
                commands = gait_generator_->getStandingPose();
            }
            
            // 3. 添加平衡修正
            auto balance_corrections = balance_controller_->computeCorrections(state);
            for (size_t i = 0; i < commands.size() && i < balance_corrections.size(); ++i) {
                commands[i] += balance_corrections[i];
            }
            
            // 4. 发送控制命令到硬件
            hardware_->sendCommands(commands);
            
            // 5. 更新状态并发送给大脑
            updateAndSendState(state);
            
            // 6. 更新时间
            next_control_time += control_interval;
            std::this_thread::sleep_until(next_control_time);
        }
    }
    
    void updateAndSendState(const RobotState& state) {
        // 发送状态到大脑（通过WebSocket）
        if (ws_client_ && ws_client_->isConnected()) {
            json state_msg = {
                {"type", "state_update"},
                {"state", {
                    {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                        state.timestamp.time_since_epoch()).count()},
                    {"joint_positions", state.joint_positions},
                    {"joint_velocities", state.joint_velocities},
                    {"imu_data", {
                        {"accel_x", state.imu.accel_x},
                        {"accel_y", state.imu.accel_y},
                        {"accel_z", state.imu.accel_z},
                        {"gyro_x", state.imu.gyro_x},
                        {"gyro_y", state.imu.gyro_y},
                        {"gyro_z", state.imu.gyro_z}
                    }},
                    {"battery_level", state.battery_level},
                    {"temperature", state.temperatures},
                    {"status", state.status}
                }}
            };
            
            ws_client_->send(state_msg);
        }
    }
    
    void commandCallback(const std_msgs::msg::String::SharedPtr msg) {
        std::string command = msg->data;
        RCLCPP_INFO(this->get_logger(), "收到命令: %s", command.c_str());
        
        // 解析命令
        if (command == "walk_forward") {
            current_task_.type = ControlCommand::Type::WALK;
            current_task_.parameters = {{"speed", 0.3}, {"turn", 0.0}};
            current_task_.task_id = "cmd_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            
        } else if (command == "walk_backward") {
            current_task_.type = ControlCommand::Type::WALK;
            current_task_.parameters = {{"speed", -0.2}, {"turn", 0.0}};
            current_task_.task_id = "cmd_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            
        } else if (command == "turn_left") {
            current_task_.type = ControlCommand::Type::WALK;
            current_task_.parameters = {{"speed", 0.1}, {"turn", 0.5}};
            current_task_.task_id = "cmd_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            
        } else if (command == "turn_right") {
            current_task_.type = ControlCommand::Type::WALK;
            current_task_.parameters = {{"speed", 0.1}, {"turn", -0.5}};
            current_task_.task_id = "cmd_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            
        } else if (command == "stop") {
            current_task_.type = ControlCommand::Type::STOP;
            current_task_.task_id = "cmd_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            
        } else if (command == "stand") {
            current_task_.type = ControlCommand::Type::STAND;
            current_task_.task_id = "cmd_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
            
        } else if (command == "emergency_stop") {
            hardware_->emergencyStop();
            current_task_.type = ControlCommand::Type::STOP;
        }
    }
    
    void publishState() {
        // 发布到ROS2话题
        auto joint_state_msg = sensor_msgs::msg::JointState();
        joint_state_msg.header.stamp = this->now();
        
        // 设置关节名称（简化）
        for (size_t i = 0; i < current_state_.joint_positions.size(); ++i) {
            joint_state_msg.name.push_back("joint_" + std::to_string(i));
            joint_state_msg.position.push_back(current_state_.joint_positions[i]);
            joint_state_msg.velocity.push_back(current_state_.joint_velocities[i]);
        }
        
        state_publisher_->publish(joint_state_msg);
        
        // 发布IMU数据
        auto imu_msg = sensor_msgs::msg::Imu();
        imu_msg.header.stamp = this->now();
        imu_msg.linear_acceleration.x = current_state_.imu.accel_x;
        imu_msg.linear_acceleration.y = current_state_.imu.accel_y;
        imu_msg.linear_acceleration.z = current_state_.imu.accel_z;
        imu_msg.angular_velocity.x = current_state_.imu.gyro_x;
        imu_msg.angular_velocity.y = current_state_.imu.gyro_y;
        imu_msg.angular_velocity.z = current_state_.imu.gyro_z;
        
        imu_publisher_->publish(imu_msg);
    }
    
    // 成员变量
    std::unique_ptr<HardwareInterface> hardware_;
    std::unique_ptr<SimpleBalanceController> balance_controller_;
    std::unique_ptr<BasicGaitGenerator> gait_generator_;
    std::unique_ptr<BrainWebSocketClient> ws_client_;
    
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr state_publisher_;
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_publisher_;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr command_subscriber_;
    
    rclcpp::TimerBase::SharedPtr publish_timer_;
    
    std::thread control_thread_;
    std::atomic<bool> running_{false};
    
    double control_frequency_;
    std::string brain_ws_url_;
    std::string hardware_platform_;
    
    ControlCommand current_task_;
    RobotState current_state_;
};

} // namespace rushi

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    try {
        auto node = std::make_shared<rushi::CerebellumNode>();
        rclcpp::spin(node);
        rclcpp::shutdown();
    } catch (const std::exception& e) {
        std::cerr << "小脑系统异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}