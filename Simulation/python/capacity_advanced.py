import cvxpy
import configparser
import sys
import numpy as np
import math
import pandas as pd

# Usage: python3 ../python/capacity_advanced.py [DATA_PATH] [CONFIGURATION_PATH] [ACTION_CODE]
# Example: python3 ../python/capacity_advanced.py ../configuration/csv/advanced.csv ../configuration/advanced.ini 3

DATEI_CAPACITY_PATH = "../data/capacity_advanced.txt"

# Get command-line arguments or set default values
DATA_PATH = sys.argv[1] if len(sys.argv) > 1 else "../configuration/csv/advanced.csv"
CONFIGURATION_PATH = sys.argv[2] if len(sys.argv) > 2 else "../configuration/advanced.ini"
ACTION_CODE = int(sys.argv[3]) if len(sys.argv) > 3 else 0

# Define action codes
ACTION_CODE_NOTHING = 0
ACTION_CODE_PRINT_RESULT = 1
ACTION_CODE_PRINT_GAIN = 2
ACTION_CODE_PRINT_BOTH = 3

# Initialize variables
arr = []
total_mean_sum = 0

# Read configuration settings
config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)
error = float(config['simulation']['error'])

# Load data from CSV
dataframe = pd.read_csv(DATA_PATH)
mean = dataframe['mean'].tolist()
standard_deviation = dataframe['standard_deviation'].tolist()
number = dataframe['number'].tolist()

# Populate the array and calculate the total mean sum
for i in range(len(dataframe)):
    for j in range(number[i]):
        arr.append((standard_deviation[i] / mean[i]) * mean[i])
    total_mean_sum += (mean[i] * number[i])

# Define the optimization variable and problem
alpha = cvxpy.Variable(nonneg=True)
objective = cvxpy.Minimize(alpha)

# Define the constraints
constraints = [
    math.sqrt(2 * np.log(1 / error)) * np.linalg.norm(arr, ord=2) + total_mean_sum <= alpha
]

# Set up and solve the optimization problem
problem = cvxpy.Problem(objective, constraints)
problem.solve(solver=cvxpy.CVXOPT)

# Print results based on action code
if ACTION_CODE in (ACTION_CODE_PRINT_RESULT, ACTION_CODE_PRINT_BOTH):
    print("Optimal value with CVXOPT: {:.12f}".format(problem.value))

if ACTION_CODE in (ACTION_CODE_PRINT_GAIN, ACTION_CODE_PRINT_BOTH):
    upper_traffic = sum((mean[i] + standard_deviation[i]) * number[i] for i in range(len(dataframe)))
    print("Gain: {:.12f} - {:.12f} = {:.12f}".format(upper_traffic, problem.value, upper_traffic - problem.value))

# Write the result to a file
with open(DATEI_CAPACITY_PATH, 'w') as file:
    file.write(str(alpha.value))
