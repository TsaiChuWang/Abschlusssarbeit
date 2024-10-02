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
error = 0.100000
mean_mu = 120
standard_derivation_sigma = 40

fst = cvxpy.Variable((pairs ,edges), nonneg=True)
alpha = cvxpy.Variable(nonneg=True)
objective = cvxpy.Minimize(alpha)
constraints = [
	   math.sqrt(2*numpy.log((1/error)))*cvxpy.norm(   (standard_derivation_sigma/mean_mu)*fst[0][0] +    (standard_derivation_sigma/mean_mu)*fst[1][0] +    (standard_derivation_sigma/mean_mu)*fst[2][0], 2) <= capacity*alpha - (fst[0][0] + fst[1][0] + fst[2][0]) ,
	   math.sqrt(2*numpy.log((1/error)))*cvxpy.norm(   (standard_derivation_sigma/mean_mu)*fst[0][1] +    (standard_derivation_sigma/mean_mu)*fst[1][1] +    (standard_derivation_sigma/mean_mu)*fst[2][1], 2) <= capacity*alpha - (fst[0][1] + fst[1][1] + fst[2][1]) ,
	   math.sqrt(2*numpy.log((1/error)))*cvxpy.norm(   (standard_derivation_sigma/mean_mu)*fst[0][2] +    (standard_derivation_sigma/mean_mu)*fst[1][2] +    (standard_derivation_sigma/mean_mu)*fst[2][2], 2) <= capacity*alpha - (fst[0][2] + fst[1][2] + fst[2][2]) ,
	   math.sqrt(2*numpy.log((1/error)))*cvxpy.norm(   (standard_derivation_sigma/mean_mu)*fst[0][3] +    (standard_derivation_sigma/mean_mu)*fst[1][3] +    (standard_derivation_sigma/mean_mu)*fst[2][3], 2) <= capacity*alpha - (fst[0][3] + fst[1][3] + fst[2][3]) ,


    fst[0][0] ==   mean_mu,
    fst[0][3] ==   mean_mu,

    fst[1][0] ==   mean_mu,
    fst[1][3] ==   mean_mu,

    fst[2][0] ==   mean_mu,
    fst[2][3] ==   mean_mu,
]

problem = cvxpy.Problem(objective, constraints)
problem.solve(solver=cvxpy.CVXOPT)
print("optimal value with CVXOPT: {:.12f}".format(problem.value))
with open(DATEI_OBJECTIVE_WEG, 'w') as file:
	file.write(str(alpha.value))
file.close()

fraction = numpy.array(fst.value)
numpy.savetxt(DATEI_FRACTION_WEG, fst.value, delimiter=',')

