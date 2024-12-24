import pandas as pd
import sys
import matplotlib.pyplot as plt

CODE_LINECHART = 0

# python3 ../python/packet_generation.py 0 ../Datei/simple_V10_1/packet_generation 0

if(len(sys.argv) < 3):
    print('Missing arguments!')
    print('example : python3 ../python/packet_generation.py [CODE] [folder_path] [identifier]')
    exit()

CODE = int(sys.argv[1])

FOLDER_PATH = sys.argv[2]
IDENTIFIER = int(sys.argv[3])

def transpose_matrix(matrix):
    return [[matrix[j][i] for j in range(len(matrix))] for i in range(len(matrix[0]))]


if(CODE == CODE_LINECHART):
    dataframe = pd.read_csv(FOLDER_PATH+'/tenant_{}.csv'.format(IDENTIFIER))
    data = dataframe.values.tolist()
    data = transpose_matrix(data)
    
    plt.plot(data[0], data[1], linestyle='-', color='blue', label='tenant {}'.format(IDENTIFIER))

    plt.title('Packet Generation of Tenant {}'.format(IDENTIFIER))
    plt.xlabel('Timestamp(second)')
    plt.ylabel('Packets')
    plt.legend()
    plt.grid(True)

    IMAGE_PATH = FOLDER_PATH+'/images/tenant_{}.png'.format(IDENTIFIER)
    plt.savefig(IMAGE_PATH)
    plt.clf()