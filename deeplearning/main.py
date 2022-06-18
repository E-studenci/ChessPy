import os

import hydra
import chesspy

import reinforcement.environment as env


@hydra.main(config_path="config", config_name="default.yaml", version_base=None)
def main(config):
    for board in env.GameProvider(config.data_path):
        engine = chesspy.SearchEngine(0)
        for result in env.GameManager(
            board,
            engine,
            config.max_game_length,
            config.max_depth,
            config.max_move_time,
        ):
            print(board.fen, result.node_count)


def evaluate_move(board, move):
    return 0


if __name__ == "__main__":
    main()
