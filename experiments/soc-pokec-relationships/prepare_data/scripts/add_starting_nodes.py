# TODO
# 1. Sample starting nodes
# 2. Add them to create new pathfinder file (just pokec_pathfinder.txt content + the declaration of start nodes)

import argparse
import random

import random

random.seed(42)  # Replace 42 with any fixed number

def sample_numbers(n, k):
    return random.sample(range(1, n + 1), k)

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


def generate_random_starting_nodes_labels(total_nodes_count, starting_nodes_counts, generated_lines_handler):
    sum_starting_nodes_in_all_categories = sum(starting_nodes_counts)
    starting_nodes_ids = sample_numbers(total_nodes_count, sum_starting_nodes_in_all_categories)

    sum_previous_nodes = 0
    for category, starting_node_in_category_count in enumerate(starting_nodes_counts):
        assign_labels_lines = generate_assign_label_to_nodes_lines(starting_nodes_ids[sum_previous_nodes:sum_previous_nodes + starting_node_in_category_count],
                                            f"start{category}")
        generated_lines_handler(assign_labels_lines)
        sum_previous_nodes += starting_node_in_category_count
    return

TOTAL_NODES_COUNT=1632803

def main():
    parser = argparse.ArgumentParser(description='Number of starting nodes to test')
    parser.add_argument('starting_nodes_counts', nargs='*', type=int, default=[10, 100, 1000],
                        help='List of starting nodes counts to sample in each category (default: [10, 100, 1000])')
    parser.add_argument('--total_nodes_count', type=int, default=TOTAL_NODES_COUNT,
                        help="Number of nodes to choose from for starting nodes (default: 1632803)")
    args = parser.parse_args()

#    for s in args.starting_nodes:
#        print(s)
    generate_random_starting_nodes_labels(args.total_nodes_count, args.starting_nodes_counts, lambda x: list(map(lambda item: print(item), x)))

if __name__ == "__main__":
    main()
