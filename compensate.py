import numpy as np
import cv2
from pprint import pprint
import copy
import time

P = [262.775, 123.2375, 98.4775, 120.6349,
     344.1349, 31.7649, 0.1934, 21.3641, 514.1541]

inv_P = [0.00452944, -0.00157421, -0.00077028, -0.00159373,
         0.00347093, 0.00009081, 0.00006452, -0.00014363, 0.00194146]


P = np.array(P).reshape(3, 3)
inv_P = np.array(inv_P).reshape(3, 3)


def getDP1RsGsBs():
    #DP1_RsGsBs_txt = open('RsGsBs_Table_DP1.txt', 'r')
    DP1_RsGsBs_txt = open('RsGsBs_Table_DP12.txt', 'r')
    lines = DP1_RsGsBs_txt.readlines()
    for i in range(len(lines)):
        lines[i] = lines[i].rstrip()
        DP1_Rs.append([254 - 2*i, float(lines[i].split('\t')[0])])
        DP1_Gs.append([254 - 2*i, float(lines[i].split('\t')[1])])
        DP1_Bs.append([254 - 2*i, float(lines[i].split('\t')[2])])
        DP1_RsGsBs_txt.close()


def RGB_to_RsGsBs(r, g, b):
    tmp_rs = copy.deepcopy(DP1_Rs)
    tmp_gs = copy.deepcopy(DP1_Gs)
    tmp_bs = copy.deepcopy(DP1_Bs)
    for i in range(len(tmp_rs)):
        tmp_rs[i][0] = abs(tmp_rs[i][0] - r)
        tmp_gs[i][0] = abs(tmp_gs[i][0] - g)
        tmp_bs[i][0] = abs(tmp_bs[i][0] - b)
    tmp_rs = sorted(tmp_rs, key=lambda item: item[0])
    tmp_gs = sorted(tmp_gs, key=lambda item: item[0])
    tmp_bs = sorted(tmp_bs, key=lambda item: item[0])

    return tmp_rs[0][1], tmp_gs[0][1], tmp_bs[0][1]


def RsGsBs_to_XYZ(rs, gs, bs):
    rsgsbs = np.array([rs, gs, bs]).reshape(3, 1)
    tmp = P@rsgsbs
    return tmp[0][0], tmp[1][0], tmp[2][0]


def XYZ_to_RsGsBs(x, y, z):
    xyz = np.array([x, y, z]).reshape(3, 1)
    tmp = inv_P@xyz
    return tmp[0][0], tmp[1][0], tmp[2][0]


def RsGsBs_to_RGB(rs, gs, bs):
    tmp_rs = copy.deepcopy(DP1_Rs)
    tmp_gs = copy.deepcopy(DP1_Gs)
    tmp_bs = copy.deepcopy(DP1_Bs)
    for i in range(len(tmp_rs)):
        tmp_rs[i][1] = abs(tmp_rs[i][1] - rs)
        tmp_gs[i][1] = abs(tmp_gs[i][1] - gs)
        tmp_bs[i][1] = abs(tmp_bs[i][1] - bs)
    tmp_rs = sorted(tmp_rs, key=lambda item: item[1])
    tmp_gs = sorted(tmp_gs, key=lambda item: item[1])
    tmp_bs = sorted(tmp_bs, key=lambda item: item[1])

    return tmp_rs[0][0], tmp_gs[0][0], tmp_bs[0][0]


def compensation(r, g, b, percentage):
    rs, gs, bs = RGB_to_RsGsBs(r, g, b)
    x, y, z = RsGsBs_to_XYZ(rs, gs, bs)  # XYZ 계산
    print("곱하기전 xyz", end=' ')
    print(x, y, z)
    x, y, z = (1+percentage/100)*x, (1+percentage/100)*y, (1+percentage/100)*z
    print("곱하기후 xyz", end=' ')
    print(x, y, z)
    new_rs, new_gs, new_bs = XYZ_to_RsGsBs(x, y, z)  # 새로운 Rs, Gs, Bs 계산
    new_r, new_g, new_b = RsGsBs_to_RGB(new_rs, new_gs, new_bs)

    return new_r, new_g, new_b


if __name__ == "__main__":
    DP1_Rs, DP1_Gs, DP1_Bs = [], [], []
    getDP1RsGsBs()

    print(compensation(200, 100, 50, 20))
    p = float(input('보상 퍼센테이지 입력: \n'))

    img = cv2.imread('cyan200.png', cv2.IMREAD_COLOR)

    h, w, c = img.shape
    start = time.time()
    for i in range(h):
        for j in range(w):
            new_r, new_g, new_b = compensation(
                img[i][j][2], img[i][j][1], img[i][j][0], p)
            img.itemset(i, j, 0, new_b)
            img.itemset(i, j, 1, new_g)
            img.itemset(i, j, 2, new_r)

    cv2.imshow('img', img)
    k = cv2.waitKey(0)
    if k == 27:
        cv2.destroyAllWindows()
        cv2.imwrite('compenstaed_color.png', img)
        end = time.time()
        print(end-start)
