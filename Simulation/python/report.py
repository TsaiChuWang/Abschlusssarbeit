import matplotlib.pyplot as plt

# Sample data (only a portion for brevity, you can extend this to the full data set)
timestamps = [246000, 247000, 248000, 249000, 250000]
regular_GCRA = [83.96, 82.17, 80.44, 79.16, 76.61]
naughty_GCRA = [20653.32, 20651.28, 20651.44, 20646.48, 20642.92]
regular_loss = [0.034503, 0.033760, 0.033196, 0.032561, 0.031704]
naughty_loss = [7.767044, 7.765097, 7.768257, 7.763841, 7.764841]

# Create figure and axes for plotting
fig, axs = plt.subplots(2, 1, figsize=(10, 8))

# Plot GCRA values
axs[0].plot(timestamps, regular_GCRA, label="Regular GCRA", color='b', marker='o')
axs[0].plot(timestamps, naughty_GCRA, label="Naughty GCRA", color='r', marker='x')
axs[0].set_title('GCRA Values Over Time')
axs[0].set_xlabel('Timestamps')
axs[0].set_ylabel('GCRA')
axs[0].legend()
axs[0].grid(True)

# Plot Loss percentages
axs[1].plot(timestamps, regular_loss, label="Regular Loss", color='b', marker='o')
axs[1].plot(timestamps, naughty_loss, label="Naughty Loss", color='r', marker='x')
axs[1].set_title('Loss Percentages Over Time')
axs[1].set_xlabel('Timestamps')
axs[1].set_ylabel('Loss (%)')
axs[1].legend()
axs[1].grid(True)

# Show the plot
plt.tight_layout()
plt.show()
