from dataclasses import dataclass
from typing import List, Tuple

import struct

"""
(fr)
Représente une joueur.

(en)
Represents a player.

Attributes:
    name (str):                     (fr) Nom du joueur.
                                    (en) Name of the player.

    pos (Tuple[int, int]):          (fr) Position du joueur. [x, y]
                                    (en) Position of the player. [x, y]
    
    alive (int):                    (fr) Nombre de ticks depuis lequel le joueur est en vie.
                                    (en) Number of ticks since the player is alive.

    trail (List[Tuple[int, int]]):  (fr) Liste des traces du joueur. Si un autre joueur passe sur une de ces
                                         positions, il meurt. Tableau de positions [[x, y], ...].
                                    (en) List of the player's traces. If another player passes over one of these
                                         positions, he dies. Array of positions [[x, y], ...].

    region (List[Tuple[int, int]]): (fr) Liste des positions de la région du joueur. Si un autre joueur passe
                                         sur une de ces positions, il retire cette position de la région du joueur.
                                         Tableau de positions [[x, y], ...].
                                    (en) List of the player's region positions. If another player passes over one of these
                                         positions, he removes this position from the player's region.
                                         Array of positions [[x, y], ...].
 """
@dataclass
class Player:
    name: str
    pos: Tuple[int, int]
    alive: int
    trail: List[Tuple[int, int]]
    region: List[Tuple[int, int]]


"""
(fr)
Représente l'état du jeu à un instant donné.

(en)
Represents the state of the game at a given time.

Attributes:
    rows (int):                 (fr) Nombre de lignes de la carte.
                                (en) Number of rows of the map.

    cols (int):                 (fr) Nombre de colonnes de la carte.
                                (en) Number of columns of the map.

    tick(int):                  (fr) Numéro du tick actuel.
                                (en) Number of the current tick.

    players (List[Player]):     (fr) Liste des joueurs.
                                (en) List of players.
"""
@dataclass
class GameState:
    rows: int
    cols: int
    players: List[Player]

    @classmethod
    def deserialize(cls, data: bytes) -> 'GameState':
        offset = 0
        rows, cols, frame = struct.unpack_from('<III', data, offset)
        offset += 12

        players = []
        while offset < len(data):
            name_size = struct.unpack_from('<I', data, offset)[0]
            offset += 4

            name = data[offset:offset+name_size].decode('utf-8')
            print("--------------------------")
            print(name)
            print("--------------------------")
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

        return cls(rows=rows, cols=cols, players=players)
    
    def __str__(self) -> str:
        return f"GameState(rows={self.rows}, cols={self.cols}, players={self.players})"