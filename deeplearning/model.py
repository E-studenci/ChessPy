import torch
import torch.nn as nn


class MoveEvaluator(nn.Module):
    def __init__(self) -> None:
        super().__init__()
        self.seq = nn.Sequential(
            nn.Linear(12, 64),
            nn.ReLU(),
            nn.Linear(64, 1)
        )
    
    def forward(self, X):
        X = self.seq(X)
        return X