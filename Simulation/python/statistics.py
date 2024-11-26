import sys
import pandas

DATA_PATH = sys.argv[1]

dataframe = pandas.read_csv(DATA_PATH, header=None)
data = dataframe.values.tolist()
# print(dataframe)
# print(len(dataframe))
pure_loss = [float(tenant[4]) for tenant in data]

print("Max  Loss(Pure)  : {:<12} %". format(max(pure_loss)))
print("Min  Loss(Pure)  : {:<12} %". format(min(pure_loss)))
print("Avg. Loss(Pure)  : {:<12.10} %". format(sum(pure_loss)/len(dataframe)))

total_loss = [float(tenant[5]) for tenant in data]

print("Max  Loss(Total) : {:<12} %". format(max(total_loss)))
print("Min  Loss(Total) : {:<12} %". format(min(total_loss)))
print("Avg. Loss(Total) : {:<12.10} %". format(sum(total_loss)/len(dataframe)))