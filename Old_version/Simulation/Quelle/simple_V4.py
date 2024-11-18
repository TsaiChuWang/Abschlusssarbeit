# importing networkx 
import networkx as nx
# importing matplotlib.pyplot
import matplotlib.pyplot as plt
import pandas as pd
import sys
import os
term_size = os.get_terminal_size()

MEAN = 120
COVARIANCE = 40

def line_chart_tenant(index):
    DATA_PATH = "../Datei/simple_V4/traffic_{}.csv"
    IMAGE_PATH = "../Datei/simple_V4/images/traffic_{}_linechart.png"

    dataframe = pd.read_csv(DATA_PATH.format(index))
    data = dataframe.values.tolist()

    if(index==0):
        label = 'A'
        color = 'g'
    elif(index==1):
        label ='B'
        color = 'b'
    elif(index==2):
        label = 'C'
        color = 'r'
    else:
        label = str(index)
        color = 'black'       

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

def _line_chart_tenant(index, MEAN, COVARIANCE):
  DATA_PATH = "../Datei/simple_V4/traffic_{}.csv"
  IMAGE_PATH = "../Datei/simple_V4/images/traffic_{}_linechart.png"

  dataframe = pd.read_csv(DATA_PATH.format(index))
  data = dataframe.values.tolist()

  if(index==0):
      label = 'A'
      color = 'g'
  elif(index==1):
      label ='B'
      color = 'b'
  elif(index==2):
      label = 'C'
      color = 'r'
  else:
      label = str(index)
      color = 'black'       

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
    DATA_PATH = "../Datei/simple_V4/traffic.csv"
    IMAGE_PATH = "../Datei/simple_V4/images/traffic_{}_linechart_gap.png"

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

def _line_chart_gap(index, MEAN, COVARIANCE):
    DATA_PATH = "../Datei/simple_V4/traffic.csv"
    IMAGE_PATH = "../Datei/simple_V4/images/traffic_{}_linechart_gap.png"

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
    elif(index==2):
        label = 'C'
        color = '#FF9999'
        real_color = 'r'
    else:
        label = str(index)
        color = 'gray'
        real_color = 'black'

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

def total_line_chart(capacity, IMAGE_PATH):
    DATA_PATH = "../Datei/simple_V4/traffic.csv"

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
    print('=' * term_size.columns)
    print("Traffic    | {:10s} | {:10s}".format("Transfer", "Real Flow"))
    print("Mean       | {:10f} | {:10f}".format(original_mean_traffic, real_mean_traffic))
    print("Mean util. | {:10f} | {:10f}".format(original_mean_traffic/capacity, real_mean_traffic/capacity))
    print("Max        | {:10f} | {:10f}".format(original_max_traffic, real_max_traffic))
    print("Max util.  | {:10f} | {:10f}".format(original_max_traffic/capacity, real_max_traffic/capacity))
    print('=' * term_size.columns)

def _total_line_chart(capacity, number, IMAGE_PATH):
    DATA_PATH = "../Datei/simple_V4/traffic.csv"
    IMAGE_PATH = "../Datei/simple_V4/images/traffic_total_linechart.png"

    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()
    original_traffic = [sum([float(row[index*3]) for index in range(number)]) for row in data]
    real_traffic = [sum([float(row[index*3+1]) for index in range(number)]) for row in data]
    
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
    print('=' * term_size.columns)
    print("Traffic    | {:10s} | {:10s}".format("Transfer", "Real Flow"))
    print("Mean       | {:10f} | {:10f}".format(original_mean_traffic, real_mean_traffic))
    print("Mean util. | {:10f} | {:10f}".format(original_mean_traffic/capacity, real_mean_traffic/capacity))
    print("Max        | {:10f} | {:10f}".format(original_max_traffic, real_max_traffic))
    print("Max util.  | {:10f} | {:10f}".format(original_max_traffic/capacity, real_max_traffic/capacity))
    print('=' * term_size.columns)

def min_transfer_rate(number):
    transfer_rate = 0.0
    DATA_PATH = "../Datei/simple_V4/traffic.csv"

    rate = []
    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()
    for index in range(number):
      original_traffic = [float(row[index*3]) for row in data]
      real_traffic = [float(row[index*3+1]) for row in data]
      rate.append(sum(real_traffic)/sum(original_traffic)*100)
    transfer_rate = min(rate)
    print("a="+str(transfer_rate))
    with open("../Datei/transrate.txt", "w") as file:
	    file.write(str(transfer_rate))
    file.close()
    return transfer_rate
    

def variation_line_chart(title):
    DATA_PATH = "../Datei/simple_V4/important_data.csv"
    IMAGE_PATH_MAX_TRANSFER_RATE ="../Datei/simple_V4/images/max_transfer_rate_variation.png"
    IMAGE_PATH_ALPHA_1 ="../Datei/simple_V4/images/alpha1.png"
    IMAGE_PATH_ALPHA_2 ="../Datei/simple_V4/images/alpha2.png"

    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()

    max_transfer_rate = [float(row[1]) for row in data]
    alpha_1 = [float(row[2]) for row in data]
    alpha_2 = [float(row[3]) for row in data]

    plt.plot([float(row[0]) for row in data], max_transfer_rate, linestyle='-', color='red', label="Min transfer rate")
    plt.title('Min Transfer Rate : '+title+' Variation')
    plt.xlabel(title)
    plt.ylabel('Min Transfer Rate')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_MAX_TRANSFER_RATE)
    plt.clf()

    plt.plot([float(row[0]) for row in data], alpha_1, linestyle='-', color='red', label="Alpha (1)")
    plt.title('Alpha(1) : '+title+' Variation')
    plt.xlabel(title)
    plt.ylabel('Alpha(1)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_ALPHA_1)
    plt.clf()

    plt.plot([float(row[0]) for row in data], alpha_2, linestyle='-', color='blue', label="Alpha (2)")
    plt.title('Alpha(2) : '+title+' Variation')
    plt.xlabel(title)
    plt.ylabel('Alpha(2)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_ALPHA_2)
    plt.clf()

def allocated_tenant_number_variation(title):
    DATA_PATH = "../Datei/simple_V4/important_data.csv"
    IMAGE_PATH_ALPHA_ALLOCATED ="../Datei/simple_V4/images/alpha_allocated.png"
    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()

    alpha_allocated = [(float(row[2])/int(row[0])) for row in data]
    plt.plot([float(row[0]) for row in data], alpha_allocated, linestyle='-', color='red', label="Alpha Each Tenant Allocated")
    plt.title('Alpha Each Tenant Allocated')
    plt.xlabel(title)
    plt.ylabel('Alpha Each Tenant Allocated')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_ALPHA_ALLOCATED)
    plt.clf()

def allocated_variation(title, number):
    DATA_PATH = "../Datei/simple_V4/important_data.csv"
    IMAGE_PATH_ALPHA_ALLOCATED ="../Datei/simple_V4/images/alpha_allocated.png"
    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()

    alpha_allocated = [(float(row[2])/number) for row in data]
    plt.plot([float(row[0]) for row in data], alpha_allocated, linestyle='-', color='red', label="Alpha Each Tenant Allocated")
    plt.title('Alpha Each Tenant Allocated')
    plt.xlabel(title)
    plt.ylabel('Alpha Each Tenant Allocated')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_ALPHA_ALLOCATED)
    plt.clf()

if(int(sys.argv[1])==0):
    print('=' * term_size.columns)

if(int(sys.argv[1])==1):
    index = int(sys.argv[2])
    line_chart_tenant(index)

if(int(sys.argv[1])==4):
    index = int(sys.argv[2])
    mean = float(sys.argv[3])
    covariance = float(sys.argv[4])
    _line_chart_tenant(index, mean, covariance)

if(int(sys.argv[1])==2):
    index = int(sys.argv[2])
    line_chart_gap(index)

if(int(sys.argv[1])==5):
    index = int(sys.argv[2])
    mean = float(sys.argv[3])
    covariance = float(sys.argv[4])
    _line_chart_gap(index, mean, covariance)

if(int(sys.argv[1])==8):
    number = int(sys.argv[2])
    min_transfer_rate(number)

if(int(sys.argv[1])==3):
    IMAGE_PATH = "../Datei/simple_V4/images/traffic_total_linechart.png"
    capacity = float(sys.argv[2])
    total_line_chart(capacity)

if(int(sys.argv[1])==6):
    IMAGE_PATH = "../Datei/simple_V4/images/traffic_total_linechart.png"
    capacity = float(sys.argv[2])
    number = int(sys.argv[3])
    _total_line_chart(capacity, number, IMAGE_PATH)

if(int(sys.argv[1])==7):
    IMAGE_PATH = "../Datei/simple_V4/images/traffic_total_linechart_2.png"
    capacity = float(sys.argv[2])
    number = int(sys.argv[3])
    _total_line_chart(capacity, number, IMAGE_PATH)

if(int(sys.argv[1])==9):
    variation_line_chart(sys.argv[2])

if(int(sys.argv[1])==10):
    allocated_tenant_number_variation(sys.argv[2])

if(int(sys.argv[1])==11):
    allocated_variation(sys.argv[2], int(sys.argv[3]))