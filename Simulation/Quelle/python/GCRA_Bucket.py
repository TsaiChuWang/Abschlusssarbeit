import matplotlib.pyplot as plt
import pandas as pd
import os
from PIL import Image
import sys

DATA_PATH = "../../Datei/token_bucket/token_bucket.csv"
IMAGE_PATH = "../../Datei/GCRA/images/GCRA_bucket{}.png"
IMAGE_FOLDER_PATH = '../../Datei/GCRA/images'
GIF_PATH = '../../Datei/GCRA/GCRA.gif'

time_interval = 99

# Data
categories = ['Tenant A Bucket', 'Tenant B Bucket', 'Tenant C Bucket']
dataframe = pd.read_csv(DATA_PATH)
data = dataframe.values.tolist()

def draw_a_timestamp_picture(index):
    """
    @brief Draws a bar chart for bucket capacities at a specific timestamp.

    This function creates and saves a bar chart representing the bucket capacities
    of different tenants at the specified index (timestamp).

    @param index The index (timestamp) for which to draw the chart.
    """
    values = data[index][:-1]
    # Define colors for each bar
    colors = ['#7ED957', '#0CC0DF', '#FF5757']

    # Create bar chart
    plt.bar(categories, values, color=colors)

    # Add labels and title
    plt.xlabel('Tenant')
    plt.ylabel('Bucket Capacity')
    plt.title('Bucket Capacity with Different Tenants')

    # Save plot
    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()

def draw_a_timestamp_picture_with_title(index, bucket_size, leakage_rate):
    """
    @brief Draws a bar chart with a title for bucket capacities at a specific timestamp.

    This function creates and saves a bar chart with a title that includes
    bucket size and leakage rate for the specified index (timestamp).

    @param index The index (timestamp) for which to draw the chart.
    @param bucket_size The size of the bucket to include in the title.
    @param leakage_rate The leakage rate to include in the title.
    """
    values = data[index][:-1]
    # Define colors for each bar
    colors = ['#7ED957', '#0CC0DF', '#FF5757']

    # Create bar chart
    plt.bar(categories, values, color=colors)

    # Add labels and title
    plt.xlabel('Tenant')
    plt.ylabel('Bucket Capacity')
    plt.title(f'Bucket: {bucket_size} Leakage: {leakage_rate}')

    # Save plot
    plt.savefig(IMAGE_PATH.format(index))
    plt.cla()

def generate_the_gif(images_folder_path, gif_path):
    """
    @brief Generates a GIF from PNG images in a specified folder.

    This function collects all PNG files from the specified folder and creates
    a GIF file from them.

    @param images_folder_path The path to the folder containing PNG images.
    @param gif_path The path where the GIF will be saved.
    """
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

if len(sys.argv) > 1:
    for index in range(time_interval):
        draw_a_timestamp_picture_with_title(index, sys.argv[1], sys.argv[2])
else:
    for index in range(time_interval):
        draw_a_timestamp_picture(index)

generate_the_gif(IMAGE_FOLDER_PATH, GIF_PATH)
