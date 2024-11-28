    
import matplotlib.pyplot as plt

IMAGE_PATH = "../data/report.png"

x_axis = [   0,
  512,
  1024,
  2048,
  3072,
  3584,
  4096,
  5120,
  8128,
  16256]
y_axis = [     27.19924804 ,
6.814139187 ,
 3.49078175 ,
 3.104870119,
 3.100598698 ,
 3.101108447 ,
 3.103707451 ,
 3.10645731 ,
 3.104225066 ,
3.09837465 ,



  ]     
print(len(x_axis))
print(len(y_axis))
plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='Total Loss')
# plt.plot(x_axis, [0.1 for row in x_axis], linestyle='dotted', color='red', label='Epsilon')

plt.title('Different τ1 with Packet Total Loss')
plt.xlabel('τ1')
plt.ylabel('Total Loss')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()