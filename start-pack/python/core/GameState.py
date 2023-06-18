from dataclasses import dataclass
from typing import List, Tuple

@dataclass
class Player:
    id: str
    pos: Tuple[int, int]
    alive: int
    trail: List[Tuple[int, int]]
    region: List[Tuple[int, int]]

@dataclass
class GameState:
    rows: int
    cols: int
    players: List[Player]
