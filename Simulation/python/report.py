    
import matplotlib.pyplot as plt

IMAGE_PATH = "../data/report.png"

x_axis = []
y_axis = []     
# print(len(x_axis))
# print(len(y_axis))
plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='GCRA 1 Loss')
plt.plot(x_axis, [0.1 for row in x_axis], linestyle='dotted', color='red', label='Epsilon')

plt.title('Different τ1 with Packet GCRA 1 Loss')
plt.xlabel('τ1')
plt.ylabel('GCRA 1 Loss')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()

import numpy as np

# 定義狀態空間與機率分佈
len_ = 10
states = np.arange(0, len_+1)  # 狀態空間: [0, 1, ..., 10]
p = {
    1: 0.2494728051203035,
    0: 0.5003244838153312,
    -1: 0.13367697621205613,
    -2: 0.06328121883870653,
    -3: 0.029956637045408463,
    -4: 0.014181144414390385,
    -5: 0.006713198701074475,
    -6: 0.0031779548591562208,
    -7: 0.001504409080758802,
    -8: 0.000712170808766761
}

# 初始化轉移矩陣
n_states = len(states)
transition_matrix = np.zeros((n_states, n_states))

# transition_matrix[0][0] = 1- 0.2494728051203035
# transition_matrix[0][1] = 0.2494728051203035

# transition_matrix[1][1] = 0.5003244838153312+0.2494728051203035
# transition_matrix[1][0] = 1- transition_matrix[1][1]
# print(p.keys())

for i in range(len_+1):
    if(i-8>0):
      transition_matrix[i][i-8] = 0.000712170808766761
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.000712170808766761

    if(i-7>0):
      transition_matrix[i][i-7] = 0.001504409080758802
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.001504409080758802

    if(i-6>0):
      transition_matrix[i][i-6] = 0.0031779548591562208
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.0031779548591562208

    if(i-5>0):
      transition_matrix[i][i-5] = 0.006713198701074475
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.006713198701074475

    if(i-4>0):
      transition_matrix[i][i-4] = 0.014181144414390385
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.014181144414390385

    if(i-3>0):
      transition_matrix[i][i-3] = 0.029956637045408463
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.029956637045408463

    if(i-2>0):
      transition_matrix[i][i-2] = 0.06328121883870653
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.06328121883870653

    if(i-1>0):
      transition_matrix[i][i-1] = 0.13367697621205613
    else:
      transition_matrix[i][0] = transition_matrix[i][0] + 0.13367697621205613
    
    transition_matrix[i][i] = 0.5003244838153312
    transition_matrix[0][0] = 1-0.2494728051203035

    if(i+1<=len_):
      transition_matrix[i][i+1] = 0.2494728051203035
    else:
      transition_matrix[len_][len_] += 0.2494728051203035

# # 初始狀態分佈 (x = 0)
initial_distribution = np.zeros(n_states)
initial_distribution[0] = 1

# 設定總步數
n_steps = 228877
distribution = initial_distribution
count_x_10 = 0

# 執行 228877 次步驟
for step in range(n_steps):
    # 更新分佈
    distribution = np.dot(distribution, transition_matrix)
    # 歸一化分佈
    distribution /= np.sum(distribution)
    # 累計 x = 10 的機率
    count_x_10 += distribution[len_]

# 輸出結果
print(f"在 {n_steps} 次變化中，x = {len_} 出現的次數: {count_x_10:.10f} loss : {count_x_10/228877*100: .10f} %")
