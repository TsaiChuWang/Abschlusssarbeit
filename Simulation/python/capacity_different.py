import cvxpy
import configparser
import sys
import numpy
import math
import pandas

# python3 ../python/capacity_different.py [CONFIGURATION_PATH] [ACTION_CODE]
# python3 ../python/capacity_different.py ../configuration/simple_V1.ini 3

DATEI_CAPACITY_PATH = "../data/capacity.txt"
DATA_PATH = "../configuration/different.csv"

dataframe = pandas.read_csv(DATA_PATH, header=None)

ACTION_CODE = int(sys.argv[2])

ACTION_CODE_NOTHING = 0
ACTION_CODE_PRINT_RESULT = 1
ACTION_CODE_PRINT_GAIN = 2
ACTION_CODE_PRINT_BOTH = 3

arr = []
sum = 0
error = 0.001

mean = dataframe[0].tolist()
standard_deviation = dataframe[1].tolist()
number = dataframe[2].tolist()

for i in range(len(dataframe)):
    for j in range(number[i]):
        arr.append((standard_deviation[i]/mean[i])*mean[i])
    sum+=(mean[i]*number[i])

alpha = cvxpy.Variable(nonneg=True)
objective = cvxpy.Minimize(alpha)

constraints = [ 
    math.sqrt(2*numpy.log((1/error)))*numpy.linalg.norm(arr, ord = 2) + (sum) <=alpha ]

problem = cvxpy.Problem(objective, constraints)
problem.solve(solver=cvxpy.CVXOPT)

if(ACTION_CODE == ACTION_CODE_PRINT_RESULT or ACTION_CODE == ACTION_CODE_PRINT_BOTH):
  print("optimal value with CVXOPT: {:.12f}".format(problem.value))

if(ACTION_CODE == ACTION_CODE_PRINT_GAIN or ACTION_CODE == ACTION_CODE_PRINT_BOTH):
  upper_traffic = 0
  for i in range(len(dataframe)):
     upper_traffic+=((mean[i]+standard_deviation[i])*number[i])
  print("Gain : {:.12f} - {:.12f} = {:.12f}".format(upper_traffic, problem.value, upper_traffic- problem.value))

with open(DATEI_CAPACITY_PATH, 'w') as file:
    file.write(str(alpha.value))
file.close()
