import random
from core.action import Action, Direction
from core.game_state import GameState

class MyBot:
    def __init__(self):
        self.__name = "MyBot"

    def tick(self, state: GameState) -> Action:
       print(state.__str__())
       # program your bot here
       

       return random.choice(list(Direction))