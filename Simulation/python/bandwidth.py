import sys
import pandas
import matplotlib.pyplot as plt
import configparser

SCALE = 0

CONFIG_PATH = sys.argv[1]
config = configparser.ConfigParser()
config.read(CONFIG_PATH)

packet_size = int(config['traffic']['packet_size'])
mean = int(config['traffic']['mean'])
standard_deviation = int(config['traffic']['standard_deviation'])
input_rate = int(config['traffic']['input_rate'])

FOLDER_PATH = config['simulation']['data_path']
tenant_number = int(config['simulation']['tenant_number'])

DATA_PATH = FOLDER_PATH+'/packets.csv'
dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()

x_axis = []
y_axis_origin = []
y_axis_after = []

# step_size = 476.8715820312
step_size = packet_size*10**9/(input_rate)
print(step_size)
for ind, row in enumerate(data):
    x_axis.append(ind*step_size)
    y_a = 0
    y_o = 0
    for y in row:
        if(y>-1):
            y_o += 1
        if(y==0):
            y_a += 1
    if(SCALE == 1):
        y_axis_after.append(y_a)
        y_axis_origin.append(y_o)
    else:
        y_axis_after.append(y_a*packet_size*(10**9)/(1024*1024*step_size))
        y_axis_origin.append(y_o*packet_size*(10**9)/(1024*1024*step_size))

if(SCALE == 1):
    print(len(y_axis_after))
    x_axis_ = []
    y_axis_origin_ = []
    y_axis_after_ = []

    for ind in range(int(len(y_axis_after)/10)):
        y_axis_origin_.append(sum([y_axis_origin[ind+i] for i in range(10)])*packet_size*(10**9)/(1024*1024*step_size*10))
        y_axis_after_.append(sum([y_axis_after[ind+i] for i in range(10)])*packet_size*(10**9)/(1024*1024*step_size*10))
        x_axis_.append(ind*step_size*10)

IMAGE_PATH = FOLDER_PATH+'/bandwidth_origin.png'

if(SCALE == 1):
    plt.plot(x_axis_, y_axis_origin_, linestyle='-', color='blue', label='Bandwidth(Origin)')
    plt.plot(x_axis_,[mean*tenant_number  for i in x_axis_], linestyle='-', color='red', label='avg')
    plt.plot(x_axis_,[(mean+standard_deviation)*tenant_number  for i in x_axis_], linestyle='-', color='red', label='upper')
else:
    plt.plot(x_axis, y_axis_origin, linestyle='-', color='blue', label='Bandwidth(Origin)')
    plt.plot(x_axis,[mean*tenant_number  for i in x_axis], linestyle='-', color='red', label='avg')
    plt.plot(x_axis,[(mean+standard_deviation)*tenant_number  for i in x_axis], linestyle='-', color='red', label='upper')

plt.title('Bandwidth')
plt.xlabel('Time')
plt.ylabel('Bandwidth(Mbps)')
plt.legend()
plt.grid(True)
plt.savefig(IMAGE_PATH)
plt.cla()

IMAGE_PATH = FOLDER_PATH+'/bandwidth_after.png'

if(SCALE == 1):
    plt.plot(x_axis_, y_axis_after_, linestyle='-', color='blue', label='Bandwidth(After)')
    plt.plot(x_axis_,[mean*tenant_number  for i in x_axis_], linestyle='-', color='red', label='avg')
    plt.plot(x_axis_,[(mean+standard_deviation)*tenant_number  for i in x_axis_], linestyle='-', color='red', label='upper')
else:
    plt.plot(x_axis, y_axis_after, linestyle='-', color='blue', label='Bandwidth(After)')
    plt.plot(x_axis,[mean*tenant_number  for i in x_axis], linestyle='-', color='red', label='avg')
    plt.plot(x_axis,[(mean+standard_deviation)*tenant_number  for i in x_axis], linestyle='-', color='red', label='upper')
plt.title('Bandwidth')
plt.xlabel('Time')
plt.ylabel('Bandwidth(Mbps)')
plt.legend()
plt.grid(True)
plt.savefig(IMAGE_PATH)
plt.cla()