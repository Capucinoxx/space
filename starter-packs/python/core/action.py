from enum import IntEnum
from dataclasses import dataclass
from typing import List, Union

import struct

class Direction(IntEnum):
    """
    Direction in which the player can move
    """
    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3

    def serialize(self) -> bytes:
        return bytes([self.value])

@dataclass
class Teleport:
    """
    Teleportation to a specific location
    """
    x: int
    y: int

    def serialize(self) -> bytes:
        return b'\x05' + struct.pack('ii', self.x, self.y)

@dataclass
class Pattern:
    actions: List[Union[Teleport, Direction]]

    def serialize(self) -> bytes:
        serialized_actions = b''.join([action.serialize() for action in self.actions])
        return b'\x07' + serialized_actions

@dataclass
class Action:
    action_type: Union[Teleport, Direction, Pattern]

    def serialize(self) -> bytes:
        return self.action_type.serialize()
