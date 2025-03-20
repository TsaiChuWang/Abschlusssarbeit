import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

IMAGE_PATH = '../data/report.png'
DATA_PATH = '../data/naughty.csv'
# DATA_PATH = '../data/naughty_1.csv'

# dataframe = pandas.read_csv(DATA_PATH, header=None)
# data = dataframe.values.tolist()

dataframe = pandas.read_csv(DATA_PATH)

tau = dataframe["tau"].tolist()
regular_loss = dataframe["regular_loss"].tolist()
naughty_loss = dataframe["naughty_loss"].tolist()

m = int(sys.argv[1])
n = int(sys.argv[2])
tau = tau[m:n]
regular_loss = regular_loss[m:n]
naughty_loss = naughty_loss[m:n]

plt.plot(tau, regular_loss, linestyle='-', label='regular_loss')
plt.plot(tau, naughty_loss, linestyle='-', label='naughty_loss')
plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')

plt.title('Packet Loss with different τ')
plt.xlabel('τ')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()