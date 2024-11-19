    
import matplotlib.pyplot as plt

IMAGE_PATH = "../data/report.png"

x_axis = [43945310000, 87890620000, 65917965000, 76904292500, 71411128750, 74157710625, 75530986562, 76217639500, 75874313000, 75702649800, 75616818200, 75573902400, 75595360300, 75584631400, 75589995900, 75587313700, 75585972550, 75586643125, 75586307837, 75585301975, 75584966687, 75584799043, 75584882865]
y_axis = [20.943015  , 0          ,    6.399496,           0,    2.763369,    0.945448,    0.036659,    0.000000,    0.000000,    0.000000,    0.000000,    0.008265,    0.000000,    0.001166,    0.000000,    0.000000,    0.000279,    0.000000,    0.000057,    0.000723,    0.000945,    0.001055,    0.001000]     
print(len(x_axis))
print(len(y_axis))
plt.plot(x_axis, y_axis, linestyle='-', color='blue', label='Packet Loss')
plt.plot(x_axis, [0.001 for row in x_axis], linestyle='dotted', color='red', label='Epsilon')

plt.title('Find Tau of the Second GCRA')
plt.xlabel('Tau')
plt.ylabel('Packet Loss')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()