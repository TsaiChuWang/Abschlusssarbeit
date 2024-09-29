import cvxpy as cp
import math
import numpy as np

m = 4
n = 3
p = 3
x = cp.Variable(p, nonneg=True)
y = cp.Variable((n ,m), nonneg=True)
a = cp.Variable(nonneg=True)

c = 60

objective = cp.Minimize(a)

print(math.sqrt(2*np.log(1/0.1)))

constraints = [
    x[0] == 10.365820,
    x[1] == 9.65275435,
    x[2] == 8.4787278,

    y[0][0] == x[0],
    y[0][3] == x[0],

    y[1][1] == x[1],
    y[1][3] == x[1],

    y[2][2] == x[2],
    y[2][3] == x[2],

    2.145966026289347*cp.norm((y[0][0]*(3.1658205/10.3658205)), 2) <= a*c - y[0][0],
    2.145966026289347*cp.norm((y[1][1]*(3.6270764/9.6527543)), 2) <= a*c - y[1][1],
    2.145966026289347*cp.norm((y[2][2]*(3.6193458/8.4787278)), 2) <= a*c - y[2][2],
    2.145966026289347*cp.norm(((y[0][3]*(3.1658205/10.3658205))+(y[1][3]*(3.6270764/9.6527543))+(y[2][3]*(3.6193458/8.4787278))), 2) <= a*c - y[0][3] - y[1][3] -y[2][3]
]

# 构建问题
problem = cp.Problem(objective, constraints)

# 求解问题
# # Solve with ECOS.
# problem.solve(solver=cp.ECOS)
# print("optimal value with ECOS:", problem.value)

# Solve with ECOS_BB.
problem.solve(solver=cp.ECOS_BB)
print("optimal value with ECOS_BB:", problem.value)

# Solve with CVXOPT.
problem.solve(solver=cp.CVXOPT)
print("optimal value with CVXOPT:", problem.value)

# Solve with SCS.
problem.solve(solver=cp.SCS)
print("optimal value with SCS:", problem.value)

# Solve with GLPK.
problem.solve(solver=cp.GLPK)
print("optimal value with GLPK:", problem.value)

# Solve with GLPK_MI.
problem.solve(solver=cp.GLPK_MI)
print("optimal value with GLPK_MI:", problem.value)

# Solve with GUROBI.
problem.solve(solver=cp.GUROBI)
print("optimal value with GUROBI:", problem.value)

# Solve with MOSEK.
# problem.solve(solver=cp.MOSEK)
# print("optimal value with MOSEK:", problem.value)

# Solve with Elemental.
# problem.solve(solver=cp.ELEMENTAL)
# print("optimal value with Elemental:", problem.value)

# 输出结果
# print("Optimal value:", problem.value)
# print("Optimal x:", x.value)
# print("Optimal y:", y.value)
# print("Optimal a:", a.value)