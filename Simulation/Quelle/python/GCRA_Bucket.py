import matplotlib.pyplot as plt
import pandas as pd
import os
from PIL import Image
import sys

DATA_PATH ="../../Datei/token_bucket/token_bucket.csv"
IMAGE_PATH = "../../Datei/GCRA/images/GCRA_bucket{}.png"
IMAGE_FOLDER_PATH = '../../Datei/GCRA/images'
GIF_PATH = '../../Datei/GCRA/GCRA.gif'

time_interval = 99

# Data
categories = ['Tenant A Bucket', 'Tenant B Bucket', 'Tenant C Bucket']
dataframe = pd.read_csv(DATA_PATH)
data = dataframe.values.tolist()

index = 0
def drawATimestampPicture(index):
    values = data[index][:-1]
    # Define colors for each bar
    colors = ['#7ED957', '#0CC0DF', '#FF5757']

    # Create bar chart
    plt.bar(categories, values, color=colors)

    # Add labels and title
    plt.xlabel('Tenant')
    plt.ylabel('Bucket Capacity')
    plt.title('Bucket Capacity with DifferentTenants')

    # Show plot
    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()

def drawATimestampPictureWithTitle(index, bucket_size, leakage_rate):
    values = data[index][:-1]
    # Define colors for each bar
    colors = ['#7ED957', '#0CC0DF', '#FF5757']

    # Create bar chart
    plt.bar(categories, values, color=colors)

    # Add labels and title
    plt.xlabel('Tenant')
    plt.ylabel('Bucket Capacity')
    plt.title('Bucket : '+str(bucket_size)+" Leakage : "+str(leakage_rate))

    # Show plot
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

if(len(sys.argv)>1):
    for index in range(time_interval):
        drawATimestampPictureWithTitle(index, sys.argv[1], sys.argv[2])
else:
    for index in range(time_interval):
        drawATimestampPicture(index)
generateTheGIF(IMAGE_FOLDER_PATH, GIF_PATH)
