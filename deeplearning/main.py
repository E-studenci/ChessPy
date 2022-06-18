import os

import hydra

@hydra.main(config_path='config', config_name='default.yaml')
def main(config):
    pass


def evaluate_move(board, move):
    return 0


if __name__ == '__main__':
    main()
