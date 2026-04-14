/*
 *  V4L2 (Video for Linux Two) 核心头文件 (应用开发精选中文版)
 *  
 *  本文件包含做摄像头应用开发最常用的结构体和 ioctl 命令。
 *  由于原始头文件过长，这里提取了最核心的接口，并配以详细中文注释。
 */

#ifndef __LINUX_VIDEODEV2_CN_H
#define __LINUX_VIDEODEV2_CN_H

#include <linux/types.h>
#include <linux/time.h>
#include <linux/videodev2.h>

/* -------------------------------------------------------------------------
 * 1. 设备能力查询 (Query Capabilities)
 * -------------------------------------------------------------------------
 * 调用: ioctl(fd, VIDIOC_QUERYCAP, &cap)
 */
struct v4l2_capability_cn {
	__u8	driver[16];	/* 驱动名称 (例如: "uvcvideo") */
	__u8	card[32];	/* 设备名称 (例如: "Integrated Camera") */
	__u8	bus_info[32];	/* 总线信息 (例如: "usb-0000:00:14.0-1") */
	__u32   version;	/* 驱动版本号 */
	__u32	capabilities;	/* 设备支持的总能力 (V4L2_CAP_VIDEO_CAPTURE 等) */
	__u32	device_caps;	/* 当前打开的特定设备支持的能力 */
	__u32	reserved[3];	/* 保留字段 */
};

/* 常用能力标志 (capabilities 字段的值) */
#define V4L2_CAP_VIDEO_CAPTURE		0x00000001  /* 是否支持视频采集 */
#define V4L2_CAP_STREAMING		0x04000000  /* 是否支持流操作 (mmap) */


/* -------------------------------------------------------------------------
 * 2. 视频数据格式 (Video Format)
 * -------------------------------------------------------------------------
 * 调用: ioctl(fd, VIDIOC_S_FMT, &fmt) - 设置格式
 *       ioctl(fd, VIDIOC_G_FMT, &fmt) - 获取当前格式
 */
struct v4l2_pix_format_cn {
	__u32         		width;		/* 图像宽度 (像素) */
	__u32         		height;		/* 图像高度 (像素) */
	__u32         		pixelformat;	/* 像素格式 (V4L2_PIX_FMT_YUYV 等) */
	__u32			field;		/* 扫描方式 (通常为 V4L2_FIELD_NONE) */
	__u32            	bytesperline;	/* 每行字节数 (用于对齐) */
	__u32          		sizeimage;	/* 一帧图像占用的总字节数 */
	__u32			colorspace;	/* 色彩空间 */
	__u32			priv;		/* 私有数据 (通常设为 0) */
};

struct v4l2_format_cn {
	__u32	 type;	/* 缓冲区类型 (通常为 V4L2_BUF_TYPE_VIDEO_CAPTURE) */
	union {
		struct v4l2_pix_format_cn	pix;     /* 常用: 像素级格式 (单平面) */
		__u8	raw_data[200];		         /* 原始数据占位符 */
	} fmt;
};

/* 常用像素格式 (pixelformat 字段) */
#define V4L2_PIX_FMT_YUYV     v4l2_fourcc('Y', 'U', 'Y', 'V') /* YUYV 4:2:2 */
#define V4L2_PIX_FMT_MJPEG    v4l2_fourcc('M', 'J', 'P', 'G') /* 压缩的 MJPEG */
#define V4L2_PIX_FMT_RGB24    v4l2_fourcc('R', 'G', 'B', '3') /* RGB 8:8:8 */


/* -------------------------------------------------------------------------
 * 3. 缓冲区请求与内存管理 (Buffer Request)
 * -------------------------------------------------------------------------
 * 调用: ioctl(fd, VIDIOC_REQBUFS, &req)
 */
struct v4l2_requestbuffers_cn {
	__u32			count;	 /* 想要申请的缓冲区数量 */
	__u32			type;	 /* 缓冲区类型 (同 v4l2_format.type) */
	__u32			memory;	 /* 内存类型 (常用: V4L2_MEMORY_MMAP) */
	__u32			reserved[2];
};

/* 内存类型标志 (memory 字段) */
#define V4L2_MEMORY_MMAP             1  /* 内存映射 (最常用，效率最高) */
#define V4L2_MEMORY_USERPTR          2  /* 用户指针方式 */


/* -------------------------------------------------------------------------
 * 4. 缓冲区信息 (Buffer Info)
 * -------------------------------------------------------------------------
 * 用于将内核空间内存映射到用户空间 (mmap)
 * 调用: ioctl(fd, VIDIOC_QUERYBUF, &buf)
 */
struct v4l2_buffer_cn {
	__u32			index;		/* 缓冲区编号 (0 到 count-1) */
	__u32			type;		/* 缓冲区类型 */
	__u32			bytesused;	/* 缓冲区中已使用的字节数 (采集完一帧后有效) */
	__u32			flags;		/* 状态标志 (是否已入队等) */
	__u32			field;
	struct timeval		timestamp;	/* 采集到该帧的时间戳 */
	struct v4l2_timecode	timecode;
	__u32			sequence;	/* 帧序号 */

	__u32			memory;		/* 内存类型 */
	union {
		__u32           offset;		/* 常用: 该内存在设备内存中的偏移量 (用于 mmap) */
		unsigned long   userptr;
	} m;
	__u32			length;		/* 缓冲区长度 (字节) */
	__u32			reserved2;
	__u32			reserved;
};


/* -------------------------------------------------------------------------
 * 5. 核心 ioctl 命令汇总
 * -------------------------------------------------------------------------
 */

/* [查询] */
#define VIDIOC_QUERYCAP		/* 查询设备支持的功能 (能力) */
#define VIDIOC_ENUM_FMT         /* 枚举摄像头支持的所有像素格式 */
#define VIDIOC_ENUM_FRAMESIZES  /* 枚举某种格式下支持的所有分辨率 */

/* [格式设置] */
#define VIDIOC_G_FMT		/* 获取当前的图像格式 */
#define VIDIOC_S_FMT		/* 设置图像格式 (宽、高、像素格式等) */
#define VIDIOC_TRY_FMT		/* 尝试某种格式，内核会修正为最接近的支持值 */

/* [内存申请与流控] */
#define VIDIOC_REQBUFS		/* 向内核申请帧缓冲区 */
#define VIDIOC_QUERYBUF		/* 查询某个缓冲区的信息 (获取 offset 用于 mmap) */
#define VIDIOC_QBUF		/* 把空的缓冲区放入“采集队列” (入队) */
#define VIDIOC_DQBUF		/* 从“完成队列”中取出一个装满图像的缓冲区 (出队) */
#define VIDIOC_STREAMON		/* 启动视频采集流 (摄像头开始出水) */
#define VIDIOC_STREAMOFF	/* 停止视频采集流 */

/* [控制参数] */
#define VIDIOC_G_CTRL		/* 获取某个控制参数 (如亮度、对比度) */
#define VIDIOC_S_CTRL		/* 设置某个控制参数 */

#endif /* __LINUX_VIDEODEV2_CN_H */
