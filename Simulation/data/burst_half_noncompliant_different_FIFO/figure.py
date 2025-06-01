import matplotlib.pyplot as plt
import pandas
import sys

DATA_PATH = './compliant_and_noncompliant_all.csv'
dataframe = pandas.read_csv(DATA_PATH)

state_r = float(sys.argv[1])
queue_buffer = list(set(dataframe['upper_queue_buffer'].tolist()))
# queue_buffer = [20,40,60,80]

# cd /home/wtsaichu/Dokumente/Arbeitsplatz/Abschlusssarbeit/Simulation/data/burst_half_noncompliant_different_FIFO
# python3 figure.py 0.6

tau_ = dataframe['tau'].tolist()
tau_ = list(set(tau_))
tau_.sort()

# pure
for buffer in queue_buffer:
    noncompliant_loss_pure = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)][['noncompliant_loss_pure']]
    if(len(noncompliant_loss_pure)!=0):
        tau = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
        plt.plot(tau, noncompliant_loss_pure, linestyle='-', label='(pure)r = {}, buffer = {}'.format(state_r, buffer), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss with different τ and buffer ({}) Non-compliant'.format(state_r), fontsize=10)
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig('./images/pure_noncompliant.png')
plt.cla()

for buffer in queue_buffer:
    compliant_loss_pure = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)][['compliant_loss_pure']]
    if(len(compliant_loss_pure)!=0):
        tau = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
        plt.plot(tau, compliant_loss_pure, linestyle='-', label='(pure)r = {}, buffer = {}'.format(state_r, buffer), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss with different τ and buffer ({}) Compliant'.format(state_r), fontsize=10)
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig('./images/pure_compliant.png')
plt.cla()

# all
for buffer in queue_buffer:
    noncompliant_loss_all = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)][['noncompliant_loss_all']]
    if(len(noncompliant_loss_all)!=0):
        tau = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
        plt.plot(tau, noncompliant_loss_all, linestyle='-', label='(all)r = {}, buffer = {}'.format(state_r, buffer), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss with different τ and buffer ({}) Non-compliant'.format(state_r), fontsize=10)
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig('./images/all_noncompliant.png')
plt.cla()

for buffer in queue_buffer:
    compliant_loss_all = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)][['compliant_loss_all']]
    if(len(compliant_loss_all)!=0):
        tau = dataframe[(dataframe['state_r'] == state_r) & (dataframe['upper_queue_buffer'] == buffer)]['tau'].tolist()
        plt.plot(tau, compliant_loss_all, linestyle='-', label='(all)r = {}, buffer = {}'.format(state_r, buffer), alpha = 1)
plt.plot(tau_, [0.1 for i in tau_], linestyle='-', color = 'red', label='ε')
plt.title('Packet Loss with different τ and buffer ({}) Compliant'.format(state_r), fontsize=10)
plt.ylabel('Loss (%)')
plt.xlabel('τ')
plt.legend()
plt.grid(True)
plt.savefig('./images/all_compliant.png')
plt.cla()