'''Info_maker.py를 이용하여 만든 txt 파일을
    L 구간에 따라서 (a, b) 분포를 2차원 평면으로 나타내는 코드'''

from matplotlib import pyplot as plt


def make_chart(f, l, arr):  # L이 p * 10 ~ (p + 1) * 10에 속하는 경우의 (a, b) 분포를 plt로 그림
    print(len(arr))
    plt.figure(num=f, figsize=(6, 6))
    plt.axvline(x=0, c='k')
    plt.axhline(y=0, c='k')
    plt.xlim([-150, 150])
    plt.xlabel('a*')
    plt.ylim([-150, 150])
    plt.ylabel('b*')
    if f > 0:
        plt.title(f'L range: {l * 10} ~ {(l+1) * 10}')
    else:
        plt.title(f'L range: {-(l+1) * 10} ~ {-l * 10}')

    for i in range(len(arr)):
        hex_c = '#' + format(int(arr[i][2]), '02x') + format(int(arr[i]
                                                                 [3]), '02x') + format(int(arr[i][4]), '02x')
        plt.scatter(arr[i][0], arr[i][1], c=hex_c)

    if f > 0:
        plt.savefig(f'{l * 10} ~ {(l+1) * 10}.png')
    else:
        plt.savefig(f'{-(l+1) * 10} ~ {-l * 10}.png')


def getAvgD(arr):
    sum = 0
    for i in range(len(arr)):
        sum += arr[i]
    avg = sum / len(arr)

    return avg


def Datas_Histogram(datas):
    plt.figure(figsize=(10, 6))
    # plt.xlim([-30, 110])
    # plt.ylim([0, 10])
    idx = range(0, 10, 1)
    plt.grid(True, axis='y')
    ax = plt.subplot()
    ax.set_xticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])
    ax.set_xticklabels(['0~10', '10~20', '20~30', '30~40',
                        '40~50', '50~60', '60~70', '70~80', '80~90', '90~100'])
    ax.set_yticks([0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5,
                   4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8])
    plt.bar(idx, datas, align='center')
    plt.ylabel('delta E')
    plt.show()


def dE_Histogram(datas):
    datas
    plt.figure(figsize=(10, 6))
    # plt.xlim([-30, 110])
    # plt.ylim([0, 10])
    idx = range(0, 12, 1)
    #plt.grid(True, axis='y')
    # ax = plt.subplot()
    # ax.set_xticks([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11])
    # ax.set_xticklabels(['0~1', '1~2', '2~3', '3~4',
    #                     '4~5', '5~6', '6~7', '7~8', '8~9', '9~10', '10~11', '11~12'])
    # # ax.set_yticks([0, 10, 20, 30, 40, 50, 60, 70,
    # #                80, 90, 100])
    plt.bar(idx, datas, align='center')
    plt.ylabel('delta E ratio')
    plt.show()


if __name__ == '__main__':
    min_L = 0
    max_L = 0
    data = open('./RGBP_info.txt', mode='rt', encoding='utf-8')

    while True:
        line = data.readline()

        if line == "":
            break

        infos = list(map(float, line.split()))

        # 첫번째 줄인 경우에는 최소 L, 최대 L이 필요함
        if len(infos) == 2:
            min_d = infos[0]
            max_d = infos[1]

            #d_arr = [[]*1 for _ in range(int(max_L / 10) + 1)]
            d_arr = [[]*1 for _ in range(int(max_d) + 1)]

        else:
            d_arr[int(abs(infos[-1]))].append(infos[-1])

    datas, de_datas = [], []
    total_cnt = 0
    print(len(d_arr))
    for i in range(len(d_arr)):
        total_cnt += len(d_arr[i])
        # datas.append(getAvgD(d_arr[i]))

    for i in range(len(d_arr)):
        for j in range()
        de_datas.append(len(d_arr[i]) / total_cnt)

    print(len(de_datas))

    print(total_cnt)
    # Datas_Histogram(datas)
    dE_Histogram(de_datas)

    # for i in range(len(L_arr_pos)):
    #     make_chart(i + 1, i, L_arr_pos[i])
    # for i in range(len(L_arr_neg)):
    #     make_chart(-(i + 1), i, L_arr_neg[i])

    # plt.show()
