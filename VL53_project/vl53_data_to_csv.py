#! /usr/bin/env python

import rospy
from pff_range_msg.msg import RangeList

INTERVAL = .1

ranger_fb_topic = "/RangeList"

feedback_file = '/home/exec/Documents/vl53_cold_results.csv'

class Listener(object):

    def __init__(self):
        rospy.init_node("listener")
        self.r_fb_sub = rospy.Subscriber(
            ranger_fb_topic, RangeList, self.ranger_fb_cb
        )        

    def ranger_fb_cb(self, msg):
        
        time_now = rospy.get_time()
        if (time_now - self.time_lastRecord) > INTERVAL:
            self.time_lastRecord = time_now
            time_stamp = time_now - self.time_last
            print('{:.0f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}\n'.format(time_stamp, msg.range[0], msg.range[1], msg.range[2], msg.range[3]))
            f = open(feedback_file, 'a')
            f.write('{:.0f}, {:.4f}, {:.4f}, {:.4f}, {:.4f}\n'.format(time_stamp, msg.range[0], msg.range[1], msg.range[2], msg.range[3]))
            f.close()


    def run(self):
        rate = rospy.Rate(25)
        self.time_last = rospy.get_time()
        self.time_lastRecord = rospy.get_time()
        #print (self.time_last)


        while not rospy.is_shutdown():
            rate.sleep()

if __name__ == '__main__':
    Listener().run()
