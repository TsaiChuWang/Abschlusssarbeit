import cvxpy
import configparser
import sys
import numpy
import math

# python3 ../python/capacity.py [CONFIGURATION_PATH] [ACTION_CODE]
# python3 ../python/capacity.py ../configuration/simple_V1.ini 3

DATEI_CAPACITY_PATH = "../data/capacity.txt"

CONFIGURATION_PATH = sys.argv[1]
ACTION_CODE = int(sys.argv[2])

ACTION_CODE_NOTHING = 0
ACTION_CODE_PRINT_RESULT = 1
ACTION_CODE_PRINT_GAIN = 2
ACTION_CODE_PRINT_BOTH = 3

alpha = cvxpy.Variable(nonneg=True)
objective = cvxpy.Minimize(alpha)

config = configparser.ConfigParser()
config.read(CONFIGURATION_PATH)

error = float(config['simulation']['error'])
mean = float(config['traffic']['mean'])
standard_deviation = float(config['traffic']['standard_deviation'])
tenant_number = int(config['simulation']['tenant_number'])

constraints = [ 
    math.sqrt(2*numpy.log((1/error)))*numpy.linalg.norm([(standard_deviation/mean)*mean for i in range(tenant_number)], ord = 2) + (mean*tenant_number) <=alpha ]

problem = cvxpy.Problem(objective, constraints)
problem.solve(solver=cvxpy.CVXOPT)

if(ACTION_CODE == ACTION_CODE_PRINT_RESULT or ACTION_CODE == ACTION_CODE_PRINT_BOTH):
  print("optimal value with CVXOPT: {:.12f}".format(problem.value))

if(ACTION_CODE == ACTION_CODE_PRINT_GAIN or ACTION_CODE == ACTION_CODE_PRINT_BOTH):
  upper_traffic = tenant_number*(mean+standard_deviation)
  print("Gain : {:.12f} - {:.12f} = {:.12f}".format(upper_traffic, problem.value, upper_traffic- problem.value))

with open(DATEI_CAPACITY_PATH, 'w') as file:
    file.write(str(alpha.value))
file.close()
