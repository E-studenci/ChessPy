import hydra
import chesspy
import exposed
import torch

import environment as env
import model as m


@hydra.main(config_path="config", config_name="default.yaml", version_base=None)
def main(config):
    model = m.MoveEvaluator()
    
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
    
    for board in env.GameProvider(config.data_path):
        engine = chesspy.SearchEngine(chesspy.MoveOrderingType.TRAINING, True)
        for result in env.GameManager(
            board,
            engine,
            config.max_game_length,
            config.max_depth,
            config.max_move_time,
        ):
            print(board.fen, result.node_count)


if __name__ == "__main__":
    main()
