import cv2
import numpy as np

intensity = int(input('사각형 색상 계조 값 입력\n'))

img = np.zeros((250, 250), dtype=np.uint8)
cv2.rectangle(img, (25, 25), (225, 225), (intensity, intensity, intensity), -1)

cv2.imshow('img', img)
k = cv2.waitKey(0)

if k == 27:
    cv2.destroyAllWindows()
    cv2.imwrite(str(intensity)+'gray_sqaure.png', img)
