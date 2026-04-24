#!/usr/bin/env python3
# 如是具身 RS_01型 - 大脑REST API服务
# 如是心（无锡）智能科技有限公司

from fastapi import FastAPI, HTTPException, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Optional, List, Dict, Any
import uvicorn
import logging
import json
import asyncio
from datetime import datetime
import numpy as np

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# 创建FastAPI应用
app = FastAPI(
    title="如是具身 RS_01型 大脑API",
    description="如是心（无锡）智能科技有限公司 - 机器人大小脑系统",
    version="0.1.0",
    docs_url="/docs",
    redoc_url="/redoc"
)

# 添加CORS中间件
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# 数据模型
class VoiceRequest(BaseModel):
    audio_data: Optional[str] = None  # base64编码的音频数据
    text: Optional[str] = None  # 或直接文本
    user_id: str = "default_user"
    session_id: Optional[str] = None

class VisionRequest(BaseModel):
    image_data: Optional[str] = None  # base64编码的图像数据
    image_url: Optional[str] = None
    task: str = "detect"  # detect, recognize, segment

class TaskRequest(BaseModel):
    task_type: str  # walk, grasp, speak, etc.
    parameters: Dict[str, Any] = {}
    priority: int = 1

class RobotState(BaseModel):
    timestamp: datetime
    joint_positions: List[float]
    joint_velocities: List[float]
    imu_data: Dict[str, float]
    battery_level: float
    temperature: Dict[str, float]
    status: str  # idle, moving, processing, error

# 连接管理器（用于WebSocket）
class ConnectionManager:
    def __init__(self):
        self.active_connections: List[WebSocket] = []
        self.cerebellum_connection: Optional[WebSocket] = None

    async def connect(self, websocket: WebSocket, client_type: str = "client"):
        await websocket.accept()
        if client_type == "cerebellum":
            self.cerebellum_connection = websocket
            logger.info("小脑系统连接建立")
        else:
            self.active_connections.append(websocket)
        return websocket

    def disconnect(self, websocket: WebSocket, client_type: str = "client"):
        if client_type == "cerebellum" and websocket == self.cerebellum_connection:
            self.cerebellum_connection = None
            logger.info("小脑系统连接断开")
        elif websocket in self.active_connections:
            self.active_connections.remove(websocket)

    async def send_to_cerebellum(self, message: Dict):
        if self.cerebellum_connection:
            await self.cerebellum_connection.send_json(message)
            return True
        return False

    async def broadcast(self, message: Dict):
        for connection in self.active_connections:
            try:
                await connection.send_json(message)
            except:
                pass

manager = ConnectionManager()

# 状态存储
class BrainState:
    def __init__(self):
        self.current_task = None
        self.robot_state = None
        self.conversation_history = []
        self.user_profiles = {}
        self.system_status = "initializing"
        
    def update_robot_state(self, state: RobotState):
        self.robot_state = state
        logger.info(f"机器人状态更新: {state.status}")
        
    def add_conversation(self, user_input: str, bot_response: str):
        self.conversation_history.append({
            "timestamp": datetime.now(),
            "user": user_input,
            "bot": bot_response
        })
        # 保持最近100条记录
        if len(self.conversation_history) > 100:
            self.conversation_history = self.conversation_history[-100:]

brain_state = BrainState()

# AI服务模拟（实际应该调用真实AI服务）
class AIService:
    def __init__(self):
        self.llm_provider = "deepseek"  # 可以使用DeepSeek API
        
    async def process_text(self, text: str, context: List[Dict] = None) -> str:
        """处理文本，生成响应"""
        # 这里应该调用真实的LLM API
        # 暂时使用规则响应
        
        responses = {
            "你好": "你好！我是如是具身 RS_01型机器人，很高兴为你服务。",
            "你叫什么名字": "我叫'如是具身'，型号RS_01，是如是心公司研发的具身智能机器人。",
            "你会做什么": "我可以与你自然对话，理解你的指令，执行行走、抓取等任务。",
            "向前走": "好的，我将向前行走。",
            "停止": "收到，立即停止。",
            "谢谢": "不客气！随时为你服务。"
        }
        
        # 查找匹配的响应
        for key in responses:
            if key in text:
                return responses[key]
        
        # 默认响应
        return f"我理解你说的是：{text}。请告诉我具体需要什么帮助？"
    
    async def process_audio(self, audio_data: bytes) -> str:
        """语音识别"""
        # 这里应该调用Whisper等语音识别服务
        # 暂时返回模拟结果
        return "你好，机器人"
    
    async def process_image(self, image_data: bytes, task: str = "detect") -> Dict:
        """图像处理"""
        # 这里应该调用视觉模型
        return {
            "objects": [
                {"class": "person", "confidence": 0.95, "bbox": [100, 100, 200, 300]},
                {"class": "chair", "confidence": 0.88, "bbox": [300, 200, 350, 250]}
            ],
            "task": task
        }

ai_service = AIService()

# API端点
@app.get("/")
async def root():
    """根端点，返回服务信息"""
    return {
        "service": "如是具身 RS_01型 大脑API",
        "version": "0.1.0",
        "company": "如是心（无锡）智能科技有限公司",
        "status": brain_state.system_status,
        "timestamp": datetime.now().isoformat()
    }

@app.get("/health")
async def health_check():
    """健康检查"""
    return {
        "status": "healthy",
        "brain": "running",
        "ai_service": "available",
        "timestamp": datetime.now().isoformat()
    }

@app.post("/voice/process")
async def process_voice(request: VoiceRequest):
    """处理语音输入"""
    try:
        # 如果有音频数据，先进行语音识别
        if request.audio_data:
            # 这里应该解码base64音频并处理
            text = await ai_service.process_audio(request.audio_data)
        else:
            text = request.text
            
        # 文本理解和响应生成
        response = await ai_service.process_text(text)
        
        # 记录对话
        brain_state.add_conversation(text, response)
        
        # 检查是否需要执行动作
        action = None
        if "走" in text or "移动" in text:
            action = {"type": "motion", "command": "walk_forward"}
        elif "停止" in text or "停" in text:
            action = {"type": "motion", "command": "stop"}
            
        # 如果有动作，发送给小脑
        if action:
            await manager.send_to_cerebellum({
                "type": "task",
                "task_id": f"task_{datetime.now().timestamp()}",
                "action": action,
                "priority": 1
            })
        
        return {
            "input": text,
            "response": response,
            "action_triggered": action is not None,
            "timestamp": datetime.now().isoformat()
        }
        
    except Exception as e:
        logger.error(f"语音处理错误: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/vision/process")
async def process_vision(request: VisionRequest):
    """处理视觉输入"""
    try:
        result = await ai_service.process_image(request.image_data, request.task)
        return {
            "task": request.task,
            "result": result,
            "timestamp": datetime.now().isoformat()
        }
    except Exception as e:
        logger.error(f"视觉处理错误: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/task/execute")
async def execute_task(request: TaskRequest):
    """执行任务"""
    try:
        # 创建任务
        task_id = f"task_{datetime.now().timestamp()}"
        brain_state.current_task = {
            "task_id": task_id,
            "type": request.task_type,
            "parameters": request.parameters,
            "status": "executing",
            "start_time": datetime.now()
        }
        
        # 发送任务给小脑
        success = await manager.send_to_cerebellum({
            "type": "task",
            "task_id": task_id,
            "task_type": request.task_type,
            "parameters": request.parameters,
            "priority": request.priority
        })
        
        if not success:
            raise HTTPException(status_code=503, detail="小脑系统未连接")
        
        return {
            "task_id": task_id,
            "status": "executing",
            "message": f"任务'{request.task_type}'已发送到小脑系统",
            "timestamp": datetime.now().isoformat()
        }
        
    except Exception as e:
        logger.error(f"任务执行错误: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/state/robot")
async def get_robot_state():
    """获取机器人状态"""
    if brain_state.robot_state:
        return brain_state.robot_state
    else:
        # 返回模拟状态
        return RobotState(
            timestamp=datetime.now(),
            joint_positions=[0.0] * 28,
            joint_velocities=[0.0] * 28,
            imu_data={"accel_x": 0.0, "accel_y": 0.0, "accel_z": 9.8, "gyro_x": 0.0, "gyro_y": 0.0, "gyro_z": 0.0},
            battery_level=85.5,
            temperature={"cpu": 45.0, "motor_avg": 38.0},
            status="idle"
        )

@app.get("/conversation/history")
async def get_conversation_history(limit: int = 10):
    """获取对话历史"""
    return {
        "history": brain_state.conversation_history[-limit:],
        "total": len(brain_state.conversation_history),
        "timestamp": datetime.now().isoformat()
    }

# WebSocket端点
@app.websocket("/ws/brain")
async def websocket_brain(websocket: WebSocket):
    """大脑WebSocket端点"""
    await manager.connect(websocket, "client")
    try:
        while True:
            # 接收消息
            data = await websocket.receive_json()
            logger.info(f"收到WebSocket消息: {data}")
            
            # 处理消息
            if data.get("type") == "ping":
                await websocket.send_json({"type": "pong", "timestamp": datetime.now().isoformat()})
            elif data.get("type") == "get_state":
                await websocket.send_json({
                    "type": "state",
                    "state": {
                        "system_status": brain_state.system_status,
                        "current_task": brain_state.current_task,
                        "robot_connected": manager.cerebellum_connection is not None
                    }
                })
                
    except WebSocketDisconnect:
        manager.disconnect(websocket, "client")
        logger.info("客户端WebSocket连接断开")

@app.websocket("/ws/cerebellum")
async def websocket_cerebellum(websocket: WebSocket):
    """小脑系统WebSocket端点"""
    await manager.connect(websocket, "cerebellum")
    brain_state.system_status = "connected"
    
    try:
        while True:
            # 接收小脑状态更新
            data = await websocket.receive_json()
            
            if data.get("type") == "state_update":
                # 更新机器人状态
                state_data = data.get("state", {})
                robot_state = RobotState(
                    timestamp=datetime.fromisoformat(state_data.get("timestamp", datetime.now().isoformat())),
                    joint_positions=state_data.get("joint_positions", []),
                    joint_velocities=state_data.get("joint_velocities", []),
                    imu_data=state_data.get("imu_data", {}),
                    battery_level=state_data.get("battery_level", 100.0),
                    temperature=state_data.get("temperature", {}),
                    status=state_data.get("status", "unknown")
                )
                brain_state.update_robot_state(robot_state)
                
                # 广播状态更新给所有客户端
                await manager.broadcast({
                    "type": "robot_state",
                    "state": robot_state.dict(),
                    "timestamp": datetime.now().isoformat()
                })
                
            elif data.get("type") == "task_complete":
                # 任务完成通知
                task_id = data.get("task_id")
                if brain_state.current_task and brain_state.current_task.get("task_id") == task_id:
                    brain_state.current_task["status"] = "completed"
                    brain_state.current_task["end_time"] = datetime.now()
                    
                await manager.broadcast({
                    "type": "task_update",
                    "task_id": task_id,
                    "status": "completed",
                    "result": data.get("result", {}),
                    "timestamp": datetime.now().isoformat()
                })
                
    except WebSocketDisconnect:
        manager.disconnect(websocket, "cerebellum")
        brain_state.system_status = "disconnected"
        logger.info("小脑系统WebSocket连接断开")

# 启动函数
if __name__ == "__main__":
    logger.info("启动如是具身 RS_01型 大脑API服务...")
    brain_state.system_status = "running"
    
    uvicorn.run(
        app,
        host="0.0.0.0",
        port=8000,
        log_level="info"
    )