#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define VIDEO_PATH "/dev/video0"

int main() {

  int video_fd;
  video_fd = open(VIDEO_PATH, O_RDWR);

  struct v4l2_capability video_cap;
  struct v4l2_format video_fmt;

  memset(&video_fmt, 0, sizeof(video_fmt));
  video_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  ioctl(video_fd, VIDIOC_QUERYCAP, &video_cap);

  printf("video name is %s \n", video_cap.card);
  ioctl(video_fd, VIDIOC_G_FMT, &video_fmt);

  printf("当前分辨率是 %d * %d \n", video_fmt.fmt.pix.width,
         video_fmt.fmt.pix.height);
  char *p = (char *)&video_fmt.fmt.pix.pixelformat;
  printf("当前像素格式: %c%c%c%c\n", p[0], p[1], p[2], p[3]);
  close(video_fd);

  printf("hello world\n");

  return 0;
}