import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

CONFIGURATION_PATH = sys.argv[2]
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

name = sys.argv[1]

IMAGE_PATH = config['simulation']['data_path']+'/images/compliant_and_noncompliant_all_{}_{}_{}.png'
DATA_PATH = config['simulation']['data_path']+'/compliant_and_noncompliant_all.csv'

dataframe = pandas.read_csv(DATA_PATH)

if(name == 'uniform_different_noncompliant_number'):
    tenant_number = int(config['simulation']['tenant_number'])
    noncompliant_mean = int(config['traffic']['noncompliant_mean'])
    noncompliant_tenant_number = int(config['traffic']['noncompliant_tenant_number'])
    mean = int(config['traffic']['mean'])

    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    # pure
    keys = list(set(dataframe['noncompliant_tenant_number'].tolist()))
    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        noncompliant_loss_pure = dataframe[dataframe['noncompliant_tenant_number'] == key][['noncompliant_loss_pure']]
        if(len(noncompliant_loss_pure)!=0):
            tau = dataframe[dataframe['noncompliant_tenant_number'] == key]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)μ = {}, n = {:2d}'.format(real_noncompliant_mean, key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and μ ({}) Non-compliant'.format(name), fontsize=7.5)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'))
    plt.cla()

    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        compliant_loss_pure = dataframe[dataframe['noncompliant_tenant_number'] == key][['compliant_loss_pure']]
        if(len(compliant_loss_pure)!=0):
            tau = dataframe[dataframe['noncompliant_tenant_number'] == key]['tau'].tolist()
            plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)μ = {}, n = {:2d}'.format(real_noncompliant_mean, key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and μ ({}) Compliant'.format(name), fontsize=7.5)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'))
    plt.cla()

    # all
    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        noncompliant_loss_all = dataframe[dataframe['noncompliant_tenant_number'] == key][['noncompliant_loss_all']]
        if(len(noncompliant_loss_all)!=0):
            tau = dataframe[dataframe['noncompliant_tenant_number'] == key]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)μ = {}, n = {:2d}'.format(real_noncompliant_mean, key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and μ ({}) Non-compliant'.format(name), fontsize=7.5)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'))
    plt.cla()

    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        compliant_loss_all = dataframe[dataframe['noncompliant_tenant_number'] == key][['compliant_loss_all']]
        if(len(compliant_loss_all)!=0):
            tau = dataframe[dataframe['noncompliant_tenant_number'] == key]['tau'].tolist()
            plt.plot(tau, compliant_loss_all, linestyle='-', label='(all)μ = {}, n = {:2d}'.format(real_noncompliant_mean, key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and μ ({}) Compliant'.format(name), fontsize=7.5)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'))
    plt.cla()

if(name == 'burst_half_noncompliant'):
    tenant_number = int(config['simulation']['tenant_number'])
    noncompliant_mean = int(config['traffic']['noncompliant_mean'])
    noncompliant_tenant_number = int(config['traffic']['noncompliant_tenant_number'])
    mean = int(config['traffic']['mean'])

    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    # pure
    keys = list(set(dataframe['state_r'].tolist()))
    for key in keys:
        noncompliant_loss_pure = dataframe[dataframe['state_r'] == key][['noncompliant_loss_pure']]
        if(len(noncompliant_loss_pure)!=0):
            tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)r = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=12)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'))
    plt.cla()

    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        compliant_loss_pure = dataframe[dataframe['state_r'] == key][['compliant_loss_pure']]
        if(len(compliant_loss_pure)!=0):
            tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
            plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)r = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=12)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'))
    plt.cla()

    # all
    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        noncompliant_loss_all = dataframe[dataframe['state_r'] == key][['noncompliant_loss_all']]
        if(len(noncompliant_loss_all)!=0):
            tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)r = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=12)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'))
    plt.cla()

    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        compliant_loss_all = dataframe[dataframe['state_r'] == key][['compliant_loss_all']]
        if(len(compliant_loss_all)!=0):
            tau = dataframe[dataframe['state_r'] == key]['tau'].tolist()
            plt.plot(tau, compliant_loss_all, linestyle='-', label='(all)r = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=12)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'))
    plt.cla()

if(name == 'burst_noncompliant_different_r'):
    rs = list(set(dataframe['state_r'].tolist()).union(set(dataframe['noncompliant_state_r'].tolist())))
    keys = []
    for state_r in rs:
        for noncompliant_state_r in rs:
            if(noncompliant_state_r > state_r):
                keys.append([state_r, noncompliant_state_r])

    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    # pure
    for key in keys:
        noncompliant_loss_pure = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])][['noncompliant_loss_pure']]
        if(len(noncompliant_loss_pure)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)r(c) = {}, r(n) = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=10)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'))
    plt.cla()

    for key in keys:
        compliant_loss_pure = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])][['compliant_loss_pure']]
        if(len(compliant_loss_pure)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])]['tau'].tolist()
            plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)r(c) = {}, r(n) = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=10)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'))
    plt.cla()

    # all
    for key in keys:
        noncompliant_loss_all = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])][['noncompliant_loss_all']]
        if(len(noncompliant_loss_all)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)r(c) = {}, r(n) = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=10)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'))
    plt.cla()

    for key in keys:
        compliant_loss_all = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])][['compliant_loss_all']]
        if(len(compliant_loss_all)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['noncompliant_state_r'] == key[1])]['tau'].tolist()
            plt.plot(tau, compliant_loss_all, linestyle='-', label='(all)r(c) = {}, r(n) = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=10)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'))
    plt.cla()