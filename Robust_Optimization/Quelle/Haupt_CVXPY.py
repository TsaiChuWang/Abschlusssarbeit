import cvxpy

nodes = 5
edges = 8
pairs = 3
capacity = 360.000000
error = 0.100000
mean_mu = [10.365820, 9.652754, 8.478728 ]
standard_derivation_sigma = [3.165820, 3.627076, 3.619346 ]

fst = cvxpy.Variable((pairs ,edges), nonneg=True)
alpha = cvxpy.Variable(nonneg=True)

objective = cvxpy.Minimize(alpha)
constraints = [
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][0] +    0.3757556*fst[1][0] +    0.4268737*fst[2][0], 2) <= capacity*alpha - (fst[0][0] + fst[1][0] + fst[2][0]),
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][1] +    0.3757556*fst[1][1] +    0.4268737*fst[2][1], 2) <= capacity*alpha - (fst[0][1] + fst[1][1] + fst[2][1]),
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][2] +    0.3757556*fst[1][2] +    0.4268737*fst[2][2], 2) <= capacity*alpha - (fst[0][2] + fst[1][2] + fst[2][2]),
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][3] +    0.3757556*fst[1][3] +    0.4268737*fst[2][3], 2) <= capacity*alpha - (fst[0][3] + fst[1][3] + fst[2][3]),
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][4] +    0.3757556*fst[1][4] +    0.4268737*fst[2][4], 2) <= capacity*alpha - (fst[0][4] + fst[1][4] + fst[2][4]),
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][5] +    0.3757556*fst[1][5] +    0.4268737*fst[2][5], 2) <= capacity*alpha - (fst[0][5] + fst[1][5] + fst[2][5]),
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][6] +    0.3757556*fst[1][6] +    0.4268737*fst[2][6], 2) <= capacity*alpha - (fst[0][6] + fst[1][6] + fst[2][6]),
	   1.2815516*cvxpy.norm(   0.3054095*fst[0][7] +    0.3757556*fst[1][7] +    0.4268737*fst[2][7], 2) <= capacity*alpha - (fst[0][7] + fst[1][7] + fst[2][7]),

	fst[0][0] - fst[0][3] ==   10.3658205,
	fst[1][0] - fst[1][3]== 0,
	fst[2][0] - fst[2][3]== 0,
	fst[0][1] - fst[0][4]== 0,
	fst[1][1] - fst[1][4] ==    9.6527543,
	fst[2][1] - fst[2][4]== 0,
	fst[0][2] - fst[0][5]== 0,
	fst[1][2] - fst[1][5]== 0,
	fst[2][2] - fst[2][5] ==    8.4787278,
	 - fst[0][0] - fst[0][1] - fst[0][2] + fst[0][3] + fst[0][4] + fst[0][5] + fst[0][6] - fst[0][7]== 0,
	 - fst[1][0] - fst[1][1] - fst[1][2] + fst[1][3] + fst[1][4] + fst[1][5] + fst[1][6] - fst[1][7]== 0,
	 - fst[2][0] - fst[2][1] - fst[2][2] + fst[2][3] + fst[2][4] + fst[2][5] + fst[2][6] - fst[2][7]== 0,
	 - fst[0][6] + fst[0][7]== -  10.3658205,
	 - fst[1][6] + fst[1][7]== -   9.6527543,
	 - fst[2][6] + fst[2][7]== -   8.4787278
			]

problem = cvxpy.Problem(objective, constraints)
problem.solve(solver=cvxpy.GLPK)
print("optimal value with GLPK:", problem.value)

