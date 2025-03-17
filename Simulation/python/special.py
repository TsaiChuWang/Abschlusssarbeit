import matplotlib.pyplot as plt
import pandas
import numpy as np
IMAGE_PATH = '../data/report.png'
DATA_PATH = '../data/naughty_1.csv'

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()

tau = []

# Naughty Number
# count = [0 for i in range(4)]

# regular_loss_naughty_number_20 = [0 for i in range(15872, 30209, 1024)]
# regular_loss_naughty_number_40 = [0 for i in range(15872, 30209, 1024)]
# regular_loss_naughty_number_60 = [0 for i in range(15872, 30209, 1024)]
# regular_loss_naughty_number_80 = [0 for i in range(15872, 30209, 1024)]

# naughty_loss_naughty_number_20 = [0 for i in range(15872, 30209, 1024)]
# naughty_loss_naughty_number_40 = [0 for i in range(15872, 30209, 1024)]
# naughty_loss_naughty_number_60 = [0 for i in range(15872, 30209, 1024)]
# naughty_loss_naughty_number_80 = [0 for i in range(15872, 30209, 1024)]

# for i in range(15872, 30209, 1024):
#   tau.append(i)
#   for row in data:
#     if(int(row[1]) == 140 and int(row[2])== 100):
#       index = int((int(row[0])-15872)/1024)

#       if(int(row[3]) == 20):
#         regular_loss_naughty_number_20[index] += float(row[4])
#         naughty_loss_naughty_number_20[index] += float(row[5])
#         count[0] += 1

#       if(int(row[3]) == 40):
#         regular_loss_naughty_number_40[index] += float(row[4])
#         naughty_loss_naughty_number_40[index] += float(row[5])
#         count[1] += 1
      
#       if(int(row[3]) == 60):
#         regular_loss_naughty_number_60[index] += float(row[4])
#         naughty_loss_naughty_number_60[index] += float(row[5])
#         count[2] += 1

#       if(int(row[3]) == 80):
#         regular_loss_naughty_number_80[index] += float(row[4])
#         naughty_loss_naughty_number_80[index] += float(row[5])
#         count[3] += 1

# regular_loss_naughty_number_20 = [i/105 for i in regular_loss_naughty_number_20]
# regular_loss_naughty_number_40 = [i/105 for i in regular_loss_naughty_number_40]
# regular_loss_naughty_number_60 = [i/105 for i in regular_loss_naughty_number_60]
# regular_loss_naughty_number_80 = [i/90 for i in regular_loss_naughty_number_80]

# naughty_loss_naughty_number_20 = [i/105 for i in naughty_loss_naughty_number_20]
# naughty_loss_naughty_number_40 = [i/105 for i in naughty_loss_naughty_number_40]
# naughty_loss_naughty_number_60 = [i/105 for i in naughty_loss_naughty_number_60]
# naughty_loss_naughty_number_80 = [i/90 for i in naughty_loss_naughty_number_80]

# Regualr 
# plt.plot(tau, regular_loss_naughty_number_20, linestyle='-', label='Naughty Number(20)')
# plt.plot(tau, regular_loss_naughty_number_40, linestyle='-', label='Naughty Number(40)')
# plt.plot(tau, regular_loss_naughty_number_60, linestyle='-', label='Naughty Number(60)')
# plt.plot(tau, regular_loss_naughty_number_80, linestyle='-', label='Naughty Number(80)')
# plt.title('Regular Loss with different naughty mean and tau')

# Naughty

# plt.plot(tau, naughty_loss_naughty_number_20, linestyle='-', label='Naughty Number(20)')
# plt.plot(tau, naughty_loss_naughty_number_40, linestyle='-', label='Naughty Number(40)')
# plt.plot(tau, naughty_loss_naughty_number_60, linestyle='-', label='Naughty Number(60)')
# plt.plot(tau, naughty_loss_naughty_number_80, linestyle='-', label='Naughty Number(80)')
# plt.title('Naughty Loss with different naughty mean and tau')

# Buffer Size

regular_loss_buffer_100 = [0 for i in range(15872, 30209, 1024)]
regular_loss_buffer_200 = [0 for i in range(15872, 30209, 1024)]
regular_loss_buffer_150 = [0 for i in range(15872, 30209, 1024)]
regular_loss_buffer_250 = [0 for i in range(15872, 30209, 1024)]

naughty_loss_buffer_100 = [0 for i in range(15872, 30209, 1024)]
naughty_loss_buffer_200 = [0 for i in range(15872, 30209, 1024)]
naughty_loss_buffer_150 = [0 for i in range(15872, 30209, 1024)]
naughty_loss_buffer_250 = [0 for i in range(15872, 30209, 1024)]


# for i in range(15872, 30209, 1024):
#   tau.append(i)
#   for row in data:
#     if(int(row[1]) == 140 and int(row[3])== 40):
#       index = int((int(row[0])-15872)/1024)
#       if(int(row[2]) == 100):
#         regular_loss_buffer_100[index] += float(row[4])
#         naughty_loss_buffer_100[index] += float(row[5])

#       if(int(row[2]) == 200):
#         regular_loss_buffer_200[index] += float(row[4])
#         naughty_loss_buffer_200[index] += float(row[5])
      
#       if(int(row[2]) == 150):
#         regular_loss_buffer_150[index] += float(row[4])
#         naughty_loss_buffer_150[index] += float(row[5])

#       if(int(row[2]) == 250):
#         regular_loss_buffer_250[index] += float(row[4])
#         naughty_loss_buffer_250[index] += float(row[5])

regular_loss_buffer_100 = [i/105 for i in regular_loss_buffer_100]
regular_loss_buffer_150 = [i/105 for i in regular_loss_buffer_150]
regular_loss_buffer_200 = [i/105 for i in regular_loss_buffer_200]
regular_loss_buffer_250 = [i/105 for i in regular_loss_buffer_250]

naughty_loss_buffer_100 = [i/105 for i in naughty_loss_buffer_100]
naughty_loss_buffer_200 = [i/105 for i in naughty_loss_buffer_200]
naughty_loss_buffer_150 = [i/105 for i in naughty_loss_buffer_150]
naughty_loss_buffer_250 = [i/105 for i in naughty_loss_buffer_250]

# Regular
# plt.plot(tau, regular_loss_buffer_100 , linestyle='-', label='Buffer(100)')
# plt.plot(tau, regular_loss_buffer_150 , linestyle='-', label='Buffer(150)')
# plt.plot(tau, regular_loss_buffer_200 , linestyle='-', label='Buffer(200)')
# plt.plot(tau, regular_loss_buffer_250 , linestyle='-', label='Buffer(250)')
# plt.title('Regular Loss with different buffer size and tau')

# Naughty
# plt.plot(tau, naughty_loss_buffer_100 , linestyle='-', label='Buffer(100)')
# plt.plot(tau, naughty_loss_buffer_150 , linestyle='-', label='Buffer(150)')
# plt.plot(tau, naughty_loss_buffer_200 , linestyle='-', label='Buffer(200)')
# plt.plot(tau, naughty_loss_buffer_250 , linestyle='-', label='Buffer(250)')
# plt.title('Naughty Loss with different buffer size and tau')

# Naughty Mean
regular_loss_130 = [0 for i in range(15872, 30209, 1024)]
regular_loss_140 = [0 for i in range(15872, 30209, 1024)]
regular_loss_150 = [0 for i in range(15872, 30209, 1024)]

naughty_loss_130 = [0 for i in range(15872, 30209, 1024)]
naughty_loss_140 = [0 for i in range(15872, 30209, 1024)]
naughty_loss_150 = [0 for i in range(15872, 30209, 1024)]

for i in range(15872, 30209, 1024):
  tau.append(i)
  for row in data:
    if(int(row[2]) == 100 and int(row[3])== 40):
      index = int((int(row[0])-15872)/1024)
      if(int(row[1]) == 130):
        regular_loss_130[index] += float(row[4])
        naughty_loss_130[index] += float(row[5])

      if(int(row[1]) == 140):
        regular_loss_140[index] += float(row[4])
        naughty_loss_140[index] += float(row[5])
      
      if(int(row[1]) == 150):
        regular_loss_150[index] += float(row[4])
        naughty_loss_150[index] += float(row[5])

regular_loss_130 = [i/105 for i in regular_loss_130]
regular_loss_140 = [i/105 for i in regular_loss_140]
regular_loss_150 = [i/105 for i in regular_loss_150]

naughty_loss_130 = [i/105 for i in naughty_loss_130]
naughty_loss_140 = [i/105 for i in naughty_loss_140]
naughty_loss_150 = [i/105 for i in naughty_loss_150]


# Regular 
# plt.plot(tau, regular_loss_130, linestyle='-', label='Regular(130)')
# plt.plot(tau, regular_loss_140, linestyle='-', label='Regular(140)')
# plt.plot(tau, regular_loss_150, linestyle='-', label='Regular(150)')
# plt.title('Regular Loss with different naughty number and tau')

# Naughty

plt.plot(tau, naughty_loss_130, linestyle='-', label='Naughty(130)')
plt.plot(tau, naughty_loss_140, linestyle='-', label='Naughty(140)')
plt.plot(tau, naughty_loss_150, linestyle='-', label='Naughty(150)')
plt.title('Naughty Loss with different naughty number and tau')

plt.plot(tau, [0.1 for i in tau], linestyle='-', color = 'red', label='epsilon')


plt.xlabel('tau')
plt.ylabel('Loss')
plt.legend()
plt.grid(True)

plt.savefig(IMAGE_PATH)
plt.cla()

