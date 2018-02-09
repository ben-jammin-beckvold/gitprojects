#include "timestamp.h"
#include <chrono>
#include <unistd.h>
#include <ros/ros.h>

using namespace std::chrono;

namespace timestamp_def
{
  const uint32_t    pub_interval = 60;
  const std::string pub_topic    = "heartbeat";
}

unsigned long long int millisSinceEpoch()
{
  return duration_cast<milliseconds>(
    high_resolution_clock::now().time_since_epoch()
  ).count();
}

void formatTimestamp(pff_embedded::EmbeddedTimestamp &t)
{
  auto ticks = millisSinceEpoch();
  t.seconds = ticks / 1000; // truncation intended
  t.millis = ticks - (t.seconds * 1000);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "~");
  ros::NodeHandle nh("~");

  int32_t pub_interval = timestamp_def::pub_interval;
  std::string pub_topic = timestamp_def::pub_topic;
  nh.param<std::string>("timestamp_topic", pub_topic, pub_topic);
  nh.param<int32_t>("publish_interval", pub_interval, pub_interval);

  // latch the message so the IO board gets the time as soon as it can
  ros::Publisher pub_time = nh.advertise<pff_embedded::EmbeddedTimestamp>(pub_topic, 1, true);
  pff_embedded::EmbeddedTimestamp stamp;

  while (ros::ok())
  {
    formatTimestamp(stamp);
    pub_time.publish(stamp);
    ros::Duration(pub_interval).sleep();
    ros::spinOnce();
  }
  return 0;
}
