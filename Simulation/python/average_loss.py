import configparser
import matplotlib.pyplot as plt
import pandas
import sys

CONFIGURATION_PATH = sys.argv[2]
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

IMAGE_PATH = config['simulation']['data_path']+'/images/average_loss_{}_{}.png'
DATA_PATH = config['simulation']['data_path']+'/record_average_loss.csv'

dataframe = pandas.read_csv(DATA_PATH)

name = sys.argv[1]

tau = dataframe['tau'].tolist()
average_loss_pure = dataframe['average_loss_pure'].tolist()
average_loss_all = dataframe['average_loss_all'].tolist()

plt.plot(tau, average_loss_pure, linestyle='-', label='average_loss(pure)')
plt.title('Average Packet Loss with different τ ({})'.format(name))

# plt.plot(tau, [float(config['simulation']['error'])*100 for i in tau], linestyle='-', color = 'red', label='ε')

plt.xlabel('GCRA Bucket Depth τ(bits)')
plt.ylabel('Packet Loss Rate(%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH.format(name, 'pure'))
plt.cla()

plt.plot(tau, average_loss_all, linestyle='-', label='average loss')
# plt.title('Average Packet Loss with different τ ({})'.format(name))

plt.plot(tau, [float(config['simulation']['error'])*100 for i in tau], linestyle='-', color = 'red', label='ε')

plt.xlabel('GCRA Bucket Depth τ(bits)')
plt.ylabel('Packet Loss Rate(%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH.format(name, 'all'))
plt.cla()