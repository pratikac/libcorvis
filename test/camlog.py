import lcm
from corvis import image_t

import cv2
import numpy as np
import os, sys

if not len(sys.argv) == 2:
    print 'Usage: python camlog.py [channel name]'
    sys.exit(1)

idx = 0
last_time = 0

win = cv2.namedWindow(sys.argv[1].lower())

def handler(c, d):
    msg = image_t.decode(d)

    global last_time, idx

    dt = (msg.utime - last_time)/float(1e6)
    last_time = msg.utime

    data = np.array(bytearray(msg.data))

    tmp = cv2.imdecode(data, cv2.CV_LOAD_IMAGE_COLOR)
    cv2.imshow(sys.argv[1].lower(), tmp)
    cv2.waitKey(1)

    #cv2.imwrite('data/chess_%05d.jpg'%idx, tmp)

    idx = idx + 1

lc = lcm.LCM()
lc.subscribe(sys.argv[1], handler)

try:
    while True:
        lc.handle()

except KeyboardInterrupt:
    cv2.destroyAllWindows()
    cv2.waitKey(1)
    pass
