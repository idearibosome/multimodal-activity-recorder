import argparse
import os

import cv2 as cv
import numpy as np


# parser
parser = argparse.ArgumentParser()

parser.add_argument('--input_path', help='Path of the input images', type=str)
parser.add_argument('--output_name', help='Name of the output video file', type=str, default='output.mp4')
parser.add_argument('--fps', help='Fps', type=float, default=10)

args = parser.parse_args()


# find images
image_name_list = [f for f in os.listdir(args.input_path) if f.lower().endswith('.png')]


# parse image names
timestamp_list = []
for image_name in image_name_list:
  timestamp = int(os.path.splitext(image_name)[0])
  timestamp_list.append(timestamp)
timestamp_list.sort()


# load first image
image = cv.imread(os.path.join(args.input_path, image_name_list[0]))
image_height, image_width, _ = image.shape


# write video
fourcc = cv.VideoWriter_fourcc(*'MP4V')
writer = cv.VideoWriter(args.output_name, fourcc, args.fps, (image_width, image_height))

index = 0
target_timestamp = 0
timestamp_step = 1000.0 / args.fps

while True:
  while (index < len(timestamp_list) and target_timestamp > timestamp_list[index]):
    index += 1
  
  if (index >= len(timestamp_list)):
    break
  
  image = cv.imread(os.path.join(args.input_path, str(timestamp_list[index])+'.png'))
  writer.write(image)

  print('- %.2f: writing %d' % (target_timestamp, timestamp_list[index]))

  target_timestamp += timestamp_step

writer.release()