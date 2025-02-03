import matplotlib.pyplot as plt
import pandas
import numpy as np
IMAGE_PATH = '../data/report.png'
DATA_PATH = '../data/naughty.csv'

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()

tau = []
naughty_loss = [[],[],[]]
regular_loss = [[],[],[]]
for row in data:
  tau.append(int(row[0]))
  if(int(row[1]) == 130):
    regular_loss[0].append(float(row[2]))
    naughty_loss[0].append(float(row[3]))
  if(int(row[1]) == 140):
    regular_loss[1].append(float(row[2]))
    naughty_loss[1].append(float(row[3]))
  if(int(row[1]) == 150):
    regular_loss[2].append(float(row[2]))
    naughty_loss[2].append(float(row[3]))

tau = set(tau)
tau = list(tau)
print(regular_loss[2])

# plt.plot(tau, np.array(naughty_loss[0]), linestyle='-', label='Naughty Mean(130)')
# plt.plot(tau, np.array(naughty_loss[1]), linestyle='-', label='Naughty Mean(140)')
# plt.plot(tau, np.array(naughty_loss[2]), linestyle='-', label='Naughty Mean(150)')

# plt.plot(tau, np.array(regular_loss[0]), linestyle='-', label='Regular(130)')
# plt.plot(tau, np.array(naughty_loss[0]), linestyle='-', label='Naughty(130)')

# plt.plot(tau, np.array(regular_loss[1]), linestyle='-', label='Regular(140)')
# plt.plot(tau, np.array(naughty_loss[1]), linestyle='-', label='Naughty(140)')

plt.plot(tau, np.array(regular_loss[2]), linestyle='-', label='Regular(150)')
plt.plot(tau, np.array(naughty_loss[2]), linestyle='-', label='Naughty(150)')

plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='epsilon')

# plt.title('Naughty Loss with different naughty mean and tau')
plt.title('Loss with different tau')
plt.xlabel('tau')
plt.ylabel('Loss')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()

