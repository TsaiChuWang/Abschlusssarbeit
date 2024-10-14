import cvxpy
import numpy
import math
import sys
import configparser
import networkx as nx
import matplotlib.pyplot as plt
import pandas as pd
import sys
import os
term_size = os.get_terminal_size()

DATEI_OBJECTIVE_WEG = "../Datei/objective.txt"
DATEI_FRACTION_WEG = "../Datei/fraction.csv"

alpha = cvxpy.Variable(nonneg=True)
objective = cvxpy.Minimize(alpha)

def line_chart_tenant(index):
    DATA_PATH = "../Datei/simple_V5/traffic_{}.csv"
    IMAGE_PATH = "../Datei/simple_V5/images/traffic_{}_linechart.png"

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
    plt.plot([index for index in range(len(data))], [160 for row in data], linestyle='dotted', color='gray', label='lower bound')
    plt.plot([index for index in range(len(data))], [120 for row in data], linestyle='dashed', color='black', label='lower bound')
    plt.plot([index for index in range(len(data))], [80 for row in data], linestyle='dotted', color='gray', label='upper bound')

    plt.title('Tenant Traffic :'+label)
    plt.xlabel('Timestamp')
    plt.ylabel('Traffic')
    plt.legend()
    plt.grid(True)

    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()

def _line_chart_tenant(index, MEAN, COVARIANCE):
  DATA_PATH = "../Datei/simple_V5/traffic_{}.csv"
  IMAGE_PATH = "../Datei/simple_V5/images/traffic_{}_linechart.png"

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

def validation(error, mean, standard_deviation, tenant_number, capacity):
    number = math.sqrt(2*numpy.log((1/error)))
    print("Validation")
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sqrt(2*ln(1/error))", number, number))
    number = number*math.sqrt(sum([((standard_deviation/mean)*mean)*((standard_deviation/mean)*mean) for i in range(tenant_number)]))
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sqrt(sum((standard_deviation)^2))", math.sqrt(sum([((standard_deviation/mean)*mean)*((standard_deviation/mean)*mean) for i in range(tenant_number)])), number))
    number = number + mean*tenant_number
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sum(mean) = ", mean*tenant_number, number))
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sum all = ", number, number))
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("alpha = ", number/capacity, number/capacity))

def line_chart_gap(index):
    DATA_PATH = "../Datei/simple_V5/traffic.csv"
    IMAGE_PATH = "../Datei/simple_V5/images/traffic_{}_linechart_gap.png"

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
    plt.plot([index for index in range(len(data))], [160 for row in data], linestyle='dotted', color='gray', label='lower bound')
    plt.plot([index for index in range(len(data))], [120 for row in data], linestyle='dashed', color='black', label='lower bound')
    plt.plot([index for index in range(len(data))], [80 for row in data], linestyle='dotted', color='gray', label='upper bound')

    plt.title('Tenant Traffic :'+label)
    plt.xlabel('Timestamp')
    plt.ylabel('Traffic')
    plt.legend()
    plt.grid(True)

    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()
    print("Trans. rate : ", transfer_rate)

def _line_chart_gap(index, MEAN, COVARIANCE):
    DATA_PATH = "../Datei/simple_V5/traffic.csv"
    IMAGE_PATH = "../Datei/simple_V5/images/traffic_{}_linechart_gap.png"

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
    DATA_PATH = "../Datei/simple_V5/traffic.csv"

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
    DATA_PATH = "../Datei/simple_V5/traffic.csv"
    IMAGE_PATH = "../Datei/simple_V5/images/traffic_total_linechart.png"

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

def min_transfer_rate(number, config_path):
    transfer_rate = 0.0
    DATA_PATH = "../Datei/simple_V5/traffic.csv"
    config = configparser.ConfigParser()
    config.read(config_path)

    error = float(config['simulation']['error'])
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

def variation_line_chart(title, config_path):
    DATA_PATH = "../Datei/simple_V5/important_data.csv"
    IMAGE_PATH_MAX_TRANSFER_RATE ="../Datei/simple_V5/images/max_transfer_rate_variation.png"
    IMAGE_PATH_ALPHA ="../Datei/simple_V5/images/alpha.png"
    config = configparser.ConfigParser()
    config.read(config_path)

    error = float(config['simulation']['error'])
    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()

    max_transfer_rate = [float(row[1]) for row in data]
    alpha = [float(row[2]) for row in data]

    plt.plot([float(row[0]) for row in data], max_transfer_rate, linestyle='-', color='red', label="Min transfer rate")
    plt.plot([float(row[0]) for row in data], [100-(error*100) for row in data], linestyle='-', color='blue', label="Predicted Transfer")
    plt.title('Min Transfer Rate : '+title+' Variation')
    plt.xlabel(title)
    plt.ylabel('Min Transfer Rate')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_MAX_TRANSFER_RATE)
    plt.clf()

    plt.plot([float(row[0]) for row in data], alpha, linestyle='-', color='red', label="Capacity(Alpha)")
    plt.title('Capacity(Alpha) : '+title+' Variation')
    plt.xlabel(title)
    plt.ylabel('Capacity(Alpha)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_ALPHA)
    plt.clf()

def variation_line_chart_tenant_number_variation(title, config_path):
    config = configparser.ConfigParser()
    config.read(config_path)
    # print(config.keys)
    error = float(config['simulation']['error'])
    standard_deviation = float(config['traffic']['standard_deviation'])
    mean = float(config['traffic']['mean'])

    DATA_PATH = "../Datei/simple_V5/important_data.csv"
    IMAGE_PATH_MAX_TRANSFER_RATE ="../Datei/simple_V5/images/max_transfer_rate_variation.png"
    IMAGE_PATH_ALPHA ="../Datei/simple_V5/images/alpha.png"

    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()

    max_transfer_rate = [float(row[1]) for row in data]
    alpha = [float(row[2]) for row in data]

    plt.plot([float(row[0]) for row in data], max_transfer_rate, linestyle='-', color='red', label="Min transfer rate")
    plt.plot([float(row[0]) for row in data], [100-(error*100) for row in data], linestyle='-', color='blue', label="Predicted Transfer")
    plt.title('Min Transfer Rate : '+title+' Variation')
    plt.xlabel(title)
    plt.ylabel('Min Transfer Rate')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_MAX_TRANSFER_RATE)
    plt.clf()

    plt.plot([float(row[0]) for row in data], alpha, linestyle='-', color='red', label="Capacity(Alpha)")
    plt.plot([float(row[0]) for row in data], [(standard_deviation+mean)*float(row[0]) for row in data], linestyle='-', color='blue', label="Capacity(Linear)")
    plt.title('Capacity(Alpha) : '+title+' Variation')
    plt.xlabel(title)
    plt.ylabel('Capacity(Alpha)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_ALPHA)
    plt.clf()

def allocated_tenant_number_variation(title):
    DATA_PATH = "../Datei/simple_V5/important_data.csv"
    IMAGE_PATH_ALPHA_ALLOCATED ="../Datei/simple_V5/images/alpha_allocated.png"
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
    DATA_PATH = "../Datei/simple_V5/important_data.csv"
    IMAGE_PATH_ALPHA_ALLOCATED ="../Datei/simple_V5/images/alpha_allocated.png"
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

def record_An_experiment_MTR():
    DATA_PATH = "../Datei/simple_V5/important_data.csv"
    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()
    data_index_row = [float(row[0]) for row in data]
    mtr = [float(row[1]) for row in data]
    # print(data_index_row)
    # print(mtr)
    RECORD_DATA_PATH = "../Datei/simple_V5/experiment.csv"
    try:
        record_dataframe = pd.read_csv(RECORD_DATA_PATH)
        record_data = record_dataframe.values.tolist()
        record_data[0] = data_index_row
    except:
        record_data = [data_index_row]
    # print(record_data)
    record_data.append(mtr)
    record_data = numpy.array(record_data)
    record_dataframe = pd.DataFrame(record_data)
    # print(record_data)
    record_dataframe.to_csv(RECORD_DATA_PATH, index=False)

def draw_An_experiment_MTR():
    RECORD_DATA_PATH = "../Datei/simple_V5/experiment.csv"
    record_dataframe = pd.read_csv(RECORD_DATA_PATH)
    record_data = record_dataframe.values.tolist()

    config = configparser.ConfigParser()
    config.read("../configuration/simple_V5.ini")
    # print(config.keys)
    error = float(config['simulation']['error'])

    num = len(record_data)-1
    cmap = plt.get_cmap('tab20')
    
    colors = [cmap(i / num) for i in range(num)]
    hex_colors = ['#%02x%02x%02x' % (int(r*255), int(g*255), int(b*255)) for r, g, b, _ in colors]
    # print(hex_colors)

    IMAGE_PATH_EXPERIMENT_MTR ="../Datei/simple_V5/images/experiment_mtr.png"
    for index in range(num):
        plt.plot(record_data[0], record_data[index+1], linestyle='-', color=hex_colors[index], label=str(index+1))
    plt.plot([float(row[0]) for row in record_data], [100-(error*100) for row in record_data], linestyle='-', color='blue', label="Predicted Transfer")
    plt.title('MTR Experiment with '+str(num)+' Times')
    plt.xlabel('Times')
    plt.ylabel('Min Transfer Rate')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH_EXPERIMENT_MTR)
    plt.clf()

if(int(sys.argv[1]) == 0):
    config = configparser.ConfigParser()
    config.read(sys.argv[2])
    error = float(config['simulation']['error'])
    standard_deviation = float(config['traffic']['standard_deviation'])
    mean = float(config['traffic']['mean'])
    tenant_number = int(config['simulation']['tenant_number'])

    constraints = [ math.sqrt(2*numpy.log((1/error)))*numpy.linalg.norm([(standard_deviation/mean)*mean for i in range(tenant_number)], ord = 2) + (mean*tenant_number) <=alpha ]
    
    problem = cvxpy.Problem(objective, constraints)
    problem.solve(solver=cvxpy.CVXOPT)
    print("optimal value with CVXOPT: {:.12f}".format(problem.value))
    with open(DATEI_OBJECTIVE_WEG, 'w') as file:
        file.write(str(alpha.value))
    file.close()

if(int(sys.argv[1])==1):
    index = int(sys.argv[2])
    line_chart_tenant(index)

if(int(sys.argv[1])==2):
    index = int(sys.argv[2])
    mean = float(sys.argv[3])
    covariance = float(sys.argv[4])
    _line_chart_tenant(index, mean, covariance)

if(int(sys.argv[1])==3):
    print('=' * term_size.columns)

if(int(sys.argv[1])==4):
    index = int(sys.argv[2])
    line_chart_gap(index)

if(int(sys.argv[1])==5):
    index = int(sys.argv[2])
    mean = float(sys.argv[3])
    covariance = float(sys.argv[4])
    _line_chart_gap(index, mean, covariance)

if(int(sys.argv[1])==6):
    IMAGE_PATH = "../Datei/simple_V5/images/traffic_total_linechart.png"
    capacity = float(sys.argv[2])
    total_line_chart(capacity)

if(int(sys.argv[1])==7):
    IMAGE_PATH = "../Datei/simple_V5/images/traffic_total_linechart.png"
    capacity = float(sys.argv[2])
    number = int(sys.argv[3])
    _total_line_chart(capacity, number, IMAGE_PATH)

if(int(sys.argv[1])==8):
    number = int(sys.argv[2])
    min_transfer_rate(number, sys.argv[3])

if(int(sys.argv[1])==9):
    # print(sys.argv)
    variation_line_chart(sys.argv[2], sys.argv[3])

if(int(sys.argv[1])==10):
    variation_line_chart_tenant_number_variation(sys.argv[2], sys.argv[3])

if(int(sys.argv[1])==11):
    allocated_tenant_number_variation(sys.argv[2])

if(int(sys.argv[1])==12):
    allocated_variation(sys.argv[2], int(sys.argv[3]))

if(int(sys.argv[1])==13):
    num = int(sys.argv[2])
    mean = float(sys.argv[3])
    covariance = float(sys.argv[4])
    for index in range(num):
        _line_chart_gap(index, mean, covariance)

if(int(sys.argv[1])==14):
    record_An_experiment_MTR()

if(int(sys.argv[1])==15):
    RECORD_DATA_PATH = "../Datei/simple_V5/experiment.csv"
    with open(RECORD_DATA_PATH, "w") as file:
      file.write("")
    file.close()

if(int(sys.argv[1])==16):
    draw_An_experiment_MTR()

if(int(sys.argv[1])==17):
    DATA_PATH = "../Datei/simple_V5/traffic.csv"
    RECORD_PATH = "../Datei/simple_V5/experiments/traffic_{}.csv"

    config = configparser.ConfigParser()
    config.read("../configuration/simple_V5.ini")
    tenant_number = int(config['simulation']['tenant_number'])

    dataframe = pd.read_csv(DATA_PATH)
    data = dataframe.values.tolist()
    record_data = []
    for row in data:
        transfer_traffic = sum([float(row[index*3]) for index in range(tenant_number)])
        real_traffic = sum([float(row[index*3+1]) for index in range(tenant_number)])
        record_data.append([transfer_traffic, real_traffic])
    
    record_data = numpy.array(record_data)
    record_dataframe = pd.DataFrame(record_data)
    record_dataframe.to_csv(RECORD_PATH.format(sys.argv[2]), index=False)