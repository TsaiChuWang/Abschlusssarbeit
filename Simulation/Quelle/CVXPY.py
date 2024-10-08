import cvxpy
import numpy
import math
import sys

DATEI_OBJECTIVE_WEG = "../Datei/objective.txt"
DATEI_FRACTION_WEG = "../Datei/fraction.csv"

nodes = 5
edges = 4
pairs = 3
capacity = float(sys.argv[1])
error = 0.0100000
mean_mu = 120
standard_derivation_sigma = 40

# fst = cvxpy.Variable(pairs, nonneg=True)
fst = cvxpy.Variable(nonneg=True)
alpha = cvxpy.Variable(nonneg=True)
objective = cvxpy.Minimize(alpha)
constraints = [
	# math.sqrt(2*numpy.log((1/error)))*cvxpy.norm(   (standard_derivation_sigma/mean_mu)*fst[0] +    (standard_derivation_sigma/mean_mu)*fst[1]+    (standard_derivation_sigma/mean_mu)*fst[2], 2) +(fst[0] + fst[1] + fst[2])<= capacity*alpha  ,
    math.sqrt(2*numpy.log((1/error)))*cvxpy.norm(   (standard_derivation_sigma/mean_mu)*fst , 2) + fst <=alpha*capacity,


    fst == mean_mu
    # fst[0] ==   mean_mu,

    # fst[1] ==   mean_mu,
]

problem = cvxpy.Problem(objective, constraints)
problem.solve(solver=cvxpy.CVXOPT)
print("optimal value with CVXOPT: {:.12f}".format(problem.value))
with open(DATEI_OBJECTIVE_WEG, 'w') as file:
	file.write(str(alpha.value))
file.close()
print(problem.value*360)

fraction = numpy.array(fst.value)
# numpy.savetxt(DATEI_FRACTION_WEG, fst.value, delimiter=',')

