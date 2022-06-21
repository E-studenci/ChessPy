import collections

import chesspy

import model


class Agent:
    def __init__(self, lr, max_memory) -> None:
        self.gamma = 0.9
        self.max_memory = max_memory
        self.memory = {}
        self.model = model.Linear_QNet(12, 64, 1)
        self.trainer = model.QTrainer(self.model, lr, self.gamma)
        
    def get_state(self, game: chesspy.Board):
        return game.fen

    def remember(self, fen, results):
        if not self.memory.get(fen):
            self.memory[fen] = collections.deque(maxlen=self.max_memory)
        self.memory[fen].append(results)

