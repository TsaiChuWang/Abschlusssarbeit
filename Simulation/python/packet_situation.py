import configparser
import matplotlib.pyplot as plt
import pandas
import sys

# Path to the configuration file
if(len(sys.argv)<3):
    CONFIGURATION_PATH = '../configuration/main.ini'
else:
    CONFIGURATION_PATH = sys.argv[2]
# Initialize the config parser
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

DATA_PATH = config['simulation']['data_path'] + '/record_packet_situation.csv'
if(len(sys.argv)<2):
    IMAGE_PATH = config['simulation']['data_path'] + '/images/record_packet_situation.png'
else:
    IMAGE_PATH = config['simulation']['data_path'] + '/images/record_packet_situation_{}.png'.format(sys.argv[1])

# Load data from the CSV file into a DataFrame
dataframe = pandas.read_csv(DATA_PATH)

# # Constants for plotting
if(len(sys.argv)<4):
    m = 500
else:
    m = int(sys.argv[3])

if(len(sys.argv)<5):
    n= 1000
else:
    n = int(sys.argv[4])

# Read parameters from the configuration file
state_r = float(config['traffic']['state_r'])
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

plt.title('Packet Situation ({})'.format(sys.argv[1]))
    
plt.xlabel('Time(ns)')
plt.ylabel('Packet Number')
plt.legend()
plt.grid(True)

# Save the plot as an image
plt.savefig(IMAGE_PATH)
plt.cla()  # Clear the current axes
