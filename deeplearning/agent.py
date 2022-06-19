import collections

import chesspy

import model


class Agent:
    def __init__(self, lr, max_memory) -> None:
        self.gamma = 0.9
        self.memory = collections.deque(maxlen=max_memory)
        self.model = model.Linear_QNet(12, 64, 1)
        self.trainer = model.QTrainer(self.model, LR=lr, gamma=self.gamma)
        
    def get_state(self, game: chesspy.Board):
        return game.fen

    def remember(self, state, action, reward, next_state, done):
        self.memory.append((state, action, reward, next_state, done)) # popleft if MAX_MEMORY is reached
