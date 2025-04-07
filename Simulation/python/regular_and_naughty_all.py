import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

IMAGE_PATH = config['simulation']['data_path']+'/regular_and_naughty_all.png'
DATA_PATH = config['simulation']['data_path']+'/regular_and_naughty_all.csv'

dataframe = pandas.read_csv(DATA_PATH)

tau = dataframe['tau'].tolist()
tau = list(set(tau))
tau.sort()

kind_key = list(set(dataframe['state_r'].tolist()))

n = 1600
for key in kind_key:
    nloss = dataframe[dataframe['state_r'] == key][['naughty_loss']]
    rloss = dataframe[dataframe['state_r'] == key][['regular_loss']]

    nloss = nloss/100
    rloss = rloss/100
    plt.plot(tau[:n],nloss[:n], linestyle='-', label='(naghty)r = '+str(key), alpha = 1)
    # plt.plot(tau[:n],rloss[:n], linestyle='-', label='(regular)r = '+str(key), alpha = 1)

plt.plot(tau[:n], [0.1 for i in tau][:n], linestyle='-', color = 'red', label='ε')

# plt.title('Average Packet Loss with different τ and r value (All Regular)')
# plt.title('Packet Loss with different τ and r value (Naughty 150) Regular Flow')
plt.title('Packet Loss with different τ and r value (Naughty 150) Naughty Flow')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()
