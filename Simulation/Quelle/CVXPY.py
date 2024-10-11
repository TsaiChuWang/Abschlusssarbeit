import cvxpy
import numpy
import math
import sys
import configparser

DATEI_OBJECTIVE_WEG = "../Datei/objective.txt"
DATEI_FRACTION_WEG = "../Datei/fraction.csv"

alpha = cvxpy.Variable(nonneg=True)
objective = cvxpy.Minimize(alpha)

def validation(error, mean, standard_deviation, tenant_number, capacity):
    number = math.sqrt(2*numpy.log((1/error)))
    print("Validation")
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sqrt(2*ln(1/error))", number, number))
    number = number*math.sqrt(sum([((standard_deviation/mean)*mean)*((standard_deviation/mean)*mean) for i in range(tenant_number)]))
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sqrt(sum((standard_deviation)^2))", math.sqrt(sum([((standard_deviation/mean)*mean)*((standard_deviation/mean)*mean) for i in range(tenant_number)])), number))
    number = number + mean*tenant_number
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sum(mean) = ", mean*tenant_number, number))
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("sum all = ", number, number))
    print("{:33s} = {:<14.10f} | {:<14.10f}".format("alpha = ", number/capacity, number/capacity))

if(int(sys.argv[1]) == 0):
    config = configparser.ConfigParser()
    config.read(sys.argv[2])
    error = float(config['simulation']['error'])
    standard_deviation = float(config['traffic']['standard_deviation'])
    mean = float(config['traffic']['mean'])
    tenant_number = int(config['simulation']['tenant_number'])
    capacity = float(config['simulation']['capacity'])

    print(error)
    constraints = [ math.sqrt(2*numpy.log((1/error)))*numpy.linalg.norm([(standard_deviation/mean)*mean for i in range(tenant_number)], ord = 2) + (mean*tenant_number) <=alpha*capacity ]
    
    problem = cvxpy.Problem(objective, constraints)
    problem.solve(solver=cvxpy.CVXOPT)
    print("optimal value with CVXOPT: {:.12f}".format(problem.value))
    with open(DATEI_OBJECTIVE_WEG, 'w') as file:
        file.write(str(alpha.value))
    file.close()

if(int(sys.argv[1]) == 1):
    config = configparser.ConfigParser()
    config.read(sys.argv[2])

    error = float(config['simulation']['error'])
    standard_deviation = float(config['traffic']['standard_deviation'])
    mean = float(config['traffic']['mean'])
    tenant_number = int(config['simulation']['tenant_number'])
    capacity = float(config['simulation']['capacity'])

    validation(error, mean, standard_deviation, tenant_number, capacity)

if(int(sys.argv[1]) == 2):
    config = configparser.ConfigParser()
    config.read(sys.argv[2])
    error = float(config['simulation']['error'])
    standard_deviation = float(config['traffic']['standard_deviation'])
    mean = float(config['traffic']['mean'])
    tenant_number = int(config['simulation']['tenant_number'])
    capacity = float(sys.argv[3])

    constraints = [ math.sqrt(2*numpy.log((1/error)))*numpy.linalg.norm([(standard_deviation/mean)*mean for i in range(tenant_number)], ord = 2) + (mean*tenant_number) <=alpha*capacity ]
    
    problem = cvxpy.Problem(objective, constraints)
    problem.solve(solver=cvxpy.CVXOPT)
    print("optimal value with CVXOPT: {:.12f}".format(problem.value))
    with open(DATEI_OBJECTIVE_WEG, 'w') as file:
        file.write(str(alpha.value))
    file.close()

if(int(sys.argv[1]) == 3):
    config = configparser.ConfigParser()
    config.read(sys.argv[2])
    error = float(config['simulation']['error'])
    standard_deviation = float(config['traffic']['standard_deviation'])
    mean = float(config['traffic']['mean'])
    tenant_number = int(config['simulation']['tenant_number'])
    capacity = float(config['simulation']['capacity'])

    constraints = [ math.sqrt(2*numpy.log((1/error)))*numpy.linalg.norm([(standard_deviation/mean)*mean for i in range(tenant_number)], ord = 2) + (mean*tenant_number) <=alpha*capacity ]
    
    problem = cvxpy.Problem(objective, constraints)
    problem.solve(solver=cvxpy.CVXOPT)
    with open(DATEI_OBJECTIVE_WEG, 'w') as file:
        file.write(str(alpha.value))
    file.close()

if(int(sys.argv[1]) == 4):
    config = configparser.ConfigParser()
    config.read(sys.argv[2])
    error = float(config['simulation']['error'])
    standard_deviation = float(config['traffic']['standard_deviation'])
    mean = float(config['traffic']['mean'])
    tenant_number = int(config['simulation']['tenant_number'])
    capacity = float(sys.argv[3])

    constraints = [ math.sqrt(2*numpy.log((1/error)))*numpy.linalg.norm([(standard_deviation/mean)*mean for i in range(tenant_number)], ord = 2) + (mean*tenant_number) <=alpha*capacity ]
    
    problem = cvxpy.Problem(objective, constraints)
    problem.solve(solver=cvxpy.CVXOPT)
    with open(DATEI_OBJECTIVE_WEG, 'w') as file:
        file.write(str(alpha.value))
    file.close()
# # nodes = 5
# # edges = 4
# # pairs = 3
# # capacity = float(sys.argv[1])
# # error = 0.0100000
# tenant_number = int(sys.argv[2])
# mean = float(sys.argv[3])
# standard_deviation_sigma = float(sys.argv[4])

# # fst = cvxpy.Variable(pairs, nonneg=True)
# # fst = cvxpy.Variable(nonneg=True)



# if int(sys.argv[1])==0:
# 	constraints = [
#         math.sqrt(2*numpy.log((1/error)))*cvxpy.norm((standard_deviation_sigma/mean)*mean*tenant_number, 2) + fst <=alpha*capacity,
#     ]

# problem = cvxpy.Problem(objective, constraints)
# problem.solve(solver=cvxpy.CVXOPT)
# print("optimal value with CVXOPT: {:.12f}".format(problem.value))
# with open(DATEI_OBJECTIVE_WEG, 'w') as file:
# 	file.write(str(alpha.value))
# file.close()
# print(problem.value*360)

# fraction = numpy.array(fst.value)
# numpy.savetxt(DATEI_FRACTION_WEG, fst.value, delimiter=',')

