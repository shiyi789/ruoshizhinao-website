# 如是具身 RS_01型 机器人大小脑系统
## 如是心（无锡）智能科技有限公司

## 🚀 项目概述

**如是具身 RS_01型** 是如是心公司研发的新一代具身智能人形机器人，采用创新的"大脑+小脑"软件架构，硬件依托松延动力等合作伙伴。

### 核心特点
- **哲学驱动**：基于"如是"（真实）和"具身"（身体智能统一）的设计理念
- **软件定义**：专注大小脑软件系统，硬件多平台适配
- **自然交互**：禅意对话，情感表达，自然运动
- **实时控制**：500Hz实时控制频率，确保运动稳定性

## 📁 项目结构

```
RS_01_如是具身/
├── brain/                    # 大脑系统（高级AI决策）
│   ├── api/                  # API接口层
│   │   ├── rest/            # REST API服务
│   │   ├── websocket/       # WebSocket服务
│   │   └── grpc/            # gRPC服务（可选）
│   ├── core/                # 核心功能模块
│   │   ├── perception/      # 感知模块
│   │   ├── cognition/       # 认知模块
│   │   ├── interaction/     # 交互模块
│   │   └── learning/        # 学习模块
│   ├── modules/             # 功能模块
│   │   ├── speech/          # 语音处理
│   │   ├── vision/          # 视觉处理
│   │   ├── nlp/             # 自然语言处理
│   │   └── memory/          # 记忆管理
│   ├── config/              # 配置文件
│   └── deploy/              # 部署配置
│
├── cerebellum/              # 小脑系统（实时运动控制）
│   ├── src/                # 源代码
│   │   ├── control/        # 控制算法
│   │   ├── planning/       # 运动规划
│   │   ├── safety/         # 安全监控
│   │   ├── hardware/       # 硬件接口
│   │   └── utils/          # 工具函数
│   ├── include/            # 头文件
│   ├── config/             # 配置文件
│   ├── test/               # 测试代码
│   └── deploy/             # 部署配置
│
├── hardware/               # 硬件适配层
│   ├── adapters/           # 硬件适配器
│   │   ├── noetix/         # 松延动力适配器
│   │   ├── unitree/        # 宇树科技适配器
│   │   ├── dji/            # 大疆适配器
│   │   └── custom/         # 自定义适配器
│   ├── drivers/            # 硬件驱动
│   ├── interfaces/         # 接口定义
│   └── tools/              # 硬件工具
│
├── simulation/             # 仿真环境
│   ├── gazebo/             # Gazebo仿真
│   ├── webots/             # Webots仿真
│   ├── isaac/              # NVIDIA Isaac Sim仿真
│   └── scenarios/          # 仿真场景
│
├── tools/                  # 开发工具
├── scripts/                # 脚本工具
├── docs/                   # 文档
└── config/                 # 全局配置
```

## 🛠️ 快速开始

### 环境要求
- **操作系统**：Ubuntu 22.04（推荐）或 Windows 10+ WSL2
- **Python**：3.9+
- **ROS2**：Humble 或 Iron
- **Docker**：20.10+（可选）

### 安装步骤

#### 1. 克隆项目
```bash
git clone <项目地址>
cd RS_01_如是具身
```

#### 2. 设置开发环境
```bash
# 运行设置脚本
./scripts/setup_development.sh

# 激活Python虚拟环境
source venv/bin/activate
```

#### 3. 启动大脑系统
```bash
cd brain/api/rest
python server.py
```
访问：http://localhost:8000/docs

#### 4. 构建小脑系统
```bash
cd cerebellum
mkdir build && cd build
cmake .. && make
./cerebellum_node
```

#### 5. 快速启动所有服务
```bash
./quick_start.sh all
```

## 🔧 技术架构

### 大脑系统（如是）
- **框架**：FastAPI + WebSocket
- **AI服务**：云端LLM API + 本地轻量模型
- **功能模块**：
  - 语音识别（Whisper）
  - 语音合成（Edge-TTS）
  - 视觉识别（YOLOv8）
  - 自然语言理解（DeepSeek API）
  - 任务规划与决策

### 小脑系统（具身）
- **框架**：ROS2 + C++17
- **控制频率**：500Hz（可升级至1000Hz）
- **核心算法**：
  - 简化平衡控制
  - 基础步态生成
  - 轨迹规划
  - 安全监控
- **硬件适配**：标准化接口，支持多平台

### 通信协议
- **大脑↔小脑**：WebSocket + JSON
- **小脑↔硬件**：标准化控制接口
- **外部API**：RESTful API + WebSocket

## 📊 性能指标

### 大脑系统
- 语音识别准确率：> 92%
- 响应延迟：< 800ms
- API吞吐量：100+ QPS

### 小脑系统
- 控制频率：500Hz
- 状态更新：10Hz（发布到ROS2）
- 命令延迟：< 20ms

### 系统整体
- 端到端延迟：< 2.0s
- 系统稳定性：连续运行4小时
- 内存占用：< 2.5GB

## 🎯 使用示例

### 语音控制
```python
import requests
import json

# 发送语音指令
response = requests.post("http://localhost:8000/voice/process", json={
    "text": "向前走",
    "user_id": "test_user"
})

print(response.json())
# 输出：机器人开始向前行走
```

### 视觉识别
```python
import base64
import requests

# 读取图像并编码
with open("test_image.jpg", "rb") as f:
    image_data = base64.b64encode(f.read()).decode()

response = requests.post("http://localhost:8000/vision/process", json={
    "image_data": image_data,
    "task": "detect"
})

print(response.json())
# 输出：检测到的物体列表
```

### ROS2控制
```bash
# 发布控制命令
ros2 topic pub /rs_01/command std_msgs/msg/String "data: 'walk_forward'"

# 查看关节状态
ros2 topic echo /rs_01/joint_states

# 查看IMU数据
ros2 topic echo /rs_01/imu
```

## 📈 开发路线图

### 第一阶段（1-3个月）：原型验证
- ✅ 基础框架搭建
- ✅ 核心功能实现
- ✅ 系统集成测试
- 🔄 性能优化

### 第二阶段（4-6个月）：产品化
- 🔄 多平台硬件适配
- 🔄 高级AI功能集成
- 🔄 用户体验优化
- 🔄 正式产品发布

### 第三阶段（7-12个月）：商业化
- 🔄 客户试点部署
- 🔄 生态建设
- 🔄 规模化应用

## 🤝 合作与支持

### 技术合作
- **松延动力**：主要硬件合作伙伴
- **其他机器人厂商**：硬件适配合作
- **AI服务商**：算法优化合作

### 开发者支持
- **文档**：完整的API文档和开发指南
- **示例**：丰富的使用示例和Demo
- **社区**：技术交流和支持

### 商业合作
- **OEM合作**：为硬件厂商提供大小脑系统
- **定制开发**：针对特定场景的定制化开发
- **技术服务**：技术咨询和培训服务

## 📞 联系我们

- **公司**：如是心（无锡）智能科技有限公司
- **项目**：如是具身 RS_01型
- **状态**：开发中
- **版本**：v0.1.0-alpha

## 📄 许可证

本项目版权归如是心（无锡）智能科技有限公司所有。

---

**如是我闻，具身智能。**  
**真实感知，自然交互。**  
**如是心公司，创造下一代机器人体验。**