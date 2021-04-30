#!/usr/bin/python3.5
# -*-utf-8 -*

import numpy as np
import cv2
 
cap = cv2.VideoCapture(0)
 
while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
 
    # Our operations on the frame come here
    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
 
    # Display the resulting frame
    frame = cv2.resize(frame, (96,72), interpolation = cv2.INTER_CUBIC)#cv2.INTER_LINEAR)
    frame = cv2.resize(frame, (320,240), interpolation = cv2.INTER_AREA)#cv2.INTER_LINEAR)
    
    cv2.imshow('frame',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
 
# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
