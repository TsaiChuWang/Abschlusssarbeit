import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

different_r = [0.6,0.7,0.8,0.9]
different_r_data = []

for i in range(0, 42560, 64):
    for j in different_r:
        for k in different_r:
            if(j==k):
                continue
            else:
                different_r_data.append(j)

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

# IMAGE_PATH = config['simulation']['data_path']+'/images/different_r_{}.png'
# DATA_PATH = config['simulation']['data_path']+'/regular_and_naughty_all.csv'

IMAGE_PATH = '/home/wtsaichu/Dokumente/Arbeitsplatz/Abschlusssarbeit/Simulation/data/different_r/main/images/different_r_{}.png'
DATA_PATH = '/home/wtsaichu/Dokumente/Arbeitsplatz/Abschlusssarbeit/Simulation/data/different_r/main/regular_and_naughty_all.csv'

dataframe = pandas.read_csv(DATA_PATH)
dataframe['diff'] = different_r_data


tau_ = dataframe['tau'].tolist()
tau_ = list(set(tau_))
tau_.sort()

keys = []
for j in different_r:
    for k in different_r:
        if(j==k):
            continue
        elif(k<j):
            keys.append([k, j])



for key in keys:
    nloss = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['diff'] == key[1])]['naughty_loss'].tolist()
    plt.plot(tau_, nloss, linestyle='-', label='r(c) = {}, r(n) = {}'.format(key[0], key[1]), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss Rate with different τ and r Combination (Non-compliant Flows)')
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig(IMAGE_PATH.format('naughty'))
plt.cla()  

for key in keys:
    rloss = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['diff'] == key[1])]['regular_loss'].tolist()
    plt.plot(tau_, rloss, linestyle='-', label='r(c) = {}, r(n) = {}'.format(key[0], key[1]), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss Rate with different τ and r Combination (Compliant Flows)')
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig(IMAGE_PATH.format('regular'))
plt.cla()  
# if(int(sys.argv[1]) == R):

#     tau_ = dataframe['tau'].tolist()
#     tau_ = list(set(tau_))
#     tau_.sort()

#     keys = list(set(dataframe['state_r'].tolist()))
#     keys.sort()

#     for key in keys:
#         nloss = dataframe[dataframe['state_r'] == key][['naughty_loss']]
#         if(len(nloss)!=0):
#             tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
#             plt.plot(tau, nloss, linestyle='-', label='(naughty)r = '+str(key), alpha = 1)
#     plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
#     plt.title('Packet Loss with different τ and r Naughty Flow')
#     plt.ylabel('Loss (%)')
#     plt.xlabel('τ')
#     plt.legend()
#     plt.grid(True)
#     plt.savefig(IMAGE_PATH.format('naughty'))
#     plt.cla()

#     for key in keys:
#         rloss = dataframe[dataframe['state_r'] == key][['regular_loss']]
#         if(len(rloss)!=0):
#             tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
#             tau = list(set(tau))
#             tau.sort()
#             plt.plot(tau, rloss, linestyle='-', label='(regular)r = '+str(key), alpha = 1)
#     plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
#     plt.title('Packet Loss with different τ and r Regular Flow')
#     plt.xlabel('τ')
#     plt.ylabel('Loss (%)')
#     plt.legend()
#     plt.grid(True)
#     plt.savefig(IMAGE_PATH.format('regular'))
#     plt.cla()
