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
