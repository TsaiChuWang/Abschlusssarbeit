# importing networkx 
import networkx as nx
# importing matplotlib.pyplot
import matplotlib.pyplot as plt
import pandas as pd
from PIL import Image
import os

DATA_PATH ="../../Datei/module_test/module_test.csv"
IMAGE_PATH = "../../Datei/module_test/image/module_test{}.png"
IMAGE_FOLDER_PATH = '../../Datei/module_test/image'
GIF_PATH = '../../Datei/module_test/module_test.gif'

dataframe = pd.read_csv(DATA_PATH)
# Convert the DataFrame to a 2D array (list of lists)
data = dataframe.values.tolist()

graph = nx.Graph()
mean = 120
standard_derivation = 40
link_capacity = 360
time_interval = 99

def label_color(value, mean, standard_derivation):
    if(float(value)<=mean-standard_derivation):
        return 'green'
    elif(float(value)>mean+standard_derivation):
        return 'red'
    return 'blue'

def label_link_traffic(value, link_capacity):
    if(value>=link_capacity):
        return 'red'
    else:
        return 'black'
# index=0

def drawATimestampPicture(index):
    node_colors = ['#7ED957', '#0CC0DF', '#FF5757', 'skyblue', 'skyblue']
    graph.add_nodes_from(['A', 'B', 'C', 'source', 'dest.'])
    graph.add_edge('A', 'source',  weight=data[index][0])
    graph.add_edge('B', 'source',  weight=data[index][1])
    graph.add_edge('C', 'source',  weight=data[index][2])
    graph.add_edge('source', 'dest.',  weight=data[index][3])
    position = {
        'A': (-1, 1),  
        'B': (-1, 0),  
        'C': (-1, -1),  
        'source': (0, 0),
        'dest.': (1, 0)   
    }
    nx.draw(graph, position, with_labels=True, node_color=node_colors, node_size=1000, font_size=10)

    edge_label_colors = {
        ('A', 'source'): label_color(data[index][0], mean, standard_derivation), 
        ('B', 'source'): label_color(data[index][1], mean, standard_derivation), 
        ('C', 'source'): label_color(data[index][2], mean, standard_derivation), 
        ('source', 'dest.'): label_link_traffic(data[index][3], link_capacity)}

    edge_labels = nx.get_edge_attributes(graph, 'weight')
    nx.draw_networkx_edge_labels(graph, position, edge_labels=edge_labels, font_color='red')
    for edge, label in edge_labels.items():
        # Draw the label for each edge individually
        nx.draw_networkx_edge_labels(
            graph,
            position,
            edge_labels={(edge[0], edge[1]): label},
            font_color=edge_label_colors[edge]
        )
    plt.text(0.95, 0.05, '{:<16s} : {:<12d}'.format('Timestamp', index), 
            horizontalalignment='right', 
            verticalalignment='bottom', 
            transform=plt.gcf().transFigure)
    plt.text(0.95, 0.1, '{:<16s} : {:<12.7f}'.format('Link-utilization', data[index][4]), 
            horizontalalignment='right', 
            verticalalignment='bottom', 
            transform=plt.gcf().transFigure)
    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()

def generateTheGIF( images_folder_path, gif_path):
    # Get all PNG files from the folder
    png_files = [f for f in os.listdir(images_folder_path) if f.endswith('.png')]
    png_files.sort()  # Sort the files if you want them in a specific order

    # Create a list to hold the images
    images = []
    for file in png_files:
        file_path = os.path.join(images_folder_path, file)
        img = Image.open(file_path)
        images.append(img)
    
    if images:
        images[0].save(gif_path, save_all=True, append_images=images[1:], duration=500, loop=0)
        print(f"GIF saved as {gif_path}")
    else:
        print("No PNG files found in the folder.")

def line_chart_tenants():
    plt.plot([index for index in range(len(data))], [float(row[0]) for row in data], linestyle='-', color='g', label='A')
    plt.plot([index for index in range(len(data))], [float(row[1]) for row in data], linestyle='-', color='b', label='B')
    plt.plot([index for index in range(len(data))], [float(row[2]) for row in data], linestyle='-', color='r', label='C')

    plt.title('Different tenants')
    plt.xlabel('X Axis')
    plt.ylabel('Y Axis')
    plt.legend()
    plt.grid(True)

    plt.savefig('../../Datei/module_test/tenants_linechart.png')
    plt.cla()

def line_chart_tenant(index):
    if(index==0):
        label = 'A'
        color = 'g'
    elif(index==1):
        label ='B'
        color = 'b'
    else:
        label = 'C'
        color = 'r'

    plt.plot([index for index in range(len(data))], [float(row[index]) for row in data], linestyle='-', color=color, label=label)

    plt.title('Tenant Traffic :'+label)
    plt.xlabel('X Axis')
    plt.ylabel('Y Axis')
    plt.legend()
    plt.grid(True)

    plt.savefig('../../Datei/module_test/tenants_linechart_'+label+'.png')
    plt.cla()

def line_chart_traffic():
    plt.plot([index for index in range(len(data))], [float(row[3]) for row in data], linestyle='-', color='black', label='traffic')
    plt.plot([index for index in range(len(data))], [link_capacity for row in data], linestyle='-', color='r', label='link capacity')

    plt.title('Link traffic')
    plt.xlabel('X Axis')
    plt.ylabel('Y Axis')
    plt.legend()
    plt.grid(True)

    plt.savefig('../../Datei/module_test/link_traffic_linechart.png')
    plt.cla()

def line_chart_utilization():
    plt.plot([index for index in range(len(data))], [float(row[4]) for row in data], linestyle='-', color='black', label='traffic')
    plt.plot([index for index in range(len(data))], [1 for row in data], linestyle='-', color='r', label='link capacity')

    plt.title('Link traffic')
    plt.xlabel('X Axis')
    plt.ylabel('Y Axis')
    plt.legend()
    plt.grid(True)

    plt.savefig('../../Datei/module_test/link_traffic_utilization.png')
    plt.cla()

# for index in range(time_interval):
#     drawATimestampPicture(index)
# generateTheGIF(IMAGE_FOLDER_PATH, GIF_PATH)

# line_chart_tenants()
# line_chart_tenant(0)
# line_chart_tenant(1)
# line_chart_tenant(2)

line_chart_traffic()
line_chart_utilization()