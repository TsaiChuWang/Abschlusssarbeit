import configparser
import matplotlib.pyplot as plt
import pandas
import numpy as np
import sys

x=[1+i*0.001 for i in range(11)]
y = [17920,16384,14848,13824,12288,10752,9728,8704,7168,6144,5120]
y_ = [-1280000*i+1297920-200 for i in x]
plt.plot(x, y, linestyle='-', label='real data')
plt.plot(x, y_, linestyle='-', label='equation data')
# plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
plt.ylabel('τ (bits)')
plt.xlabel('Ratio')

plt.legend()
plt.grid(True)
plt.savefig('../data/uniform_half_noncompliant_ratio/images/a.png')

# x=[1+i*0.001 for i in range(11)]
# y = [51200,49512,48128,44032,41984,38912,36864,33792,30720,28672,26624]
# y_ = [-2457600*i+2508800+200 for i in x]
# plt.plot(x, y, linestyle='-', label='real data')
# plt.plot(x, y_, linestyle='-', label='equation data')
# # plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
# plt.ylabel('τ')
# plt.xlabel('Ratio')

# plt.legend()
# plt.grid(True)
# plt.savefig('../data/burst_half_noncompliant_ratio/images/b.png')


# x=[1+i*0.001 for i in range(11)]
# y = [51200,49152,48128,44032,41984,38912,36864,33792,30720,28672,26624]
# y_ = [-2457600*i+2508800+200 for i in x]
# plt.plot(x, y, linestyle='-', label='real data')
# plt.plot(x, y_, linestyle='-', label='equation data')
# # plt.title('Packet Loss with different τ and ratio ({}) Non-compliant'.format(name), fontsize=12)
# plt.ylabel('τ')
# plt.xlabel('Ratio')

# plt.legend()
# plt.grid(True)
# plt.savefig('../data/burst_half_noncompliant_ratio/images/b.png')