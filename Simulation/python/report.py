import matplotlib.pyplot as plt

IMAGE_PATH = '../data/report.png'
# Sample data (only a portion for brevity, you can extend this to the full data set)
timestamps = [i*3051 for i in range(0,327681)]
packets = []
for i in range(327681):
    step = int(32768/64)
    # if(i%step<step*0.75):
    #     packets.append(1)
    # else:
    #     packets.append(0)

    if(i%step<step*0.25):
        packets.append(0)
    else:
        packets.append(1) 
plt.plot(timestamps, packets, linestyle='-', color='blue', label='Loss(Pure)')

plt.title('8')
plt.xlabel('timestamps')
plt.ylabel('Packets')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()

