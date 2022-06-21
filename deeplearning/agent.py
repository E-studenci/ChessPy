import os
import datetime
import collections

import torch

import model as md


class Agent:
    def __init__(self, model, model_save_path, lr, max_memory) -> None:
        self.model = model
        self.model_save_path = model_save_path
        self.max_memory = max_memory
        self.gamma = 0.9
        self.memory = {}
        self.trainer = md.QTrainer(self.model, lr, self.gamma)
        self.model_iteration = 0

    def remember(self, fen, nodes, reward):
        if not self.memory.get(fen):
            self.memory[fen] = collections.deque(maxlen=self.max_memory)
        self.memory[fen].append((nodes, reward))

    def save_model(self):
        date_time = datetime.datetime.now().strftime("%m-%d-%Y-%H-%M-%S")
        path = os.path.join(
            self.model_save_path, f"{self.model_iteration}-iteration-{date_time}.model"
        )
        torch.save(self.model, path)

    def train(self, state, current_result):
        previous_results = self.memory.get(state)
        if previous_results:
            self.trainer.train_step(current_result, previous_results)
