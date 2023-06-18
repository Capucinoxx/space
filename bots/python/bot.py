from core.action import Action, Direction
import random

class MyBot:
    def __init__(self, id: str) -> None:
        self.__id = id

    def __random_action(self) -> Action:
        return Action(direction=random.choice(list(Direction)))

    def tick(self, state) -> Action:
        return self.__random_action()