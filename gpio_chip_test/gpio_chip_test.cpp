/* Copyright (c) 2019 Piaggio Fast Forward (PFF), Inc.
All Rights Reserved. Reproduction, publication,
or re-transmittal not allowed without written permission of PFF, Inc. */

#include <string>
#include <cstring>
#include <unistd.h>
#include <csignal>

#include <chrono>
#include <thread>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
  struct gpiohandle_request req;
  struct gpiohandle_data data;

  std::string fdName("/dev/gpiochip");
  fdName += std::to_string(0);
  auto f = open(fdName.c_str(), O_RDWR);
  req.lineoffsets[0] = 0;
  req.flags = GPIOHANDLE_REQUEST_OUTPUT; /* Request as output */
  req.default_values[0] = 0;
  req.lines = 1;

  /* Note that there is a new file descriptor in req.fd to handle the GPIO lines */
  auto ret = ioctl(f, GPIO_GET_LINEHANDLE_IOCTL, &req);
  if (ret < 0)
  {
    printf("ioclt error: %d\n", perror);
  }

  close(f);

  data.values[0] = 1;
  ret = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
  if (ret < 0)
  {
    printf("ioclt error: %d\n", perror);
  }

  std::this_thread::sleep_for(2s);

  data.values[0] = 0;
  ret = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
  if (ret < 0)
  {
    printf("ioclt error: %d\n", perror);
  }

  std::this_thread::sleep_for(2s);

  // struct gpiochip_info info;

  // auto fd = open("/dev/gpiochip0", O_RDWR);
  // printf("open\n");

  // ioctl(fd, GPIO_GET_CHIPINFO_IOCTL, info);

  // printf("name[0]: %s\n", info.name[0]);

  // close(fd);

  return 0;
}
