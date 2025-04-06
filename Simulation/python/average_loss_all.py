import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

IMAGE_PATH = config['simulation']['data_path']+'/record_average_loss.png'
DATA_PATH = config['simulation']['data_path']+'/record_average_loss.csv'

dataframe = pandas.read_csv(DATA_PATH)

tau = dataframe['tau'].tolist()
tau = list(set(tau))
tau.sort()

kind_key = list(set(dataframe['state_r'].tolist()))
datas = []
n = 800
for key in kind_key:
    datas.append(dataframe[dataframe['state_r'] == key][['average_loss']])
    if(key == 0.9775):
        plt.plot(tau[:n],dataframe[dataframe['state_r'] == key][['average_loss']][:n], linestyle='-', label='r = '+str(key), alpha = 0.5)
    else:
        plt.plot(tau[:n],dataframe[dataframe['state_r'] == key][['average_loss']][:n], linestyle='-', label='r = '+str(key), alpha = 1)

plt.plot(tau[:n], [0.1 for i in tau][:n], linestyle='-', color = 'red', label='ε')

plt.title('Average Packet Loss with different τ and r value (All Regular)')

plt.xlabel('τ')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()
