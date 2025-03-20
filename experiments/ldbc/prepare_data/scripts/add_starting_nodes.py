# 1. Sample starting nodes
# 2. Add them to create new pathfinder file (just pokec_pathfinder.txt content + the declaration of start nodes)

import argparse
import random

import random

random.seed(42)  # Replace 42 with any fixed number

def sample_numbers(all_numbers, k):
    return random.sample(all_numbers, k)

# awk -F'P|->| ' '{for(i=1;i<=NF;i++) if($i ~ /^[0-9]+$/ && $i > max) max=$i} END{print max}' $FILE
# 1632803
# Joe :start

def append_list_to_file(file_path, items):
    """
    Appends each item in the list to a new line in the given file.

    :param file_path: Path to the file
    :param items: List of elements to append
    """
    with open(file_path, 'a') as file:
        for item in items:
            file.write(f"{item}\n")  # Ensure each item is on a new line

def generate_assign_label_to_nodes_lines(node_ids, label):
    return list(map(lambda node_id: f"P{node_id}: {label}", node_ids))


def generate_random_starting_nodes_labels(nodes_list, starting_nodes_counts, generated_lines_handler):
    sum_starting_nodes_in_all_categories = sum(starting_nodes_counts)
    starting_nodes_ids = sample_numbers(nodes_list, sum_starting_nodes_in_all_categories)

    sum_previous_nodes = 0
    for category, starting_node_in_category_count in enumerate(starting_nodes_counts):
        assign_labels_lines = generate_assign_label_to_nodes_lines(starting_nodes_ids[sum_previous_nodes:sum_previous_nodes + starting_node_in_category_count],
                                            f"start{category}")
        generated_lines_handler(assign_labels_lines)
        sum_previous_nodes += starting_node_in_category_count
    return

def extract_nodes_from_file(file_path):
    """Extracts unique node IDs from the graph file."""
    nodes = set()

    try:
        with open(file_path, "r") as f:
            for line in f:
                parts = line.strip().split("->")
                if len(parts) == 2:
                    person1 = parts[0].strip().lstrip("P")  # Remove "P" prefix
                    person2 = parts[1].split(":")[0].strip().lstrip("P")
                    nodes.add(person1)
                    nodes.add(person2)

        return list(nodes)
    except Exception as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description='Number of starting nodes to test')
    parser.add_argument('starting_nodes_counts', nargs='*', type=int, default=[10, 100, 1000],
                        help='List of starting nodes counts to sample in each category (default: [10, 100, 1000])')
    parser.add_argument('--file_with_graph', type=str,
                        help="Path to file with graph structure")
    args = parser.parse_args()

    all_nodes_list = extract_nodes_from_file(args.file_with_graph)
    generate_random_starting_nodes_labels(all_nodes_list, args.starting_nodes_counts, lambda x: list(map(lambda item: print(item), x)))

if __name__ == "__main__":
    main()
