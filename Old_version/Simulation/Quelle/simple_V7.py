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

DATA_PATH = "../Datei/simple_V7/data.csv"
dataframe = pd.read_csv(DATA_PATH)
data = dataframe.values.tolist()[:-1]

#calculate keys
keys = [row[1] for row in data]
keys = set(keys)
for key in keys:
  
  time_data = [int(float(row[0])/100000) for row in data]
  time_end = int(time_data[-1])+1

  x_axis = [i for i in range(time_end)]
  y_axis = []
  for i in x_axis:
    if i in time_data:
      y_axis.append(1)
    else:
      y_axis.append(0)
  # y_axis = [0 for i in range(time_end)]
  plt.plot(x_axis, y_axis, linestyle='-', color='red', label=key)
  plt.title('Tenant Traffic :'+key+' (Time)')
  plt.xlabel('Timestamp')
  plt.ylabel('Package')
  plt.legend()
  plt.grid(True)

  plt.savefig("../Datei/simple_V7/images/"+key+"_time.png")
  plt.cla()

  
  
  speed = []

