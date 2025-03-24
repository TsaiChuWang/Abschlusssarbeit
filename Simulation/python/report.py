import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

IMAGE_PATH = '../data/report.png'
DATA_PATH = '../data/main/record_average_loss.csv'
# DATA_PATH = '../data/naughty_1.csv'

# dataframe = pandas.read_csv(DATA_PATH, header=None)
# data = dataframe.values.tolist()

dataframe = pandas.read_csv(DATA_PATH)

tau_original = dataframe["tau"].tolist()
average_loss_original = dataframe["average_loss"].tolist()
# state_r = dataframe["state_r"].tolist()


# m = float(sys.argv[1])


# tau = []
# average_loss = []
# for index, value in enumerate(state_r):
#   if(value == m):
#     tau.append(tau_original[index])
#     average_loss.append(average_loss_original[index])

plt.plot(tau_original , average_loss_original, linestyle='-', label='average_loss')

# plt.plot(tau, average_loss, linestyle='-', label='average_loss')
# plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')

plt.title('Packet Loss with different τ (Naughty 140)')
plt.xlabel('τ')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()