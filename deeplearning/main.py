import hydra
import chesspy
import exposed
import torch

import environment as env
import model as md
import agent as ag


@hydra.main(config_path="config", config_name="default.yaml", version_base=None)
def main(config):
    model = md.Linear_QNet(12, 64, 1)

    def evaluate_move(board: chesspy.Board, move: chesspy.Move):
        start_row = move.origin.row
        start_column = move.origin.column

        end_row = move.destination.row
        end_column = move.destination.column

        moving_piece = board.board[start_row][start_column]
        capture_piece = board.board[end_row][end_column]

        result = [0] * 12

        if moving_piece > 6:
            result[moving_piece - 7] = 1
            if capture_piece != 0:
                result[capture_piece + 5] = 1
        else:
            result[moving_piece - 1] = 1
            if capture_piece != 0:
                result[capture_piece - 1] = 1
        evaluation = model.forward(torch.Tensor(result))
        return float(evaluation)

    exposed.set_func(evaluate_move)

    agent = ag.Agent(model, config.model_save_path, config.lr, config.max_memory)

    awardee = env.Awardee(config.no_change_range)

    while True:
        try:
            for board in env.GameProvider(config.data_path):
                engine = chesspy.SearchEngine(
                    chesspy.MoveOrderingType.TRAINING,
                    chesspy.SearchParams(
                        use_null_move_pruning=False,
                        use_killer_moves=True,
                        use_hashed_positions=False,
                        use_hashed_moves=False,
                        use_quiescence=True,
                        use_check_extension=True,
                        use_MVVLVA=True,
                    ),
                )

                for result in env.GameManager(
                    board,
                    engine,
                    config.max_game_length,
                    config.max_depth,
                    config.max_move_time,
                ):

                    result = engine.root(board, config.max_depth, config.max_move_time)

                    board.make_move(result.best_move)

                    reward = awardee.award(
                        result.node_count[-1], agent.memory.get(board.hash)
                    )

                    agent.train(board.hash, reward)

                    agent.remember(board.hash, result.node_count[-1], reward)
            agent.model_iteration += 1
            agent.save_model()
        except KeyboardInterrupt:
            break


if __name__ == "__main__":
    main()
