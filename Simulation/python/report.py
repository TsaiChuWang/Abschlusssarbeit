import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Ellipse

# 設定時間範圍
T = np.linspace(0, 10, 1000)

# 模擬頻寬使用率
np.random.seed(0)
tx_rate = np.random.normal(loc=120, scale=20, size=T.shape)

# 在中間加入突發流量
burst_start = int(len(T) * 0.4)
burst_end = int(len(T) * 0.5)
T_ = np.linspace(0, burst_start-burst_start, 100)
tx_rate[burst_start:burst_end] = np.random.normal(loc=160, scale=20, size=T_.shape)  # 增加突發流量

# 設定頻寬限制
upper_limit_bandwidth = 160
guaranteed_bandwidth = 80
r = (upper_limit_bandwidth + guaranteed_bandwidth) / 2

# 繪製圖表
plt.figure(figsize=(8, 6))
plt.plot(T, tx_rate, 'k-', label='Tx rate')
plt.axhline(y=upper_limit_bandwidth, color='r', linestyle='-', label='Upper bound bandwidth: μ+σ')
plt.axhline(y=guaranteed_bandwidth, color='g', linestyle='-', label='Lower bound bandwdith : μ-σ')
plt.axhline(y=r, color='b', linestyle='--', label='long term mean μ(Soft Guaranteed)')

# 標記陰影區域
# plt.fill_betweenx([60, 100], T[burst_start], T[burst_end], color='gray', alpha=0.5)
ellipse = Ellipse(xy=(4.5, 180), width=1.4, height=40, angle=0, color='gray', alpha=0.5)
plt.gca().add_patch(ellipse)
plt.text((T[burst_start] + T[burst_end]) / 2+2.5, 185, 'Drop Packets unconditionally', fontsize=9, verticalalignment='center', horizontalalignment='center', color='black')

# 標題和標籤
plt.title('Bandwidth Guaranteed Service')
plt.xlabel('Time (T)')
plt.ylabel('Bandwidth Usage')
plt.legend(loc='upper left')
plt.grid(True)

# 顯示圖表
# plt.show()
plt.savefig('../data/images/BandwidthGuaranteedService.png')
