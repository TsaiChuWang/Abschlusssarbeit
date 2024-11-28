    
import matplotlib.pyplot as plt

IMAGE_PATH = "../data/report.png"

x_axis = [ 7680,
  7424,
  7552,
  7616,
  7648,
  7488, 
  7520,
  7504,
  7512,
  7516,
  7514,
  7513
  ]
y_axis = [ 0.08798286,
  0.10296441,
  0.09602841,
  0.09181585,
  0.09001778,
  0.10027699,
  0.0992252,
  0.10049743,
  0.1001163,
  0.09982913,
  0.09958553,
  0.10014665,
  ]     
print(len(x_axis))
print(len(y_axis))
plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='Packet Loss(Pure)')
plt.plot(x_axis, [0.1 for row in x_axis], linestyle='dotted', color='red', label='Epsilon')

plt.title('Different τ2 with Packet Losss(Pure)')
plt.xlabel('τ2')
plt.ylabel('Packet Loss(Pure)')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()