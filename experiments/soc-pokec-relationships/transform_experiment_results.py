import pandas as pd
import argparse
import os

# Function to read, process, and pivot the data
def process_csv(input_filename):
    # Read the CSV data into a pandas DataFrame
    df = pd.read_csv(input_filename)

    values_column = df.columns[-1]

    # Pivot the data to get a 2D table
    pivot_table = df.pivot(index='Server Param', columns='Query Param', values=values_column)

    # Generate the output filename based on the input file
    base_filename = os.path.splitext(input_filename)[0]  # Remove the .csv extension
    output_filename = f"{base_filename}-transformed.csv"  # Add -transformed.csv

    # Save the pivoted DataFrame to the dynamically generated output filename
    pivot_table.to_csv(output_filename)

    # Print the file path to use for downloading
    print(f"File saved to: {output_filename}")

# Set up the argument parser
def main():
    parser = argparse.ArgumentParser(description='Read CSV, pivot data and save to a new CSV file')
    parser.add_argument('input_file', help='The input CSV file to process')

    # Parse the arguments
    args = parser.parse_args()

    # Process the input CSV file
    process_csv(args.input_file)

if __name__ == '__main__':
    main()
