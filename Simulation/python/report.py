import matplotlib.pyplot as plt
import pandas
import numpy as np
IMAGE_PATH = '../data/report.png'
DATA_PATH = '../data/naughty.csv'

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()

tau = []
tau_130 = []
naughty_loss_130 = []
regular_loss_130 = []

tau_140 = []
naughty_loss_140 = []
regular_loss_140 = []

tau_150 = []
naughty_loss_150 = []
regular_loss_150 = []
for row in data:
  tau.append(int(row[0]))
  if(int(row[1]) == 130):
    tau_130.append(int(row[0]))
    regular_loss_130.append(float(row[2]))
    naughty_loss_130.append(float(row[3]))
  if(int(row[1]) == 140):
    tau_140.append(int(row[0]))
    regular_loss_140.append(float(row[2]))
    naughty_loss_140.append(float(row[3]))
  if(int(row[1]) == 150):
    tau_150.append(int(row[0]))
    regular_loss_150.append(float(row[2]))
    naughty_loss_150.append(float(row[3]))

tau = set(tau)
tau = list(tau)

# plt.plot(tau_130, regular_loss_130, linestyle='-', label='Regular(130)')
# plt.plot(tau_130, naughty_loss_130, linestyle='-', label='Naughty(130)')

# plt.plot(tau_140, regular_loss_140, linestyle='-', label='Regular(140)')
# plt.plot(tau_140, naughty_loss_140, linestyle='-', label='Naughty(140)')

plt.plot(tau_150, regular_loss_150, linestyle='-', label='Regular')
plt.plot(tau_150, naughty_loss_150, linestyle='-', label='Naughty(150)')

plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='epsilon')

plt.title('Regular Loss with different naughty mean and tau')
# plt.title('Naughty Loss with different naughty mean and tau')
# plt.title('Regular Loss with different tau (Naughty 130)')
# plt.title('Regular Loss with different tau (Naughty 140)')
# plt.title('Regular Loss with different tau (Naughty 150)')
plt.xlabel('tau')
plt.ylabel('Loss')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()

