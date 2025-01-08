import matplotlib.pyplot as plt
import numpy as np
IMAGE_PATH = '../data/report.png'
# Data
x_values = np.array([1, 6, 11, 16, 21, 26, 31, 36, 41, 46, 51, 56, 61, 66, 71, 76, 81, 86, 91, 96])
y_values_1 = np.array([0.000000, 11.331659, 9.853969, 1.797631, 0.814911, 0.688099, 0.619820, 0.559667, 
                     0.511287, 0.473493, 0.445057, 0.410499, 0.384486, 0.361319, 0.337342, 0.314978, 
                     0.296302, 0.280038, 0.266219, 0.251997])
y_values_2 = np.array([0.000000, 11.194430, 5.106383, 0.120711, 0.033328, 0.026826, 0.021135, 0.019509, 
                     0.016664, 0.015038, 0.013819, 0.011787, 0.010162, 0.009349, 0.007723, 0.007723, 
                     0.006910, 0.005690, 0.005690, 0.004065])
y_values_3 = np.array([0.1 for i in y_values_2])
# Plot
plt.figure(figsize=(10, 6))
plt.plot(x_values, y_values_1, linestyle='-', color='b', label="Naughty")
plt.plot(x_values, y_values_2, linestyle='-', color='g', label="Regular")
plt.plot(x_values, y_values_3, linestyle='-', color='r', label="epsilon")
plt.title("Loss Percentage")
plt.xlabel("Max Queue Buffer")
plt.ylabel("Percentage (%)")
plt.grid(alpha=0.3)
plt.legend()
plt.tight_layout()

# Save plot as PNG
output_path = "./percentage_distribution.png"
# plt.savefig(output_path)
plt.savefig(IMAGE_PATH)
