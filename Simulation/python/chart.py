import sys
import pandas
import matplotlib.pyplot as plt
import configparser

CONFIG_PATH = sys.argv[1]
config = configparser.ConfigParser()
config.read(CONFIG_PATH)
error = float(config['simulation']['error'])
tau = int(config['GCRA']['tau_2'])
DATA_PATH = config['simulation']['data_path']+'/label.csv'
IMAGE_PATH = config['simulation']['data_path']+'/loss_pure_line_chart.png'

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()
# print(dataframe)
# print(len(dataframe))

x_axis = [index for index, value in enumerate(data)]
pure_loss = [float(tenant[4]) for tenant in data]
total_loss = [float(tenant[5]) for tenant in data]
  
plt.plot(x_axis, pure_loss, linestyle='-', color='blue', label='Loss(Pure)')
plt.plot(x_axis, [error*100 for loss in pure_loss], linestyle='dotted', color='red', label='Epsilon')

plt.title('Pure Loss of Each Tenant with τ({})'.format(tau))
plt.xlabel('Tenant')
plt.ylabel('Loss(Only for secod GCRA)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()

IMAGE_PATH = config['simulation']['data_path']+'/loss_total_line_chart.png'
plt.plot(x_axis, total_loss, linestyle='-', color='blue', label='Loss(Total)')

plt.title('Total Loss of Each Tenant with τ({})'.format(tau))
plt.xlabel('Tenant')
plt.ylabel('Loss(Total)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()

