#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define VIDEO_PATH "/dev/video0"

int main() {

  int video_fd;
  video_fd = open(VIDEO_PATH, O_RDWR);

  struct v4l2_capability video_cap;
  struct v4l2_format video_fmt;

  ioctl(video_fd, VIDIOC_QUERYCAP, &video_cap);

  printf("video name is %s \n", video_cap.card);

  close(video_fd);

  printf("hello world\n");

  return 0;
}