import matplotlib.pyplot as plt
import pandas
import sys

DATA_PATH = './record_average_loss.csv'
dataframe = pandas.read_csv(DATA_PATH)

state_r = float(sys.argv[1])
# queue_buffer = list(set(dataframe['upper_queue_buffer'].tolist()))
queue_buffer = [21+i for i in range(10)]

tau_ = dataframe['tau'].tolist()
tau_ = list(set(tau_))
tau_.sort()

# cd /home/wtsaichu/Dokumente/Arbeitsplatz/Abschlusssarbeit/Simulation/data/burst_all_compliant_different_FIFO
# python3 figure.py 0.6

# pure
for key in queue_buffer:
    average_loss_pure = dataframe[(dataframe['state_r'] ==state_r) & (dataframe['upper_queue_buffer'] == key)][['average_loss_pure']]
    # print(key)
    # print(average_loss_pure)
    if(len(average_loss_pure)!=0):
        tau = dataframe[(dataframe['state_r'] ==state_r) & (dataframe['upper_queue_buffer'] == key)]['tau'].tolist()
        plt.plot(tau, average_loss_pure, linestyle='-', label='(pure)r = {}, buffer = {}'.format(state_r, key), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss with different τ and r ({})'.format(state_r), fontsize=10)
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig('./images/pure_{}.png'.format(state_r*10))
plt.cla()

# all
for key in queue_buffer:
    average_loss_all = dataframe[(dataframe['state_r'] ==state_r) & (dataframe['upper_queue_buffer'] == key)][['average_loss_all']]
    if(len(average_loss_all)!=0):
        tau = dataframe[(dataframe['state_r'] ==state_r) & (dataframe['upper_queue_buffer'] == key)]['tau'].tolist()
        plt.plot(tau, average_loss_all, linestyle='-', label='(all)r = {}, buffer = {}'.format(state_r, key), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss with different τ and r ({})'.format(state_r), fontsize=10)
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig('./images/all_{}.png'.format(state_r*10))
plt.cla()
