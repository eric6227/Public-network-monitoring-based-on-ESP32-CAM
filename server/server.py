import asyncio
import struct
import time
from aiohttp import web
import logging

# 设置日志配置
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler("server.log"),
        logging.StreamHandler()
    ]
)

# 最终完整帧
latest_frames = {}
pending_frames = {}
TIMEOUT = 2.0  # 收齐超时秒数

class UdpProtocol(asyncio.DatagramProtocol):
    def datagram_received(self, data, addr):
        try:
            pos = 0
            # 1B camId长度
            id_len = data[pos]
            pos += 1
            camid = data[pos:pos+id_len].decode()
            pos += id_len
            # 2B 帧序号
            frame_seq = struct.unpack('>H', data[pos:pos+2])[0]
            pos += 2
            # 1B 总分片数
            total_chunks = data[pos]
            pos += 1
            # 1B 当前分片索引
            chunk_idx = data[pos]
            pos += 1
            # 剩余为数据
            chunk_data = data[pos:]

            logging.info(f"收到数据包: camid={camid}, frame_seq={frame_seq}, total_chunks={total_chunks}, chunk_idx={chunk_idx}")

            if chunk_idx >= total_chunks or total_chunks == 0:
                return

            key = (camid, frame_seq)
            now = time.time()

            if key in pending_frames:
                if now - pending_frames[key]['last_time'] > TIMEOUT:
                    logging.warning(f"超时丢弃帧: camid={camid}, frame_seq={frame_seq}")
                    del pending_frames[key]

            if key not in pending_frames:
                pending_frames[key] = {
                    'total': total_chunks,
                    'chunks': [None] * total_chunks,
                    'received': 0,
                    'last_time': now
                }
            info = pending_frames[key]
            info['last_time'] = now

            if info['chunks'][chunk_idx] is None:
                info['chunks'][chunk_idx] = chunk_data
                info['received'] += 1

            if info['received'] == total_chunks:
                full_frame = b''.join(info['chunks'])
                latest_frames[camid] = {
                    "frame": full_frame,
                    "last_update": time.time()
                }
                del pending_frames[key]
                logging.info(f"完整帧接收完成: camid={camid}, frame_seq={frame_seq}")
        except Exception as e:
            logging.error(f"解析错误: {e}")

async def mjpeg_handler(request):
    camid = request.match_info.get('camid', None)
    if not camid:
        active = ", ".join(latest_frames.keys()) if latest_frames else "无"
        logging.warning("未指定摄像头ID")
        return web.Response(text=f"请指定摄像头ID，当前活跃: {active}", status=400,
                            headers={"Cache-Control": "no-cache"})
    if camid not in latest_frames:
        logging.warning(f"未找到摄像头: {camid}")
        return web.Response(text=f"未找到摄像头: {camid}", status=404,
                            headers={"Cache-Control": "no-cache"})

    logging.info(f"开始推送视频流: camid={camid}")
    response = web.StreamResponse()
    response.headers['Content-Type'] = 'multipart/x-mixed-replace; boundary=frame'
    await response.prepare(request)

    timeout = 5.0   # 秒
    cam_info = latest_frames[camid]
    last_frame_time = cam_info.get("last_update", time.time())

    try:
        while camid in latest_frames:
            cam_info = latest_frames[camid]
            frame_data = cam_info.get("frame")
            current_update = cam_info.get("last_update", time.time())

            # 新帧到达时更新时间戳
            if current_update > last_frame_time:
                last_frame_time = current_update

            if time.time() - last_frame_time > timeout:
                logging.warning(f"摄像头 {camid} 超时无新帧，停止推流")
                break

            if frame_data:
                await response.write(
                    b'--frame\r\n'
                    b'Content-Type: image/jpeg\r\n\r\n' + frame_data + b'\r\n'
                )
            await asyncio.sleep(0.05)
    except asyncio.CancelledError:
        logging.info(f"HTTP流客户端主动断开: {camid}")
    finally:
        return response

async def main():
    loop = asyncio.get_running_loop()
    await loop.create_datagram_endpoint(UdpProtocol, local_addr=('0.0.0.0', 8888))  #此处设置UDP监听端口，必须与摄像头发送端一致
    logging.info("UDP 链路已启动，监听端口 8888")

    app = web.Application()
    app.router.add_get('/stream/{camid}', mjpeg_handler)
    runner = web.AppRunner(app)
    await runner.setup()
    site = web.TCPSite(runner, '0.0.0.0', 8081)
    await site.start()
    logging.info("HTTP 服务器已启动，端口 8081")

    await asyncio.Event().wait()

if __name__ == '__main__':
    asyncio.run(main())