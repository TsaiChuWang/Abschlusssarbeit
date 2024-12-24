import sys
import pandas
import shutil

terminal_width = shutil.get_terminal_size().columns


DATA_PATH = sys.argv[1]

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()
# print(dataframe)
# print(len(dataframe))

def calculateLoss_all(loss, index):
  return (float(loss[index])*100.0)/(sum([float(loss[i]) for i in range(4)]))

def calculateLoss_pure(loss, index):
  return (float(loss[index])*100.0)/(float(loss[0])+float(loss[2])+float(loss[3]))

print('=' * terminal_width)
print("Denominator with total packets")

gcra1_loss = [calculateLoss_all(tenant, 1) for tenant in data]
print("Max  Loss(GCRA1) : {:<12} %". format(max(gcra1_loss)))
print("Min  Loss(GCRA1) : {:<12} %". format(min(gcra1_loss)))
print("Avg. Loss(GCRA1) : {:<12.10} %". format(sum(gcra1_loss)/len(dataframe)))

print('-' * terminal_width)

gcra2_loss = [calculateLoss_all(tenant, 2) for tenant in data]
print("Max  Loss(GCRA2) : {:<12} %". format(max(gcra2_loss)))
print("Min  Loss(GCRA2) : {:<12} %". format(min(gcra2_loss)))
print("Avg. Loss(GCRA2) : {:<12.10} %". format(sum(gcra2_loss)/len(dataframe)))
print('-' * terminal_width)

link_loss = [calculateLoss_all(tenant, 3) for tenant in data]
print("Max  Loss(Link)  : {:<12} %". format(max(link_loss)))
print("Min  Loss(Link)  : {:<12} %". format(min(link_loss)))
print("Avg. Loss(Link)  : {:<12.10} %". format(sum(link_loss)/len(dataframe)))
print('-' * terminal_width)

total_loss = [calculateLoss_all(tenant, 1)+calculateLoss_all(tenant, 2)+calculateLoss_all(tenant, 3) for tenant in data]
print("Max  Loss(Total) : {:<12} %". format(max(total_loss)))
print("Min  Loss(Total) : {:<12} %". format(min(total_loss)))
print("Avg. Loss(Total) : {:<12.10} %". format(sum(total_loss)/len(dataframe)))

print('=' * terminal_width)
print("Denominator with packets in contract")

gcra2_loss = [calculateLoss_pure(tenant, 2) for tenant in data]
print("Max  Loss(GCRA2) : {:<12} %". format(max(gcra2_loss)))
print("Min  Loss(GCRA2) : {:<12} %". format(min(gcra2_loss)))
print("Avg. Loss(GCRA2) : {:<12.10} %". format(sum(gcra2_loss)/len(dataframe)))
print('-' * terminal_width)

link_loss = [calculateLoss_pure(tenant, 3) for tenant in data]
print("Max  Loss(Link)  : {:<12} %". format(max(link_loss)))
print("Min  Loss(Link)  : {:<12} %". format(min(link_loss)))
print("Avg. Loss(Link)  : {:<12.10} %". format(sum(link_loss)/len(dataframe)))
print('-' * terminal_width)

total_loss = [calculateLoss_pure(tenant, 1)+calculateLoss_pure(tenant, 2)+calculateLoss_pure(tenant, 3) for tenant in data]
print("Max  Loss(Total) : {:<12} %". format(max(total_loss)))
print("Min  Loss(Total) : {:<12} %". format(min(total_loss)))
print("Avg. Loss(Total) : {:<12.10} %". format(sum(total_loss)/len(dataframe)))
print('=' * terminal_width)