import pandas as pd
import os
import sys


def merge_csv_files(folder_path: str):
    all_data = []

    columns = ['Docker image name', 'Overall runtime', 'C3', 'C4', 'C5']

    for filename in os.listdir(folder_path):
        if filename.endswith('.csv'):
            file_path = os.path.join(folder_path, filename)

            df = pd.read_csv(file_path, header=None, names=columns)

            df['Average'] = df[['C3', 'C4', 'C5']].mean(axis=1)
            df = df[['Docker image name', 'Average']]
            df.columns = ['Docker image name', filename[:-4]]

            all_data.append(df)

    merged_df = all_data[0]
    for df in all_data[1:]:
        merged_df = pd.merge(merged_df, df, on='Docker image name', how='outer')

    merged_df.to_csv('merged_output.csv', index=False)
    return 'Merge Complete'


if __name__ == '__main__':
    folder_path = sys.argv[1]
    print(merge_csv_files(folder_path))
