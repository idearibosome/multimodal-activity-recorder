import argparse
import os

import cv2 as cv
import numpy as np


# parser
parser = argparse.ArgumentParser()

parser.add_argument('--input_path1', help='Path of the input images 1', type=str)
parser.add_argument('--input_path2', help='Path of the input images 2', type=str)
parser.add_argument('--output_name', help='Name of the output annoation file', type=str, default='output.csv')
parser.add_argument('--annotation_name', help='Name of the annotation', type=str, default='annotation')
parser.add_argument('--fps', help='Fps', type=float, default=5)

args = parser.parse_args()


# find images
image_name_list1 = [f for f in os.listdir(args.input_path1) if f.lower().endswith('.png')]
image_name_list2 = [f for f in os.listdir(args.input_path2) if f.lower().endswith('.png')]


# parse image names
timestamp_list1 = []
for image_name in image_name_list1:
  timestamp = int(os.path.splitext(image_name)[0])
  timestamp_list1.append(timestamp)
timestamp_list1.sort()

timestamp_list2 = []
for image_name in image_name_list2:
  timestamp = int(os.path.splitext(image_name)[0])
  timestamp_list2.append(timestamp)
timestamp_list2.sort()


# annotate images
index1 = 0
index2 = 0
target_timestamp = 0
timestamp_step = 1000.0 / args.fps

target_timestamp += timestamp_step

with open(args.output_name, 'w') as f:
  f.write('timestamp,%s\n' % (args.annotation_name))
  f.flush()

  while True:
    while (index1 < len(timestamp_list1) and target_timestamp > timestamp_list1[index1]):
      index1 += 1
    while (index2 < len(timestamp_list2) and target_timestamp > timestamp_list2[index2]):
      index2 += 1
    
    if (index1 >= len(timestamp_list1)):
      break
    if (index2 >= len(timestamp_list2)):
      break
    
    image1 = cv.imread(os.path.join(args.input_path1, str(timestamp_list1[index1])+'.png'))
    cv.imshow('image1', image1)

    image2 = cv.imread(os.path.join(args.input_path2, str(timestamp_list2[index2])+'.png'))
    cv.imshow('image2', image2)

    retval = cv.waitKey(0)
    label = retval - 48 # '0'
    f.write('%d,%d\n' % (target_timestamp, label))

    target_timestamp += timestamp_step
