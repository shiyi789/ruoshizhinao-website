@echo off
echo ==========================================
echo   如是具身 RS_01型 系统启动脚本
echo   如是心（无锡）智能科技有限公司
echo ==========================================
echo.

REM 检查Python环境
python --version >nul 2>&1
if errorlevel 1 (
    echo ❌ Python未安装或未添加到PATH
    echo 请先安装Python 3.9+
    pause
    exit /b 1
)

REM 检查虚拟环境
if not exist "venv\Scripts\activate.bat" (
    echo 创建Python虚拟环境...
    python -m venv venv
)

REM 激活虚拟环境
call venv\Scripts\activate.bat

REM 安装依赖
echo 安装大脑系统依赖...
pip install -r requirements.txt >nul 2>&1
if errorlevel 1 (
    echo 依赖安装失败，尝试手动安装...
    pip install fastapi uvicorn openai whisper opencv-python numpy pydantic redis python-socketio edge-tts torch --index-url https://download.pytorch.org/whl/cpu
)

REM 创建必要目录
if not exist "brain\api\rest\logs" mkdir brain\api\rest\logs
if not exist "cerebellum\build" mkdir cerebellum\build

echo.
echo 选择启动模式：
echo 1. 只启动大脑系统
echo 2. 只启动小脑系统（需要ROS2环境）
echo 3. 启动完整系统（大脑 + 小脑模拟）
echo 4. 开发模式（所有服务 + 监控）
echo.
set /p choice="请选择 (1-4): "

if "%choice%"=="1" goto start_brain
if "%choice%"=="2" goto start_cerebellum
if "%choice%"=="3" goto start_all
if "%choice%"=="4" goto start_dev
goto invalid_choice

:start_brain
echo.
echo 启动大脑系统...
cd brain\api\rest
start "如是大脑" cmd /k "python server.py"
echo ✅ 大脑系统已启动
echo 访问: http://localhost:8000/docs
goto end

:start_cerebellum
echo.
echo 启动小脑系统...
echo 注意：需要ROS2环境
cd cerebellum\build
REM 这里应该调用ROS2启动脚本
echo ⚠️  小脑系统需要ROS2环境，请手动启动
goto end

:start_all
echo.
echo 启动完整系统...
echo 启动大脑系统...
cd brain\api\rest
start "如是大脑" cmd /k "python server.py"
timeout /t 3 /nobreak >nul

echo 启动小脑模拟器...
cd ..\..\cerebellum\build
start "如是小脑" cmd /k "echo 小脑系统模拟模式运行中..."
echo.
echo ✅ 完整系统已启动
echo 大脑API: http://localhost:8000/docs
echo 小脑状态: 模拟模式
goto end

:start_dev
echo.
echo 启动开发模式...
echo 启动大脑系统...
cd brain\api\rest
start "如是大脑" cmd /k "python server.py"
timeout /t 2 /nobreak >nul

echo 启动API监控...
start "API监控" cmd /k "python -m http.server 8001"
timeout /t 1 /nobreak >nul

echo 启动日志监控...
if not exist "logs" mkdir logs
start "日志监控" cmd /k "tail -f brain\api\rest\logs\server.log"

echo.
echo ✅ 开发模式已启动
echo 大脑API: http://localhost:8000/docs
echo 文件服务: http://localhost:8001
echo 日志监控: tail -f logs/server.log
goto end

:invalid_choice
echo 无效选择
goto end

:end
echo.
echo 按任意键退出...
pause >nul