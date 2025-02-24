# TODO
# 1. Sample starting nodes
# 2. Add them to create new pathfinder file (just pokec_pathfinder.txt content + the declaration of start nodes)

import argparse
import random

def sample_numbers(n, k):
    return random.sample(range(1, n + 1), k)

# awk -F'P|->| ' '{for(i=1;i<=NF;i++) if($i ~ /^[0-9]+$/ && $i > max) max=$i} END{print max}' $FILE
# 1632803

N_NODES=1632803

def main():
    parser = argparse.ArgumentParser(description='Number of starting nodes to test')
    parser.add_argument('starting_nodes', nargs='*', type=int, default=[10, 100, 1000],
                        help='List of starting nodes to sample in each dimension (default: [10, 100, 1000])')
    parser.add_argument('--n_nodes', type=int, default=N_NODES,
                        help="Number of nodes to choose from for starting nodes (default: 1632803)")
    args = parser.parse_args()

    for s in args.starting_nodes:
        print(s)


if __name__ == "__main__":
    main()
