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