# importing networkx 
import networkx as nx
# importing matplotlib.pyplot
import matplotlib.pyplot as plt
import pandas as pd
import sys

MEAN = 120
COVARIANCE = 40

def line_chart_tenant(index):
    DATA_PATH = "../Datei/Simple_V3_MVP/traffic_{}.csv"
    IMAGE_PATH = "../Datei/Simple_V3_MVP/images/traffic_{}_linechart.png"

    dataframe = pd.read_csv(DATA_PATH.format(index))
    data = dataframe.values.tolist()

    if(index==0):
        label = 'A'
        color = 'g'
    elif(index==1):
        label ='B'
        color = 'b'
    else:
        label = 'C'
        color = 'r'

    plt.plot([index for index in range(len(data))], [float(row[0]) for row in data], linestyle='-', color=color, label=label)
    plt.plot([index for index in range(len(data))], [MEAN-COVARIANCE for row in data], linestyle='dotted', color='gray', label='lower bound')
    plt.plot([index for index in range(len(data))], [MEAN for row in data], linestyle='dashed', color='black', label='lower bound')
    plt.plot([index for index in range(len(data))], [MEAN-COVARIANCE for row in data], linestyle='dotted', color='gray', label='upper bound')

    plt.title('Tenant Traffic :'+label)
    plt.xlabel('Timestamp')
    plt.ylabel('Traffic')
    plt.legend()
    plt.grid(True)

    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()

def line_chart_gap(index):
    DATA_PATH = "../Datei/Simple_V3_MVP/traffic.csv"
    IMAGE_PATH = "../Datei/Simple_V3_MVP/images/traffic_{}_linechart_gap.png"

    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()

    if(index==0):
        label = 'A'
        color = '#99FF99'
        real_color = 'g'
    elif(index==1):
        label ='B'
        color = '#9999FF'
        real_color = 'b'
    else:
        label = 'C'
        color = '#FF9999'
        real_color = 'r'
    
    original_traffic = [float(row[index*3]) for row in data]
    real_traffic = [float(row[index*3+1]) for row in data]
    transfer_rate = sum(real_traffic)/sum(original_traffic)*100

    plt.plot([index for index in range(len(data))], [float(row[index*3]) for row in data], linestyle='-', color=color, label=label+"(Transfer)")
    plt.plot([index for index in range(len(data))], [float(row[index*3+1]) for row in data], linestyle='-', color=real_color, label=label+"(Real Flow)")
    plt.plot([index for index in range(len(data))], [MEAN-COVARIANCE for row in data], linestyle='dotted', color='gray', label='lower bound')
    plt.plot([index for index in range(len(data))], [MEAN for row in data], linestyle='dashed', color='black', label='lower bound')
    plt.plot([index for index in range(len(data))], [MEAN-COVARIANCE for row in data], linestyle='dotted', color='gray', label='upper bound')

    plt.title('Tenant Traffic :'+label)
    plt.xlabel('Timestamp')
    plt.ylabel('Traffic')
    plt.legend()
    plt.grid(True)

    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()
    print("Trans. rate : ", transfer_rate)

def total_line_chart(capacity):
    DATA_PATH = "../Datei/Simple_V3_MVP/traffic.csv"
    IMAGE_PATH = "../Datei/Simple_V3_MVP/images/traffic_total_linechart.png"

    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()

    original_traffic = [float(row[0])+float(row[3])+float(row[6]) for row in data]
    real_traffic = [float(row[1])+float(row[4])+float(row[7]) for row in data]
    
    original_mean_traffic = sum(original_traffic)/len(data)
    real_mean_traffic = sum(real_traffic)/len(data)
    original_max_traffic = max(original_traffic)
    real_max_traffic = max(real_traffic)

    plt.plot([index for index in range(len(data))], original_traffic, linestyle='-', color='#9999FF', label="(Transfer)")
    plt.plot([index for index in range(len(data))], real_traffic, linestyle='-', color='b', label="(Real Flow)")
    plt.plot([index for index in range(len(data))], [capacity for row in data], linestyle='dotted', color='r', label='link capacity')


    plt.title('Tenant Traffics')
    plt.xlabel('Timestamp')
    plt.ylabel('Traffic')
    plt.legend()
    plt.grid(True)

    plt.savefig(IMAGE_PATH)
    plt.cla()
    print("Traffic    | {:10s} | {:10s}".format("Transfer", "Real Flow"))
    print("Mean       | {:10f} | {:10f}".format(original_mean_traffic, real_mean_traffic))
    print("Mean util. | {:10f} | {:10f}".format(original_mean_traffic/capacity, real_mean_traffic/capacity))
    print("Max        | {:10f} | {:10f}".format(original_max_traffic, real_max_traffic))
    print("Max util.  | {:10f} | {:10f}".format(original_max_traffic/capacity, real_max_traffic/capacity))

if(int(sys.argv[1])==1):
    index = int(sys.argv[2])
    line_chart_tenant(index)

if(int(sys.argv[1])==2):
    index = int(sys.argv[2])
    line_chart_gap(index)

if(int(sys.argv[1])==3):
    capacity = float(sys.argv[2])
    total_line_chart(capacity)