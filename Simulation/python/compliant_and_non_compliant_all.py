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
    keys.sort()
    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        noncompliant_loss_pure = dataframe[dataframe['noncompliant_tenant_number'] == key][['noncompliant_loss_pure']]
        if(len(noncompliant_loss_pure)!=0):
            tau = dataframe[dataframe['noncompliant_tenant_number'] == key]['tau'].tolist()
            plt.plot(tau[:], noncompliant_loss_pure[:], linestyle='-', label='(pure)μ = {}, n = {:2d}'.format(real_noncompliant_mean, key), alpha = 1)
    plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and μ ({}) Non-compliant'.format(name), fontsize=7.5)
    plt.title('Non-compliant SLA')
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'))
    plt.cla()

    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        compliant_loss_pure = dataframe[dataframe['noncompliant_tenant_number'] == key][['compliant_loss_pure']]
        if(len(compliant_loss_pure)!=0):
            tau = dataframe[dataframe['noncompliant_tenant_number'] == key]['tau'].tolist()
            plt.plot(tau[:], compliant_loss_pure[:], linestyle='-', label='(pure)μ = {}, n = {:2d}'.format(real_noncompliant_mean, key), alpha = 1)
    plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and μ ({}) Compliant'.format(name), fontsize=7.5)
    plt.title('Compliant SLA')
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
            plt.plot(tau[:], noncompliant_loss_all[:], linestyle='-', label='n$_n$ = {:2d}'.format(key), alpha = 1)
    plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and μ ({}) Non-compliant'.format(name), fontsize=7.5)
    plt.title('Non-compliant SLA')
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'))
    plt.cla()

    keys.reverse()
    for key in keys:
        real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
        compliant_loss_all = dataframe[dataframe['noncompliant_tenant_number'] == key][['compliant_loss_all']]
        if(len(compliant_loss_all)!=0):
            tau = dataframe[dataframe['noncompliant_tenant_number'] == key]['tau'].tolist()
            plt.plot(tau[:], compliant_loss_all[:], linestyle='-', label='n$_c$ = {:2d}'.format(100-key), alpha = 1)
    plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and μ ({}) Compliant'.format(name), fontsize=7.5)
    plt.title('Compliant SLA')
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
    plt.title('Non-compliant SLA'.format(name), fontsize=12)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
            plt.plot(tau, noncompliant_loss_all, linestyle='-', label='r = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Non-compliant SLA', fontsize=12)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
    plt.title('Compliant SLA', fontsize=12)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
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
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'))
    plt.cla()

if(name == 'burst_half_noncompliant_different_FIFO'):
    rs = list(set(dataframe['state_r'].tolist()))
    queue_buffer = list(set(dataframe['upper_queue_buffer'].tolist()))
    # queue_buffer = [20,40,60,80]
    keys = []
    for state_r in rs:
        for buff in queue_buffer:
            keys.append([state_r, buff])

    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    # pure
    for key in keys:
        noncompliant_loss_pure = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])][['noncompliant_loss_pure']]
        if(len(noncompliant_loss_pure)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)r = {}, buffer = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=10)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'))
    plt.cla()

    for key in keys:
        compliant_loss_pure = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])][['compliant_loss_pure']]
        if(len(compliant_loss_pure)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])]['tau'].tolist()
            plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)r = {}, buffer = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=10)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'))
    plt.cla()

    # all
    for key in keys:
        noncompliant_loss_all = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])][['noncompliant_loss_all']]
        if(len(noncompliant_loss_all)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)r = {}, buffer = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=10)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'))
    plt.cla()

    for key in keys:
        compliant_loss_all = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])][['compliant_loss_all']]
        if(len(compliant_loss_all)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])]['tau'].tolist()
            plt.plot(tau, compliant_loss_all, linestyle='-', all='(pure)r = {}, buffer = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=10)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'))
    plt.cla()

if(name == 'uniform_different_noncompliant_number_FIFO'):
    tenant_number = int(config['simulation']['tenant_number'])
    noncompliant_mean = int(config['traffic']['noncompliant_mean'])
    noncompliant_tenant_number = int(config['traffic']['noncompliant_tenant_number'])
    mean = int(config['traffic']['mean'])

    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    noncompliant_tenant_numbers = list(set(dataframe['noncompliant_tenant_number'].tolist()))

    for n in noncompliant_tenant_numbers:
        # pure
        keys = list(set(dataframe['upper_queue_buffer'].tolist()))
        for key in keys:
            real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
            noncompliant_loss_pure = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)][['noncompliant_loss_pure']]
            if(len(noncompliant_loss_pure)!=0):
                tau = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)]['tau'].tolist()
                plt.plot(tau[:], noncompliant_loss_pure[:], linestyle='-', label='(pure)n = {}, buffer = {:d}'.format(n, key), alpha = 1)
        plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and μ ({}) Non-compliant'.format(name), fontsize=7.5)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'+str(n)))
        plt.cla()

        for key in keys:
            real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
            compliant_loss_pure = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)][['compliant_loss_pure']]
            if(len(compliant_loss_pure)!=0):
                tau = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)]['tau'].tolist()
                plt.plot(tau[:], compliant_loss_pure[:], linestyle='-', label='(pure)n = {}, buffer = {:2d}'.format(n, key), alpha = 1)
        plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and μ ({}) Compliant'.format(name), fontsize=7.5)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'+str(n)))
        plt.cla()

        # all
        for key in keys:
            real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
            noncompliant_loss_all = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)][['noncompliant_loss_all']]
            if(len(noncompliant_loss_all)!=0):
                tau = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)]['tau'].tolist()
                plt.plot(tau[:], noncompliant_loss_all[:], linestyle='-', label='(all)n = {}, buffer = {:2d}'.format(n, key), alpha = 1)
        plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and μ ({}) Non-compliant'.format(name), fontsize=7.5)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'+str(n)))
        plt.cla()

        for key in keys:
            real_noncompliant_mean = (noncompliant_mean*key+ mean*(tenant_number-key))/tenant_number
            compliant_loss_all = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)][['compliant_loss_all']]
            if(len(compliant_loss_all)!=0):
                tau = dataframe[(dataframe['upper_queue_buffer'] == key)& (dataframe['noncompliant_tenant_number'] == n)]['tau'].tolist()
                plt.plot(tau[:], compliant_loss_all[:], linestyle='-', label='(all)n = {}, buffer =  {:d}'.format(n, key), alpha = 1)
        plt.plot(tau_[:], [0.1 for i in tau_][:], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and μ ({}) Compliant'.format(name), fontsize=7.5)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'+str(n)))
        plt.cla()

if(name == 'burst_half_noncompliant_six_seven'):
    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    buffers = list(set(dataframe['upper_queue_buffer'].tolist()))
    print(buffers)

    for buffer in buffers:
        # pure
        keys = list(set(dataframe['state_r'].tolist()))
        for key in keys:
            noncompliant_loss_pure = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)][['noncompliant_loss_pure']]
            if(len(noncompliant_loss_pure)!=0):
                tau = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
                plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)r = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=12)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'+'_'+str(buffer)))
        plt.cla()

        for key in keys:
            compliant_loss_pure = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)][['compliant_loss_pure']]
            if(len(compliant_loss_pure)!=0):
                tau = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
                plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)r = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=12)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'+'_'+str(buffer)))
        plt.cla()

        # all
        for key in keys:
            noncompliant_loss_all = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)][['noncompliant_loss_all']]
            if(len(noncompliant_loss_all)!=0):
                tau = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
                plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)r = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and r ({}) Non-compliant'.format(name), fontsize=12)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'+'_'+str(buffer)))
        plt.cla()

        for key in keys:
            compliant_loss_all = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)][['compliant_loss_all']]
            if(len(compliant_loss_all)!=0):
                tau = dataframe[(dataframe['state_r'] == key) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
                plt.plot(tau, compliant_loss_all, linestyle='-', label='(all)r = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and r ({}) Compliant'.format(name), fontsize=12)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'+'_'+str(buffer)))
        plt.cla()

if(name == 'burst_half_noncompliant_7_25'):
    tau = dataframe['tau'].tolist()
    tau = list(set(tau))
    tau.sort()

    buffers = list(set(dataframe['upper_queue_buffer'].tolist()))
    title = 'Packet loss rate with different τ, r=0.7, n=25 '
    for buffer in buffers:
        compliant_loss_pure = dataframe[(dataframe['upper_queue_buffer'] == buffer)]['compliant_loss_pure'].tolist()
        noncompliant_loss_pure = dataframe[(dataframe['upper_queue_buffer'] == buffer)]['noncompliant_loss_pure'].tolist()
        compliant_loss_all = dataframe[(dataframe['upper_queue_buffer'] == buffer)]['compliant_loss_all'].tolist()
        noncompliant_loss_all = dataframe[(dataframe['upper_queue_buffer'] == buffer)]['noncompliant_loss_all'].tolist()

        # Pure
        # compliant
        plt.plot(tau, compliant_loss_pure ,linestyle='-', label='pure', alpha = 1)
        plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.title(title+'Compliant')
        plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'+'_'+str(buffer)))
        plt.cla()

        # noncompliant
        plt.plot(tau, noncompliant_loss_pure ,linestyle='-', label='pure', alpha = 1)
        plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.title(title+'Non-compliant')
        plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'+'_'+str(buffer)))
        plt.cla()

        # All
        # compliant
        plt.plot(tau, compliant_loss_all ,linestyle='-', label='all', alpha = 1)
        plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.title(title+'Compliant')
        plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'+'_'+str(buffer)))
        plt.cla()

        # noncompliant
        plt.plot(tau, noncompliant_loss_all ,linestyle='-', label='all', alpha = 1)
        plt.plot(tau, [float(config['simulation']['error'])*100.0 for i in tau], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.title(title+'Non-compliant')
        plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'+'_'+str(buffer)))
        plt.cla()

if(name == 'sensitive_noncompliant_number'):
    tau = dataframe['tau'].tolist()
    tau = list(set(tau))
    tau.sort()

    title = 'Packet loss rate with different Noncompliant Tenant Number '
    for t in tau:
        compliant_loss_pure = dataframe[(dataframe['tau'] == t)]['compliant_loss_pure'].tolist()
        noncompliant_loss_pure = dataframe[(dataframe['tau'] == t)]['noncompliant_loss_pure'].tolist()
        compliant_loss_all = dataframe[(dataframe['tau'] == t)]['compliant_loss_all'].tolist()
        noncompliant_loss_all = dataframe[(dataframe['tau'] == t)]['noncompliant_loss_all'].tolist()

        noncompliant_tenant_number = dataframe[(dataframe['tau'] == t)]['noncompliant_tenant_number'].tolist()
        # Pure
        # compliant
        plt.plot(noncompliant_tenant_number, compliant_loss_pure ,linestyle='-', label='pure', alpha = 1)
        plt.plot(noncompliant_tenant_number, [float(config['simulation']['error'])*100.0 for i in noncompliant_tenant_number], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('Noncompliant Tenant Number')
        plt.legend()
        plt.grid(True)
        plt.title(title+'Compliant')
        plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'+'_'+str(t)))
        plt.cla()

        # noncompliant
        plt.plot(noncompliant_tenant_number, noncompliant_loss_pure ,linestyle='-', label='pure', alpha = 1)
        plt.plot(noncompliant_tenant_number, [float(config['simulation']['error'])*100.0 for i in noncompliant_tenant_number], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('Noncompliant Tenant Number')
        plt.legend()
        plt.grid(True)
        plt.title(title+'Non-compliant')
        plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'+'_'+str(t)))
        plt.cla()

        # All
        # compliant
        plt.plot(noncompliant_tenant_number, compliant_loss_all ,linestyle='-', label='all', alpha = 1)
        plt.plot(noncompliant_tenant_number, [float(config['simulation']['error'])*100.0 for i in noncompliant_tenant_number], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('Noncompliant Tenant Number')
        plt.legend()
        plt.grid(True)
        plt.title('Compliant')
        plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'+'_'+str(t)))
        plt.cla()

        # noncompliant
        plt.plot(noncompliant_tenant_number, noncompliant_loss_all ,linestyle='-', label='all', alpha = 1)
        plt.plot(noncompliant_tenant_number, [float(config['simulation']['error'])*100.0 for i in noncompliant_tenant_number], linestyle='-', color = 'red', label='ε')
        plt.ylabel('Loss (%)')
        plt.xlabel('Noncompliant Tenant Number')
        plt.legend()
        plt.grid(True)
        plt.title(title+'Non-compliant')
        plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'+'_'+str(t)))
        plt.cla()

if(name == "uniform_half_noncompliant_ratio"):
    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()

    # pure
    keys = list(set(dataframe['ratio'].tolist()))
    for key in keys:
        noncompliant_loss_pure = dataframe[dataframe['ratio'] == key][['noncompliant_loss_pure']]
        if(len(noncompliant_loss_pure)!=0):
            tau = dataframe[dataframe['ratio'] == key]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)ratio = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'noncompliant'))
    plt.cla()

    # keys = [keys[2*i+1] for i in range(int(len(keys)/2))]

    for key in keys:
        compliant_loss_pure = dataframe[dataframe['ratio'] == key][['compliant_loss_pure']]
        if(len(compliant_loss_pure)!=0):
            tau = dataframe[dataframe['ratio'] == key]['tau'].tolist()
            plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)ratio = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and ratio ({}) Compliant'.format(name), fontsize=12)
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure', 'compliant'))
    plt.cla()

    # all
    for key in keys:
        noncompliant_loss_all = dataframe[dataframe['ratio'] == key][['noncompliant_loss_all']]
        if(len(noncompliant_loss_all)!=0):
            tau = dataframe[dataframe['ratio'] == key]['tau'].tolist()
            plt.plot(tau, noncompliant_loss_all, linestyle='-', label='ratio = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
    plt.title('Non-compliant SLA')
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'noncompliant'))
    plt.cla()

    for key in keys:
        compliant_loss_all = dataframe[dataframe['ratio'] == key][['compliant_loss_all']]
        if(len(compliant_loss_all)!=0):
            tau = dataframe[dataframe['ratio'] == key]['tau'].tolist()
            plt.plot(tau, compliant_loss_all, linestyle='-', label='ratio = {}'.format(key), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    # plt.title('Packet Loss with different τ and ratio ({}) Compliant'.format(name), fontsize=12)
    plt.title('Compliant SLA')
    plt.xlabel('GCRA Bucket Depth τ(bits)')
    plt.ylabel('Packet Loss Rate(%)')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all', 'compliant'))
    plt.cla()

if(name == "burst_half_noncompliant_ratio"):
    # tau_ = dataframe['tau'].tolist()
    # tau_ = list(set(tau_))
    # tau_.sort()
    # rs = list(set(dataframe['state_r'].tolist()))
    # for r in rs:
    #     # pure
    #     keys = list(set(dataframe['ratio'].tolist()))
    #     for key in keys:
    #         noncompliant_loss_pure = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)][['noncompliant_loss_pure']]
    #         # print(noncompliant_loss_pure )
    #         if(len(noncompliant_loss_pure)!=0):
    #             tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)]['tau'].tolist()
    #             plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)ratio = {}'.format(key), alpha = 1)
    #     plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    #     plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
    #     plt.ylabel('Loss (%)')
    #     plt.xlabel('τ (bits)')
    #     plt.legend()
    #     plt.grid(True)
    #     plt.savefig(IMAGE_PATH.format(name,'pure_', 'noncompliant'+str(r)+'_'))
    #     plt.cla()

    #     for key in keys:
    #         compliant_loss_pure = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)][['compliant_loss_pure']]
    #         if(len(compliant_loss_pure)!=0):
    #             tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)]['tau'].tolist()
    #             plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)ratio = {}'.format(key), alpha = 1)
    #     plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    #     plt.title('Packet Loss with different τ and ratio ({}) Compliant'.format(name), fontsize=12)
    #     plt.ylabel('Loss (%)')
    #     plt.xlabel('τ (bits)')
    #     plt.legend()
    #     plt.grid(True)
    #     plt.savefig(IMAGE_PATH.format(name,'pure_', 'compliant'+str(r)+'_'))
    #     plt.cla()

    #     # all
    #     for key in keys:
    #         noncompliant_loss_all = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)][['noncompliant_loss_all']]
    #         if(len(noncompliant_loss_all)!=0):
    #             tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)]['tau'].tolist()
    #             plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)ratio = {}'.format(key), alpha = 1)
    #     plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    #     plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
    #     plt.ylabel('Loss (%)')
    #     plt.xlabel('τ (bits)')
    #     plt.legend()
    #     plt.grid(True)
    #     plt.savefig(IMAGE_PATH.format(name,'all_', 'noncompliant'+str(r)+'_'))
    #     plt.cla()

    #     for key in keys:
    #         compliant_loss_all = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)][['compliant_loss_all']]
    #         if(len(compliant_loss_all)!=0):
    #             tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)]['tau'].tolist()
    #             plt.plot(tau, compliant_loss_all, linestyle='-', label='(all)ratio = {}'.format(key), alpha = 1)
    #     plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    #     plt.title('Packet Loss with different τ and ratio ({}) Compliant'.format(name), fontsize=12)
    #     plt.ylabel('Loss (%)')
    #     plt.xlabel('τ (bits)')
    #     plt.legend()
    #     plt.grid(True)
    #     plt.savefig(IMAGE_PATH.format(name,'all_', 'compliant'+str(r)+'_'))
    #     plt.cla()
    tau_ = dataframe['tau'].tolist()
    tau_ = list(set(tau_))
    tau_.sort()
    rs = [0.6]
    for r in rs:
        # pure
        keys = list(set(dataframe['ratio'].tolist()))
        keys = [keys[2*i+1] for i in range(int(len(keys)/2))]
        for key in keys:
            noncompliant_loss_pure = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)][['noncompliant_loss_pure']]
            # print(noncompliant_loss_pure )
            if(len(noncompliant_loss_pure)!=0):
                tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)]['tau'].tolist()
                plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)ratio = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'pure_', 'noncompliant'+str(r)+'_'))
        plt.cla()

        for key in keys:
            compliant_loss_pure = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)][['compliant_loss_pure']]
            if(len(compliant_loss_pure)!=0):
                tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)]['tau'].tolist()
                plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)ratio = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Packet Loss with different τ and ratio ({}) Compliant'.format(name), fontsize=12)
        plt.ylabel('Loss (%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'pure_', 'compliant'+str(r)+'_'))
        plt.cla()

        # all
        for key in keys:
            noncompliant_loss_all = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)][['noncompliant_loss_all']]
            if(len(noncompliant_loss_all)!=0):
                tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)]['tau'].tolist()
                plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)ratio = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Non-compliant SLA r={}'.format(r), fontsize=12)
        plt.ylabel('Packet Loss Rate(%)')
        plt.xlabel('GCRA Bucket Depth τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'all_', 'noncompliant'+str(r)+'_'))
        plt.cla()

        for key in keys:
            compliant_loss_all = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)][['compliant_loss_all']]
            if(len(compliant_loss_all)!=0):
                tau = dataframe[(dataframe['ratio'] == key) & (dataframe['state_r'] == r)  & (dataframe['upper_queue_buffer'] == 25)]['tau'].tolist()
                plt.plot(tau, compliant_loss_all, linestyle='-', label='(all)ratio = {}'.format(key), alpha = 1)
        plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
        plt.title('Compliant SLA r={}'.format(r), fontsize=12)
        plt.ylabel('GCRA Bucket Depth Packet Loss Rate(%)')
        plt.xlabel('τ (bits)')
        plt.legend()
        plt.grid(True)
        plt.savefig(IMAGE_PATH.format(name,'all_', 'compliant'+str(r)+'_'))
        plt.cla()