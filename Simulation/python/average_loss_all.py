import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

path = '/home/wtsaichu/Dokumente/Arbeitsplatz/Abschlusssarbeit/Simulation/data/burst_regular_different_r/main'
tail = '_burst_r_regular'

# IMAGE_PATH = config['simulation']['data_path']+'/images/record_average_loss_all.png'
# DATA_PATH = config['simulation']['data_path']+'/record_average_loss.csv'

IMAGE_PATH = path+'/images/record_average_loss_all'+tail+'.png'
DATA_PATH = path+'/record_average_loss.csv'

dataframe = pandas.read_csv(DATA_PATH)

tau = dataframe['tau'].tolist()
tau = list(set(tau))
tau.sort()

if(int(sys.argv[1])==2):
    kind_key = list(set(dataframe['state_r'].tolist()))
    for key in kind_key:
        loss = dataframe[dataframe['state_r'] == key][['average_loss']]
        if(key == 0.97875):
            plt.plot(tau, loss, linestyle='-', label='r = '+str(key), alpha = 0.5)
        else:
            plt.plot(tau, loss, linestyle='-', label='r = '+str(key), alpha = 1)

    # plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')
    plt.title('Average Packet Loss Rate with different τ and r value (All Compliant SLA)')
    
plt.xlabel('τ')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)
# plt.title('Average Packet Loss with different τ and r value (Naughty 150)')

plt.savefig(IMAGE_PATH)
plt.cla()
