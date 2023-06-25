from enum import IntEnum
from dataclasses import dataclass
from typing import Union

import struct

class Direction(IntEnum):
    """
    Direction in which the player can move
    """
    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3

    def serialize(self) -> str:
        return chr(self.value)

@dataclass
class Teleport:
    """
    Teleportation to a specific location
    """
    x: int
    y: int

    def serialize(self) -> str:
        return b'\x05' + struct.pack('i', self.x) + struct.pack('i', self.y)


@dataclass
class Action:
    action_type: Union[Teleport, Direction]

    def serialize(self) -> str:
        return self.action_type.serialize()
