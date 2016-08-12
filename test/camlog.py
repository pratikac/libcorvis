import lcm
from corvis import image_t

import cv2
import numpy as np
import os, sys
import subprocess as sp
import argparse

parser = argparse.ArgumentParser(description='Log LCM camera channel to video',
                conflict_handler='resolve')
parser.add_argument('-c', '--channel',  help='LCM channel name (image_t)', type=str)
parser.add_argument('-o', '--output',   help='Output video (.mp4)', type=str)
parser.add_argument('-w', '--width',    help='Output width',    type=int)
parser.add_argument('-h', '--height',   help='Output height',   type=int)
parser.add_argument('-f', '--fps',      help='Output FPS',      type=int)
parser.add_argument('-v', '--verbose',  help='Show video',      action='store_true')
parser.set_defaults(verbose=False)
args = parser.parse_args()

args = vars(args)
args['size'] = (args['height'], args['width'])
print args

idx, last_time, fps = 0, 0, None

win = cv2.namedWindow(args['channel'])
proc = None

burn = 30

def handler(c, d):
    global idx, last_time, fps, proc

    msg = image_t.decode(d)
    data = np.array(bytearray(msg.data))
    img = cv2.imdecode(data, cv2.CV_LOAD_IMAGE_COLOR)
    
    # use the first few frames to estimate freq and frame_size
    if idx < burn:
        if not args['width'] and not args['height']:
            args['width'], args['height'] = img.shape[1], img.shape[0]
            args['size'] = (args['height'], args['width'])

        if not args['fps']:
            dt = (msg.utime - last_time)/float(1e6)
            last_time = msg.utime
            if fps:
                fps = (fps*idx + int(1.0/dt))/(idx+1)
            else:
                fps = int(1.0/dt)
    elif idx == burn:
        if not args['fps']:
            args['fps'] = fps

        print 'Initializing ffmpeg: ', args

        ffmpeg_cmd = ['ffmpeg',
        '-y',
        '-f', 'rawvideo',
        '-vcodec', 'rawvideo',
        '-s', '%dx%d'%(args['size'][1], args['size'][0]),
        '-pix_fmt', 'bgr24',
        '-r', '%d'%args['fps'],
        '-i', '-',          # piped input
        '-an',              # no audio
        '-preset', 'slow',
        '-crf', '10',
        '-vcodec', 'libx264',
        args['output']]
        proc = sp.Popen(ffmpeg_cmd, stdin=sp.PIPE, stderr=sp.PIPE)
    else:
        pass

    img = cv2.resize(img, (args['size'][1], args['size'][0]))
    if proc:
        proc.stdin.write(img.tostring())

    if args['verbose']:
        cv2.imshow(args['channel'], img)
        cv2.waitKey(10)

    idx = idx + 1

lc = lcm.LCM()
lc.subscribe(args['channel'], handler)

try:
    while True:
        lc.handle()

except KeyboardInterrupt:
    cv2.destroyAllWindows()
    cv2.waitKey(1)
    pass
