#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <errno.h>

/**
 * 嵌入式 Linux V4L2 摄像头采集示例 (单帧抓取)
 * 流程：打开 -> 设置格式 -> 申请内存 -> 内存映射(mmap) -> 入队 -> 开启流 -> 出队(抓取) -> 保存 -> 清理
 */

#define WIDTH  640
#define HEIGHT 480
#define VIDEO_DEV "/dev/video0"

int main() {
    int fd;
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;
    struct v4l2_buffer buf;
    void *buffer_start;
    enum v4l2_buf_type type;

    // 1. 打开摄像头设备
    // 使用 O_RDWR 读写模式打开字符设备节点
    fd = open(VIDEO_DEV, O_RDWR);
    if (fd < 0) {
        perror("打开摄像头失败，请检查设备是否存在或是否有权限(sudo)");
        return -1;
    }

    // 2. 查询设备能力
    // 确认设备是否支持视频捕获(CAPTURE)和流操作(STREAMING)
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("查询设备能力失败");
        close(fd);
        return -1;
    }
    printf("驱动名称: %s\n", cap.driver);

    // 3. 设置视频格式
    // 这里设置分辨率为 640x480，格式为 YUYV (常见的原始像素格式)
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; 
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("设置格式失败");
        close(fd);
        return -1;
    }
    printf("当前设置分辨率: %d x %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

    // 4. 申请缓冲区 (Request Buffers)
    // 告诉内核我们要申请 1 个用于 mmap 的缓冲区
    memset(&req, 0, sizeof(req));
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("申请缓冲区失败");
        close(fd);
        return -1;
    }

    // 5. 查询缓冲区信息并建立内存映射 (mmap)
    // 这一步是实现“零拷贝”的关键：将内核地址映射到用户空间指针
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
        perror("查询缓冲区信息失败");
        close(fd);
        return -1;
    }

    // 映射内存：buffer_start 就是我们直接读取图像数据的指针
    buffer_start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
    if (buffer_start == MAP_FAILED) {
        perror("mmap 失败");
        close(fd);
        return -1;
    }

    // 6. 缓冲区入队 (Queue Buffer)
    // 告诉驱动：这个映射好的 Buffer 是空的，你可以往里写数据了
    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        perror("入队失败");
        munmap(buffer_start, buf.length);
        close(fd);
        return -1;
    }

    // 7. 开启视频流
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("开启视频流失败");
        munmap(buffer_start, buf.length);
        close(fd);
        return -1;
    }

    printf("采集开始，等待数据...\n");

    // 8. 取出缓冲区 (Dequeue Buffer)
    // 程序会在此阻塞，直到硬件填满一帧图像
    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        perror("获取图像数据失败");
    } else {
        // 9. 保存数据到本地文件
        FILE *fp = fopen("output.yuv", "wb");
        if (fp) {
            // 注意：使用 buf.bytesused 而不是 buf.length，因为实际数据可能较小
            fwrite(buffer_start, buf.bytesused, 1, fp);
            fclose(fp);
            printf("成功抓取一帧！保存到: output.yuv (大小: %d 字节)\n", buf.bytesused);
        } else {
            perror("创建文件失败");
        }
    }

    // 10. 收尾清理
    ioctl(fd, VIDIOC_STREAMOFF, &type);
    munmap(buffer_start, buf.length);
    close(fd);

    printf("程序退出。\n");
    return 0;
}
