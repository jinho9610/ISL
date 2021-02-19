for r in range(0, 256, 8):
    #     for g in range(0, 256, 8):
    #         for b in range(0, 256, 8):
    #             cnt += 1
    #             # info = calDeltaE(r, g, b, 20)  # 색차
    #             info = stretch_calDeltaE(r, g, b, 50)  # 스트레처블 구조에서 색차
    #             min_L = min(min_L, round(info[6], 3))
    #             max_L = max(max_L, round(info[6], 3))
    #             min_d = min(min_d, round(info[-1], 3))
    #             max_d = max(max_d, round(info[-1], 3))
    #             total_d += round(info[16], 3)
    #             f.write(f'{info[0]} {info[1]} {info[2]} {round(info[3], 3)} {round(info[4], 3)} {round(info[5], 3)} {round(info[6], 3)} {round(info[7], 3)} {round(info[8], 3)} {round(info[9], 3)} {round(info[10], 3)} {round(info[11], 3)} {round(info[12], 3)} {round(info[13], 3)} {round(info[14], 3)} {info[15]} {round(info[16], 3)}\n')
    #             dataset.append((r, g, b, 20, round(info[16], 3)))

    # dataset.sort(key=lambda d: -d[4])

    # f.write("\n\n")
    # f.write(
    #     f'{dataset[0][0]} {dataset[0][1]} {dataset[0][2]} {dataset[0][3]} {dataset[0][4]}\n')

    # f.write(f'{min_d} {max_d}')
    # print(total_d/cnt)

    # end = time.time()