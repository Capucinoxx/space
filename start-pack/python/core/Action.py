from enum import IntEnum
from dataclasses import dataclass
from typing import Union

class Direction(IntEnum):
    """
    Direction in which the player can move
    """
    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3

@dataclass
class Teleport:
    """
    Teleportation to a specific location
    """
    x: int
    y: int


@dataclass
class Action:
    action_type: Union[Teleport, Direction]
