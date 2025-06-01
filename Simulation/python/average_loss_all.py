import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

# Get configuration file path from command line arguments
CONFIGURATION_PATH = sys.argv[2]
# Initialize and read configuration file
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

# Define file paths using configuration settings
IMAGE_PATH = config['simulation']['data_path']+'/images/average_loss_all_{}_{}.png'
DATA_PATH = config['simulation']['data_path']+'/record_average_loss.csv'

# Load simulation data from CSV file
dataframe = pandas.read_csv(DATA_PATH)
# Get name identifier from command line arguments
name = sys.argv[1]

# Extract tau values from the dataframe and convert to list
tau = dataframe['tau'].tolist()
# Remove duplicate tau values by converting to set then back to list
tau = list(set(tau))
# Sort tau values in ascending order
tau.sort()

# Check if the analysis type is for burst all compliant scenario
if(name == 'burst_all_compliant'):
    # Extract unique state_r values from dataframe and convert to list
    kind_key = list(set(dataframe['state_r'].tolist()))
    
    # Generate plot for pure packet loss data
    for key in kind_key:
        # Filter dataframe by state_r value and extract pure loss data
        loss = dataframe[dataframe['state_r'] == key][['average_loss_pure']]
        # Plot pure loss data for current r value with line style and label
        plt.plot(tau, loss, linestyle='-', label='(pure)r = '+str(key), alpha = 1)

    # Add horizontal reference line at 0.1 (epsilon threshold)
    plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')
    
    # Configure plot appearance for pure loss chart
    plt.title('Average Packet Loss Rate with different τ and r value (All Compliant)', fontsize=12)
    plt.xlabel('τ')
    plt.ylabel('Loss (%)')
    plt.legend()
    plt.grid(True)
    
    # Save pure loss plot to file and clear current plot
    plt.savefig(IMAGE_PATH.format(name, 'pure'))
    plt.cla()

    # Generate plot for all packet loss data
    for key in kind_key:
        # Filter dataframe by state_r value and extract all loss data
        loss = dataframe[dataframe['state_r'] == key][['average_loss_all']]
        # Plot all loss data for current r value with line style and label
        plt.plot(tau, loss, linestyle='-', label='(all)r = '+str(key), alpha = 1)

    # Add horizontal reference line at 0.1 (epsilon threshold)
    plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='ε')
    
    # Configure plot appearance for all loss chart
    plt.title('Average Packet Loss Rate with different τ and r value (All Compliant)', fontsize=12)
    plt.xlabel('τ')
    plt.ylabel('Loss (%)')
    plt.legend()
    plt.grid(True)
    
    # Save all loss plot to file and clear current plot
    plt.savefig(IMAGE_PATH.format(name, 'all'))
    plt.cla()

if(name == 'burst_all_compliant_different_FIFO'):
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
        average_loss_pure = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])][['average_loss_pure']]
        if(len(average_loss_pure)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])]['tau'].tolist()
            plt.plot(tau, average_loss_pure, linestyle='-', label='(pure)r = {}, buffer = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({})'.format(name), fontsize=10)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'pure'))
    plt.cla()

    # all
    for key in keys:
        average_loss_all = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])][['average_loss_all']]
        if(len(average_loss_all)!=0):
            tau = dataframe[(dataframe['state_r'] == key[0]) & (dataframe['upper_queue_buffer'] == key[1])]['tau'].tolist()
            plt.plot(tau, average_loss_all, linestyle='-', label='(all)r = {}, buffer = {}'.format(key[0], key[1]), alpha = 1)
    plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
    plt.title('Packet Loss with different τ and r ({})'.format(name), fontsize=10)
    plt.ylabel('Loss (%)')
    plt.xlabel('τ')
    plt.legend()
    plt.grid(True)
    plt.savefig(IMAGE_PATH.format(name,'all'))
    plt.cla()
