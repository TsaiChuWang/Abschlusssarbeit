import cvxpy as cp
import math
import numpy as np
import random

m = 4
n = 3
p = 3
x = cp.Variable(p, nonneg=True)
y = cp.Variable((n ,m), nonneg=True)
a = cp.Variable(nonneg=True)

c = 360
error = 0.1

def obtain_Gap(capacity, index, OD_pair_number):
    mean_origin = capacity/OD_pair_number - capacity/(OD_pair_number*OD_pair_number)
    step_size = capacity/(OD_pair_number*OD_pair_number*OD_pair_number)
    lower_bound = mean_origin - random.uniform(0, index)*step_size
    upper_bound = mean_origin + random.uniform(0, index)*step_size
    return lower_bound, upper_bound

def obtain_mean(lower_bound, upper_bound):
    return (lower_bound+upper_bound)/2

def obtain_sigma(lower_bound, upper_bound):
    return (upper_bound-lower_bound)/2

objective = cp.Minimize(a)

print(math.sqrt(2*np.log(1/0.1)))

gap = [
    obtain_Gap(c, 1, 3),
    obtain_Gap(c, 2, 3),
    obtain_Gap(c, 2, 3),
]

mean = [
    obtain_mean(gap[0][0], gap[0][1]),
    obtain_mean(gap[1][0], gap[1][1]),
    obtain_mean(gap[2][0], gap[2][1])
]

sigma =[
    obtain_sigma(gap[0][0], gap[0][1]),
    obtain_sigma(gap[1][0], gap[1][1]),
    obtain_sigma(gap[2][0], gap[2][1])
]

print(mean)
constraints = [
    x[0] == mean[0],
    x[1] == mean[1],
    x[2] == mean[2],

    y[0][0] == x[0],
    y[0][3] == x[0],

    y[1][1] == x[1],
    y[1][3] == x[1],

    y[2][2] == x[2],
    y[2][3] == x[2],

    math.sqrt(2*np.log(1/(1-error)))*cp.norm((y[0][0]*(sigma[0]/mean[0])), 2) <= a*c - y[0][0],
    math.sqrt(2*np.log(1/(1-error)))*cp.norm((y[1][1]*(sigma[1]/mean[1])), 2) <= a*c - y[1][1],
    math.sqrt(2*np.log(1/(1-error)))*cp.norm((y[2][2]*(sigma[2]/mean[2])), 2) <= a*c - y[2][2],
    math.sqrt(2*np.log(1/(1-error)))*cp.norm(((y[0][3]*(sigma[0]/mean[0]))+(y[1][3]*(sigma[1]/mean[1]))+(y[2][3]*(sigma[2]/mean[2]))), 2) <= a*c - y[0][3] - y[1][3] -y[2][3]
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