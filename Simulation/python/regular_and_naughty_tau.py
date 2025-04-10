import configparser
import matplotlib.pyplot as plt
import pandas
import sys

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

IMAGE_PATH_REGULAR = config['simulation']['data_path']+'/images/regular_and_naughty_tau_regular.png'
IMAGE_PATH_NAUGHTY = config['simulation']['data_path']+'/images/regular_and_naughty_tau_naughty.png'
DATA_PATH = config['simulation']['data_path']+'/regular_and_naughty_tau.csv'

dataframe = pandas.read_csv(DATA_PATH)

tau = dataframe['tau'].tolist()
tau = list(set(tau))
tau.sort()

regular_loss = dataframe['regular_loss'].tolist()
naughty_loss = dataframe['naughty_loss'].tolist()

if(int(sys.argv[1])==0):
    title = 'Packet Loss with different τ and r value : Naughty {}'.format(config['traffic']['naughty_mean'])
elif(int(sys.argv[1])==1):
    average = int(config['traffic']['naughty_mean'])*int(config['traffic']['naughty_tenant_number'])+int(config['traffic']['mean'])*(int(config['simulation']['tenant_number'])-int(config['traffic']['naughty_tenant_number']))
    average = average/int(config['simulation']['tenant_number'])
    title = 'Packet Loss with different τ and r value : Average {}'.format(average)
elif(int(sys.argv[1])==2):
    title = 'Packet Loss with different τ and r value : Naughty {} [{}]'.format(config['traffic']['naughty_mean'], sys.argv[2])
else:
    title = 'Packet Loss with different τ and r value : Naughty {}'.format(config['traffic']['naughty_mean'])

# regular
plt.plot(tau, regular_loss ,linestyle='-', label='(regular)', alpha = 1)
plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.title(title+' (Regular)')

plt.savefig(IMAGE_PATH_REGULAR)
plt.cla()

# naughty
plt.plot(tau, naughty_loss ,linestyle='-', label='(naughty)', alpha = 1)
plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
plt.ylabel('Loss (%)')
plt.legend()
plt.grid(True)

plt.title(title+' (Naughty)')

plt.savefig(IMAGE_PATH_NAUGHTY)
plt.cla()




