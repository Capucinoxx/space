from dataclasses import dataclass
from typing import List, Tuple

import struct

@dataclass
class Player:
    name: str
    pos: Tuple[int, int]
    alive: int
    trail: List[Tuple[int, int]]
    region: List[Tuple[int, int]]

@dataclass
class GameState:
    rows: int
    cols: int
    players: List[Player]
    inacessible_tiles: List[Tuple[int, int]]

    @classmethod
    def deserialize(cls, data: bytes) -> 'GameState':
        offset = 0
        rows, cols, frame = struct.unpack_from('<III', data, offset)
        offset += 12

        inacessible_tiles_length = struct.unpack_from('<I', data, offset)[0]
        offset += 4

        inacessible_tiles = struct.unpack_from('<' + 'II' * inacessible_tiles_length, data, offset)
        offset += inacessible_tiles_length * 8

        players = []
        while offset < len(data):
            name_size = struct.unpack_from('<I', data, offset)[0]
            offset += 4

            name = data[offset:name_size]
            offset += name_size

            # skip 24 bytes for the player's color
            offset += 24

            pos_x, pos_y, tick_alive = struct.unpack_from('<III', data, offset)
            offset += 12

            trail_length = struct.unpack_from('<I', data, offset)[0]
            offset += 4

            trail = struct.unpack_from('<' + 'II' * trail_length, data, offset)
            offset += trail_length * 8

            region_length = struct.unpack_from('<I', data, offset)[0]
            offset += 4

            region = struct.unpack_from('<' + 'II' * region_length, data, offset)
            offset += region_length * 8

        players.append(Player(name=name, pos=(pos_x, pos_y), 
                              alive=tick_alive, 
                              trail=list(zip(trail[::2], trail[1::2])), 
                              region=list(zip(region[::2], region[1::2]))))

        return cls(rows=rows, cols=cols, players=players, inacessible_tiles=inacessible_tiles)
    
    def __str__(self) -> str:
        return f"GameState(rows={self.rows}, cols={self.cols}, players={self.players})"