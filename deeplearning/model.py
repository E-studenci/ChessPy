import torch
import torch.nn as nn
import torch.optim as optim
import torch.autograd as autograd


class Linear_QNet(nn.Module):
    def __init__(self, input_size, hidden_size, output_size) -> None:
        super().__init__()
        self.seq = nn.Sequential(
            nn.Linear(input_size, hidden_size),
            nn.ReLU(),
            nn.Linear(hidden_size, output_size),
        )

    def forward(self, X):
        X = self.seq(X)
        return X


class QTrainer:
    def __init__(self, model, lr, gamma):
        self.lr = lr
        self.gamma = gamma
        self.model = model
        self.optimizer = optim.Adam(model.parameters(), lr=self.lr)
        self.criterion = nn.MSELoss()

    def train_step(self, current_reward, previous_results):
        previous_rewards = list(map(lambda item: item[1], previous_results))
        expected = (min(previous_rewards) + self.gamma * current_reward) / 2
        current_reward_tensor = torch.Tensor([current_reward])
        expected_tensor = torch.Tensor([expected])
        self.optimizer.zero_grad()
        loss = self.criterion(current_reward_tensor, expected_tensor)
        loss = autograd.Variable(loss, requires_grad=True)
        loss.backward()
        self.optimizer.step()
