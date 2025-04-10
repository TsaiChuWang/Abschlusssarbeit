import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

IMAGE_PATH = '../data/unifrom_naughty_naughty_mode/'
DATA_PATH = '../data/unifrom_naughty_naughty_mode/{}/main/regular_and_naughty_tau.csv'


for i in range(3):
    dataframe = pandas.read_csv(DATA_PATH.format(i))
    tau = dataframe['tau'].tolist()
    tau = list(set(tau))
    tau.sort()

    nloss = dataframe['naughty_loss']
    rloss = dataframe['regular_loss']

    plt.plot(tau, nloss, linestyle='-', label='naughty mode = {}'.format(i))

plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')

plt.title('Packet Loss with different τ and Naughty Mode Naughty Flow')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH+'naughty.png')
plt.cla()

for i in range(3):
    dataframe = pandas.read_csv(DATA_PATH.format(i))
    tau = dataframe['tau'].tolist()
    tau = list(set(tau))
    tau.sort()

    nloss = dataframe['naughty_loss']
    rloss = dataframe['regular_loss']

    plt.plot(tau, rloss, linestyle='-', label='naughty mode = {}'.format(i))

plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')

plt.title('Packet Loss with different τ and Naughty Mode Regular Flow')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH+'regular.png')
plt.cla()