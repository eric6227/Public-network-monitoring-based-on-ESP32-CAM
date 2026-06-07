# 使用官方Python 3.9镜像作为基础镜像
FROM python:3.9-slim

# 设置工作目录
WORKDIR /app

# 复制requirements.txt文件
COPY requirements.txt .

# 安装依赖
RUN pip install --no-cache-dir -r requirements.txt

# 复制服务端代码
COPY server.py .

# 创建日志目录
RUN mkdir -p /app/logs

# 暴露端口
EXPOSE 8888/udp
EXPOSE 8081/tcp

# 设置环境变量
ENV PYTHONUNBUFFERED=1

# 运行服务端
CMD ["python", "server.py"]
