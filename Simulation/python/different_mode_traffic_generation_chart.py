import matplotlib.pyplot as plt  # Import Matplotlib for plotting
import pandas  # Import Pandas for data handling
import numpy as np  # Import NumPy for numerical operations
import sys  # Import sys to handle command-line arguments

# Define file paths
IMAGE_PATH = '../data/different_mode_traffic_generation_chart.png'  # Path to save the generated chart
DATA_PATH = '../data/different_mode_traffic_generation_chart.csv'  # Path to the CSV data file

# Read data from CSV file into a Pandas DataFrame
dataframe = pandas.read_csv(DATA_PATH)

# Extract data columns as lists
ru = dataframe["ru"].tolist()
nu = dataframe["nu"].tolist()

r875 = dataframe["r0.875"].tolist()
n875 = dataframe["n0.875"].tolist()

r9 = dataframe["r0.9"].tolist()
n9 = dataframe["n0.9"].tolist()

r9125 = dataframe["r0.9125"].tolist()
n9125 = dataframe["n0.9125"].tolist()

r925 = dataframe["r0.925"].tolist()
n925 = dataframe["n0.925"].tolist()

r9375 = dataframe["r0.9375"].tolist()
n9375 = dataframe["n0.9375"].tolist()

r95 = dataframe["r0.95"].tolist()
n95 = dataframe["n0.95"].tolist()

r9625 = dataframe["r0.9625"].tolist()
n9625 = dataframe["n0.9625"].tolist()

r975 = dataframe["r0.975"].tolist()
n975 = dataframe["n0.975"].tolist()

# Get command-line arguments
a = sys.argv[1]  # First argument (possibly a mode or type of analysis)
rn = sys.argv[2]  # Second argument (possibly a specific dataset identifier)

step_size = 40  # Define step size for processing or visualization

def sum_chunks(arr, n):
    # List comprehension that iterates over the array in chunks of size 'n'
    return [sum(arr[i:i + n])/n for i in range(0, len(arr), n)]
    # For each chunk, calculate the sum and divide it by 'n' to get the average of that chunk

# Check if the distribution is uniform ('u') and regular ('r')
if(a == 'u' and rn == 'r'):
  # Sum chunks of 'ru' with step size
  ru = sum_chunks(ru, step_size)
  # Generate x values for the plot
  x = [(i+1)*3051*step_size for i in range(len(ru))]
  # Plot the regular distribution
  plt.plot(x, ru, linestyle='-', label='regular')
  # Plot the predicted mean as a red line
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  # Set the plot title
  plt.title('Uniform Distribution Regular')

# Check if the distribution is uniform ('u') and naughty ('n')
if(a == 'u' and rn == 'n'):
  # Sum chunks of 'nu' with step size
  nu = sum_chunks(nu, step_size)
  # Generate x values for the plot
  x = [(i+1)*3051*step_size for i in range(len(nu))]
  # Plot the naughty distribution
  plt.plot(sum_chunks(x, step_size), sum_chunks(nu, step_size), linestyle='-', label='naughty')
  # Plot the predicted mean as a red line
  plt.plot(x, [0.875 for i in x], linestyle='-', color = 'red', label='predicted mean')
  # Set the plot title
  plt.title('Uniform Distribution Naughty')

# Check if the state machine r value is 0.875 and regular ('r')
if(a == '875' and rn == 'r'):
  r875 = sum_chunks(r875, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r875))]
  plt.plot(x, r875, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.875 Regular')

# Check if the state machine r value is 0.875 and naughty ('n')
if(a == '875' and rn == 'n'):
  n875 = sum_chunks(n875, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n875))]
  plt.plot(sum_chunks(x, step_size), sum_chunks(n875, step_size), linestyle='-', label='naughty')
  plt.plot(x, [0.875 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.875 Naughty')

# Check if the state machine r value is 0.9 and regular ('r')
if(a == '9' and rn == 'r'):
  r9 = sum_chunks(r9, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r9))]
  plt.plot(x, r9, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9 Regular')

# Check if the state machine r value is 0.9 and naughty ('n')
if(a == '9' and rn == 'n'):
  n9 = sum_chunks(n9, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n9))]
  plt.plot(sum_chunks(x, step_size), sum_chunks(n9, step_size), linestyle='-', label='naughty')
  plt.plot(x, [0.875 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9 Naughty')

# Check if the state machine r value is 0.9125 and regular ('r')
if(a == '9125' and rn == 'r'):
  r9125 = sum_chunks(r9125, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r9125))]
  plt.plot(x, r9125, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9125 Regular')

# Check if the state machine r value is 0.9125 and naughty ('n')
if(a == '9125' and rn == 'n'):
  n9125 = sum_chunks(n9125, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n9125))]
  plt.plot(x, n9125, linestyle='-', label='naughty')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9125 Naughty')

# Check if the state machine r value is 0.925 and regular ('r')
if(a == '925' and rn == 'r'):
  r925 = sum_chunks(r925, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r925))]
  plt.plot(x, r925, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.925 Regular')

# Check if the state machine r value is 0.925 and naughty ('n')
if(a == '925' and rn == 'n'):
  n925 = sum_chunks(n925, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n925))]
  plt.plot(x, n925, linestyle='-', label='naughty')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.925 Naughty')

# Check if the state machine r value is 0.9375 and regular ('r')
if(a == '9375' and rn == 'r'):
  r9375 = sum_chunks(r9375, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r9375))]
  plt.plot(x, r9375, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9375 Regular')

# Check if the state machine r value is 0.9375 and naughty ('n')
if(a == '9375' and rn == 'n'):
  n9375 = sum_chunks(n9375, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n9375))]
  plt.plot(x, n9375, linestyle='-', label='naughty')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9375 Naughty')

# Check if the state machine r value is 0.95 and regular ('r')
if(a == '95' and rn == 'r'):
  r95 = sum_chunks(r95, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r95))]
  plt.plot(x, r95, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.95 Regular')

# Check if the state machine r value is 0.95 and naughty ('n')
if(a == '95' and rn == 'n'):
  n95 = sum_chunks(n95, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n95))]
  plt.plot(x, n95, linestyle='-', label='naughty')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.95 Naughty')

# Check if the state machine r value is 0.9625 and regular ('r')
if(a == '9625' and rn == 'r'):
  r9625 = sum_chunks(r9625, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r9625))]
  plt.plot(x, r9625, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9625 Regular')

# Check if the state machine r value is 0.9625 and naughty ('n')
if(a == '9625' and rn == 'n'):
  n9625 = sum_chunks(n9625, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n9625))]
  plt.plot(x, n9625, linestyle='-', label='naughty')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.9625 Naughty')

# Check if the state machine r value is 0.975 and regular ('r')
if(a == '975' and rn == 'r'):
  r975 = sum_chunks(r975, step_size)
  x = [(i+1)*3051*step_size for i in range(len(r975))]
  plt.plot(x, r975, linestyle='-', label='regular')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.975 Regular')

# Check if the state machine r value is 0.975 and naughty ('n')
if(a == '975' and rn == 'n'):
  n975 = sum_chunks(n975, step_size)
  x = [(i+1)*3051*step_size for i in range(len(n975))]
  plt.plot(x, n975, linestyle='-', label='naughty')
  plt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='predicted mean')
  plt.title('State Machine r= 0.975 Naughty')

# Set the label for the x-axis
plt.xlabel('Time')
# Set the label for the y-axis
plt.ylabel('Packet')
# Display the legend, which will show the labels for different plot lines
plt.legend()
# Add gridlines to the plot for better readability
plt.grid(True)
# Save the current plot to an image file
plt.savefig(IMAGE_PATH)
# Clear the current plot to prepare for the next plot
plt.cla()

