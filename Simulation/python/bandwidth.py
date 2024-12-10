import sys
import pandas
import matplotlib.pyplot as plt
import configparser

DATA_PATH = '../data/simple_V3/packets.csv'
IMAGE_PATH = '../data/simple_V3/bandwidth.png'

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()

index = 0

# packet_count = [0 for i in data[0]]

# for row in data:
#     for i,y in enumerate(row):
#         if(y>-1):
#             packet_count[i] += 1

# for i in range(len(data[0])):
#     print("Tenant [{}] : {:.10f} %".format(i, packet_count[i]*100/1953125))
            

x_axis = []
y_axis = []
# print(data[0])
# print(sum(data[0]))
for ind, row in enumerate(data):
    x_axis.append(ind*512)
    y_a = 0
    for y in row:
        # if(y>-1):
        if(y==0):
            y_a += 1
            # packet_count += 1
    y_axis.append(y_a*(10**9)/(1024*1024))
    # y_axis.append((sum(row)+len(row))*(10**9)/(1024*1024))



# print((sum(y_axis)*1024*1024/(10**9))/195312500)
# ratio = packet_count/1953125
# print("Total packets : {:.8f}, Ratio : {:.8f} %".format(packet_count, ratio))
# step_size = 125
# x_axis_ = []
# y_axis_ = []
# for ind in range(int(len(y_axis)/2)):
#     x_axis_.append(ind*512*step_size)
#     y = 0
#     for ind_ in range(step_size):
#         y += y_axis[ind+ind_]
#     y_axis_.append(y/step_size)
# print((10**9)/(1024*1024))

# print(sum(y_axis))
# print(sum(y_axis)/195312500)
# print(y_axis)

# for y in y_axis:
#     if(y>1):
#         print(y)
plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='Bandwidth(After)')
plt.plot(x_axis,[12000 for i in x_axis], linestyle='-', color='red', label='avg')
plt.plot(x_axis,[16000 for i in x_axis], linestyle='-', color='red', label='upper')
plt.title('Bandwidth')
plt.xlabel('Time')
plt.ylabel('Bandwidth')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()