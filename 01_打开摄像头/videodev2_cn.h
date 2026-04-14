/*
 *  V4L2 (Video for Linux Two) 核心头文件 (应用开发精选中文版)
 *  
 *  【特别标注：数据格式说明】
 *  - [字符串] : __u8 数组，使用 printf("%s", ...) 打印。
 *  - [数值]   : __u32 整数，使用 printf("%u", ...) 打印。
 *  - [位掩码] : __u32 整数，使用 & 运算符判断功能，如 (caps & V4L2_CAP_VIDEO_CAPTURE)。
 *  - [十六进制]: __u32 整数，通常用于像素格式，使用 printf("%08x", ...) 或特定的 4CC 转换。
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
	__u8	driver[16];	/* [字符串] 驱动名称 (如: "uvcvideo") */
	__u8	card[32];	/* [字符串] 设备名称 (如: "Integrated Camera") */
	__u8	bus_info[32];	/* [字符串] 总线信息 (如: "usb-0000:00:14.0-1") */
	__u32   version;	/* [数值]   驱动版本号 (内核版本编码) */
	__u32	capabilities;	/* [位掩码] 设备支持的总能力 (物理层+驱动层) */
	__u32	device_caps;	/* [位掩码] 当前节点支持的能力 (应用层主要看这个) */
	__u32	reserved[3];	/* [保留]   固定为 0 */
};

/* 常用能力标志 (用于判断 capabilities 和 device_caps) */
#define V4L2_CAP_VIDEO_CAPTURE		0x00000001  /* 支持视频采集 */
#define V4L2_CAP_STREAMING		0x04000000  /* 支持流操作 (mmap 映射方式) */
#define V4L2_CAP_READWRITE              0x01000000  /* 支持传统的 read/write 方式 */


/* -------------------------------------------------------------------------
 * 2. 视频数据格式 (Video Format)
 * -------------------------------------------------------------------------
 * 调用: ioctl(fd, VIDIOC_S_FMT, &fmt) - 设置格式
 */
struct v4l2_pix_format_cn {
	__u32         		width;		/* [数值]   图像宽度 (单位: 像素) */
	__u32         		height;		/* [数值]   图像高度 (单位: 像素) */
	__u32         		pixelformat;	/* [十六进制] 像素格式 (如 V4L2_PIX_FMT_YUYV) */
	__u32			field;		/* [数值]   扫描方式 (通常填 V4L2_FIELD_NONE) */
	__u32            	bytesperline;	/* [数值]   每行字节数 (由驱动计算返回) */
	__u32          		sizeimage;	/* [数值]   一帧图像的总大小 (由驱动计算返回) */
	__u32			colorspace;	/* [数值]   色彩空间 (如 V4L2_COLORSPACE_SRGB) */
	__u32			priv;		/* [数值]   私有数据 (通常填 0) */
};

struct v4l2_format_cn {
	__u32	 type;	/* [数值]   缓冲区类型 (通常填 V4L2_BUF_TYPE_VIDEO_CAPTURE) */
	union {
		struct v4l2_pix_format_cn	pix;     /* [结构体] 像素级格式定义 */
		__u8	raw_data[200];		         /* [字节流] 占位符 */
	} fmt;
};


/* -------------------------------------------------------------------------
 * 3. 缓冲区请求 (Buffer Request)
 * -------------------------------------------------------------------------
 * 调用: ioctl(fd, VIDIOC_REQBUFS, &req)
 */
struct v4l2_requestbuffers_cn {
	__u32			count;	 /* [数值]   想要申请的缓冲区个数 (如: 4) */
	__u32			type;	 /* [数值]   缓冲区类型 (同 v4l2_format.type) */
	__u32			memory;	 /* [数值]   内存类型 (通常填 V4L2_MEMORY_MMAP) */
	__u32			reserved[2];
};


/* -------------------------------------------------------------------------
 * 4. 缓冲区信息 (Buffer Info)
 * -------------------------------------------------------------------------
 * 调用: ioctl(fd, VIDIOC_QUERYBUF, &buf)
 */
struct v4l2_buffer_cn {
	__u32			index;		/* [数值]   缓冲区索引编号 (0, 1, 2...) */
	__u32			type;		/* [数值]   缓冲区类型 */
	__u32			bytesused;	/* [数值]   该缓冲区已存入的图像大小 (字节) */
	__u32			flags;		/* [位掩码] 缓冲区状态 (如: 是否已入队) */
	__u32			field;
	struct timeval		timestamp;	/* [结构体] 采集到图像时的时间戳 */
	__u32			sequence;	/* [数值]   该帧在视频流中的序号 */

	__u32			memory;		/* [数值]   内存类型 */
	union {
		__u32           offset;		/* [数值]   内存偏移量 (用于 mmap 函数) */
		unsigned long   userptr;
	} m;
	__u32			length;		/* [数值]   整个缓冲区的长度 (字节) */
	__u32			reserved2;
	__u32			reserved;
};

#endif /* __LINUX_VIDEODEV2_CN_H */
