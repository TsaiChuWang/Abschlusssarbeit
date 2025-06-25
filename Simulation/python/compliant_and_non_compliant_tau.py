import configparser
import matplotlib.pyplot as plt
import pandas
import sys

CONFIGURATION_PATH = sys.argv[2]
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

IMAGE_PATH_COMPLIANT = config['simulation']['data_path']+'/images/compliant_and_noncompliant_tau_{}_{}_compliant.png'
IMAGE_PATH_NONCOMPLIANT = config['simulation']['data_path']+'/images/compliant_and_noncompliant_tau_{}_{}_noncompliant.png'
DATA_PATH = config['simulation']['data_path']+'/compliant_and_noncompliant_tau.csv'

dataframe = pandas.read_csv(DATA_PATH)

tau = dataframe['tau'].tolist()
tau = list(set(tau))
tau.sort()

name = sys.argv[1]

compliant_loss_pure = dataframe['compliant_loss_pure'].tolist()
noncompliant_loss_pure = dataframe['noncompliant_loss_pure'].tolist()
compliant_loss_all = dataframe['compliant_loss_all'].tolist()
noncompliant_loss_all = dataframe['noncompliant_loss_all'].tolist()

title = 'Packet Loss with different τ ({}) : '.format(name)

# Pure
# compliant
plt.plot(tau, compliant_loss_pure ,linestyle='-', label='pure', alpha = 1)
plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
plt.xlabel('GCRA Bucket Depth τ(bits)')
plt.ylabel('Packet Loss Rate(%)')
plt.legend()
plt.grid(True)
# plt.title(title+'Compliant')
plt.savefig(IMAGE_PATH_COMPLIANT.format(name, 'pure'))
plt.cla()

# noncompliant
plt.plot(tau, noncompliant_loss_pure ,linestyle='-', label='pure', alpha = 1)
plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
plt.xlabel('GCRA Bucket Depth τ(bits)')
plt.ylabel('Packet Loss Rate(%)')
plt.legend()
plt.grid(True)
# plt.title(title+'Non-compliant')
plt.savefig(IMAGE_PATH_NONCOMPLIANT.format(name, 'pure'))
plt.cla()

# All
# compliant
plt.plot(tau, compliant_loss_all ,linestyle='-', label='all', alpha = 1)
plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
plt.xlabel('GCRA Bucket Depth τ(bits)')
plt.ylabel('Packet Loss Rate(%)')
plt.legend()
plt.grid(True)
plt.title('Uniform Distribution : Half Compliant SLA(120)')
# plt.title('Compliant SLA')
plt.savefig(IMAGE_PATH_COMPLIANT.format(name, 'all'))
plt.cla()

# noncompliant
plt.plot(tau, noncompliant_loss_all ,linestyle='-', label='all', alpha = 1)
plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
plt.xlabel('GCRA Bucket Depth τ(bits)')
plt.ylabel('Packet Loss Rate(%)')
plt.legend()
plt.grid(True)
plt.title('Uniform Distribution : Half Non-ompliant SLA(155)')
# plt.title(title+'Non-compliant')
plt.savefig(IMAGE_PATH_NONCOMPLIANT.format(name, 'all'))
plt.cla()



