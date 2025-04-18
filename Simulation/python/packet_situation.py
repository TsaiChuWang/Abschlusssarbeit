import configparser
import matplotlib.pyplot as plt
import pandas
import sys

# Path to the configuration file
CONFIGURATION_PATH = '../configuration/main.ini'

# Initialize the config parser
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

# Read parameters from the configuration file
state_r = float(config['traffic']['state_r'])

if(int(sys.argv[1]) == 0):
    IMAGE_PATH = config['simulation']['data_path'] + '/images/record_packet_situation_uniform_regular.png'
elif(int(sys.argv[1]) == 5):
    IMAGE_PATH = config['simulation']['data_path'] + '/images/record_packet_situation_{}.png'.format(state_r)
else:
    IMAGE_PATH = config['simulation']['data_path'] + '/images/record_packet_situation_uniform_regular.png'
# IMAGE_PATH = config['simulation']['data_path'] + '/record_packet_situation.png'
DATA_PATH = config['simulation']['data_path'] + '/record_packet_situation.csv'

# Load data from the CSV file into a DataFrame
dataframe = pandas.read_csv(DATA_PATH)

# Constants for plotting
m = 500
n = 1000  # Number of data points to plot
step = 10**9 * float(config['traffic']['packet_size']) / float(config['traffic']['input_rate'])

# Extract packets and dequeue data from the DataFrame
packets = dataframe['packets'].tolist()[m:n]
dequeue = dataframe['dequeue'].tolist()[m:n]

average = sum(packets)/len(packets)

# Generate time values for the x-axis
x = [i * step for i in range(len(packets))]

# Plotting the data
plt.plot(x, packets, linestyle='-', label='packets')
plt.plot(x, dequeue, linestyle='-', label='dequeue')
plt.plot(x, [average for i in x], linestyle='-', label='average({})'.format(average))

# Set plot title and labels
if(int(sys.argv[1]) == 0):
    plt.title('Packet Situation (Uniform Distribution)')
elif(int(sys.argv[1]) == 5):
    plt.title('Packet Situation (state_r = {})'.format(state_r))
else:
    plt.title('Packet Situation (Uniform Distribution)')
    
plt.xlabel('Time(ns)')
plt.ylabel('Packet Number')
plt.legend()
plt.grid(True)

# Save the plot as an image
plt.savefig(IMAGE_PATH)
plt.cla()  # Clear the current axes
