    
import matplotlib.pyplot as plt

IMAGE_PATH = "../data/report.png"

x_axis = [3087500, 100, 385488, 192794, 96447, 48273, 24187]
y_axis = [98348431, 99892091]     

plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='Dropped Packets')
plt.plot(x_axis, [160 for row in x_axis], linestyle='dotted', color='red', label='Expected Dropped Packets')

plt.title('Find Tau of the First GCRA')
plt.xlabel('Tau')
plt.ylabel('Number of Dropped Packets')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()