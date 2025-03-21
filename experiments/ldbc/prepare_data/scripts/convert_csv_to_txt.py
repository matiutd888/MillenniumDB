import pandas as pd
import sys

def convert_csv_to_txt(input_file):
    """Reads a CSV file and prints the transformed output to stdout."""
    try:
        df = pd.read_csv(input_file, sep="|")
        for _, row in df.iterrows():
            print(f"P{row['Person1Id']}->P{row['Person2Id']} :Knows")
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python convert_csv_to_txt.py <input_file.csv>", file=sys.stderr)
        sys.exit(1)

    input_filepath = sys.argv[1]
    convert_csv_to_txt(input_filepath)
