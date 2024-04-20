import pandas as pd
import os
import sys


def merge_csv_files(folder_path):
    # List to hold data from each file
    all_data = []

    # Column names based on your description
    columns = ['Docker image name', 'Overall runtime', 'C3', 'C4', 'C5']

    # Iterate over each file in the specified folder
    for filename in os.listdir(folder_path):
        if filename.endswith('.csv'):
            file_path = os.path.join(folder_path, filename)
            # Read the CSV file without headers
            df = pd.read_csv(file_path, header=None, names=columns)

            # Calculate the average of columns C3, C4, C5
            df['Average'] = df[['C3', 'C4', 'C5']].mean(axis=1)

            # Select the Docker image name and the average column
            df = df[['Docker image name', 'Average']]
            df.columns = ['Docker image name', filename[:-4]]  # Rename columns to use the filename as a new column name

            # Append to the list
            all_data.append(df)

    # Merge all dataframes on Docker image name
    merged_df = all_data[0]
    for df in all_data[1:]:
        merged_df = pd.merge(merged_df, df, on='Docker image name', how='outer')

    # Save the merged DataFrame to a new CSV file
    merged_df.to_csv('merged_output.csv', index=False)
    return 'Merge Complete'


# Example usage
folder_path = sys.argv[1]
print(merge_csv_files(folder_path))
