import configparser
import matplotlib.pyplot as plt
import pandas
import sys

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

IMAGE_PATH = config['simulation']['data_path']+'/images/record_average_loss.png'
DATA_PATH = config['simulation']['data_path']+'/record_average_loss.csv'

dataframe = pandas.read_csv(DATA_PATH)

tau = dataframe['tau'].tolist()
average_loss = dataframe['average_loss'].tolist()

plt.plot(tau, average_loss, linestyle='-', label='average_loss')
# plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')

if(int(sys.argv[1])==0):
    plt.title('Average Packet Loss with different τ (All Regular)')
else:
    plt.title('Average Packet Loss with different τ (All Regular)')

plt.xlabel('τ')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()
