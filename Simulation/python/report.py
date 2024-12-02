    
import matplotlib.pyplot as plt

IMAGE_PATH = "../data/report.png"

x_axis = [   
  2048,
  3072,
  3584,
  4096,
  5120,
  8128,
  16256]
y_axis = [    
   0.02926757 ,
0.00174733 ,

 0.00043685 ,
 0 ,
 0 ,
0 ,
0 ,




  ]     
print(len(x_axis))
print(len(y_axis))
plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='GCRA 1 Loss')
plt.plot(x_axis, [0.1 for row in x_axis], linestyle='dotted', color='red', label='Epsilon')

plt.title('Different τ1 with Packet GCRA 1 Loss')
plt.xlabel('τ1')
plt.ylabel('GCRA 1 Loss')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()