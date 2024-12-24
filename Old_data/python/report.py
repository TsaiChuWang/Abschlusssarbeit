import sys
import pandas
import matplotlib.pyplot as plt
import configparser

DATA_PATH = '../data/simple_V3/packets.csv'
IMAGE_PATH = '../data/report.png'

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()

index = 0

x_axis = []
y_axis = []

for ind, row in enumerate(data):
    x_axis.append(ind*512)
    y_a = 0
    for y in row:
      if(y==0):
          y_a += 1
    y_axis.append(y_a)

packet_count = [0 for i in data[0]]
for y in y_axis:
    packet_count[y] += 1

max_y = max(y_axis)
y_axis = packet_count[:max_y]
x_axis = [i for i in range(max_y)]
print(packet_count)

plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='Packets')
plt.title('Packets')
plt.xlabel('Number')
plt.ylabel('Packets')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()