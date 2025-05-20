import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

CONFIGURATION_PATH = '../configuration/main.ini'
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

# path = '/home/wtsaichu/Dokumente/Arbeitsplatz/Abschlusssarbeit/Simulation/data/unifrom_naughty_different_average/main'
# tail = '_uniform_ditribution_'

path = '/home/wtsaichu/Dokumente/Arbeitsplatz/Abschlusssarbeit/Simulation/data/burst_different_r_naughty'
tail = 'burst_'

NAUGHTY_MEAN = 1
R = 2
IMAGE_PATH = path+'/images/regular_and_naughty_all_'+tail+'{}.png'
DATA_PATH = path+'/regular_and_naughty_all.csv'

dataframe = pandas.read_csv(DATA_PATH)

if(int(sys.argv[1]) == NAUGHTY_MEAN):
    tenant_number = int(config['simulation']['tenant_number'])
    naughty_mean = int(config['traffic']['naughty_mean'])
    naughty_tenant_number = int(config['traffic']['naughty_tenant_number'])
    mean = int(config['traffic']['mean'])

    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()
    # print(real_naughty_mean)

    keys = [1,25,50,75,99]
    for key in keys:
        real_naughty_mean = (naughty_mean*key+ mean*(tenant_number-key))/tenant_number
        nloss = dataframe[dataframe['naughty_tenant_number'] == key][['naughty_loss']]
        if(len(nloss)!=0):
            # print('regular : '+str(nloss)+" "+str(key))
            tau = dataframe[dataframe['naughty_tenant_number'] == key]['tau'].tolist()
            # print(len(tau))
            # print(len(nloss))
            plt.plot(tau, nloss, linestyle='-', label='n = {}, μ = {}'.format(key, real_naughty_mean), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and μ (Non-Compliant SLA)')
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format('naughty'))
    plt.cla()

    for key in keys:
        real_naughty_mean = (naughty_mean*key+ mean*(tenant_number-key))/tenant_number
        rloss = dataframe[dataframe['naughty_tenant_number'] == key][['regular_loss']]
        # print('regular : '+str(rloss)+" "+str(key))
        if(len(rloss)!=0):
            tau = dataframe[dataframe['naughty_tenant_number'] == key]['tau'].tolist()
            tau = list(set(tau))
            tau.sort()
            print(len(tau))
            print(len(rloss))
            plt.plot(tau, rloss, linestyle='-', label='n = {}, μ = {}'.format(key, real_naughty_mean), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and μ (Compliant SLA)')
    plt.xlabel('τ')
    plt.ylabel('Loss (%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format('regular'))
    plt.cla()

if(int(sys.argv[1]) == R):

    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    keys = list(set(dataframe['state_r'].tolist()))
    keys.sort()

    for key in keys:
        nloss = dataframe[dataframe['state_r'] == key][['naughty_loss']]
        if(len(nloss)!=0):
            tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
            plt.plot(tau, nloss, linestyle='-', label='r = '+str(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss Rate with different τ and r (Non-compliant SLA Flow)')
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format('naughty'))
    plt.cla()

    for key in keys:
        rloss = dataframe[dataframe['state_r'] == key][['regular_loss']]
        if(len(rloss)!=0):
            tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
            tau = list(set(tau))
            tau.sort()
            plt.plot(tau, rloss, linestyle='-', label='r = '+str(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss Rate with different τ and r (Compliant SLA Flow)')
    plt.xlabel('τ')
    plt.ylabel('Loss (%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format('regular'))
    plt.cla()

# kind_key = list(set(dataframe['state_r'].tolist()))

# n = 1600
# for key in kind_key:
#     nloss = dataframe[dataframe['state_r'] == key][['naughty_loss']]
#     rloss = dataframe[dataframe['state_r'] == key][['regular_loss']]

#     nloss = nloss/100
#     rloss = rloss/100
#     plt.plot(tau[:n],nloss[:n], linestyle='-', label='(naghty)r = '+str(key), alpha = 1)
#     # plt.plot(tau[:n],rloss[:n], linestyle='-', label='(regular)r = '+str(key), alpha = 1)

# plt.plot(tau[:n], [0.1 for i in tau][:n], linestyle='-', color = 'red', label='ε')

# # plt.title('Average Packet Loss with different τ and r value (All Regular)')
# # plt.title('Packet Loss with different τ and r value (Naughty 150) Regular Flow')
# plt.title('Packet Loss with different τ and r value (Naughty 150) Naughty Flow')
# plt.ylabel('Loss (%)')
# plt.legend()
# plt.grid(True)

# plt.savefig(IMAGE_PATH)
# plt.cla()
