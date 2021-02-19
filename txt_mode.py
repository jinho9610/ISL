
data = open('input.txt', 'r', encoding='utf-8')

a = []
while True:
    line = data.readline()

    if line == "":
        break

    a.append(int(line.split()[0]))

freq = [0] * 21
for i in range(len(a)):
    freq[a[i]] += 1

freqs = []
for i in range(len(freq)):
    freqs.append((i, freq[i]))

freqs = sorted(freqs, key=lambda x: -x[1])

for i in range(len(freqs)):
    print(freqs[i])
