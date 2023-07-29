from dataclasses import dataclass
from typing import Dict, List, Tuple, Set

import struct


@dataclass
class Player:
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
        
        alive (int):                    (fr) Nombre de ticks depuis que le joueur est en vie.
                                        (en) Number of ticks since the player is alive.

        trail (Set[Tuple[int, int]]):   (fr) Liste des positions des traces du joueur. Si un autre joueur passe sur une de ces
                                            positions, il meurt. Set de positions [[x, y], ...].
                                        (en) List of the player's traces. If another player passes over one of these
                                            positions, he dies. Set of positions [[x, y], ...].

        region (Set[Tuple[int, int]]):  (fr) Liste des positions de la région du joueur. Si un autre joueur passe
                                            sur une de ces positions, il retire cette position de la région du joueur.
                                            Set de positions [[x, y], ...].
                                        (en) List of the player's region positions. If another player passes over one of these
                                            positions, he removes this position from the player's region.
                                            Set of positions [[x, y], ...].

        teleport_cooldown (int):        (fr) Nombre de de ticks avant que le joueur puisse réutiliser son action de téléportation.
                                        (en) Number of ticks before the player can use the teleport action again.
    """

    name: str
    pos: Tuple[int, int]
    alive: int
    trail: Set[Tuple[int, int]]
    region: Set[Tuple[int, int]]
    teleport_cooldown: int

    def __str__(self) -> str:
        return f"Player(name={self.name}, pos={self.pos}, alive={self.alive}, trail={self.trail}, region={self.region}, teleport_cooldown={self.teleport_cooldown})"


@dataclass
class GameState:
    """
    (fr)
    Représente l'état du jeu à un instant donné.

    (en)
    Represents the state of the game at a given time.

    Attributes:
        rows (int):                     (fr) Nombre de lignes de la carte.
                                        (en) Number of rows of the map.

        cols (int):                     (fr) Nombre de colonnes de la carte.
                                        (en) Number of columns of the map.

        tick(int):                      (fr) Numéro du tick actuel.
                                        (en) Number of the current tick.

        players (Dict[str, Player]):    (fr) Dictionnaire des joueurs. Clé: nom du joueur, Valeur: joueur.
                                        (en) Dictionnary of players. Key: player name, Value: player.
    """

    rows: int
    cols: int
    players: Dict[str, Player]

    @classmethod
    def deserialize(cls, data: bytes) -> 'GameState':
        offset = 0
        rows, cols, _ = struct.unpack_from('<III', data, offset)
        offset += 12

        players: Dict[str, Player] = {}
        while offset < len(data):
            name_size = struct.unpack_from('<I', data, offset)[0]
            offset += 4

            name = data[offset:offset+name_size].decode('utf-8')
            offset += name_size

            # skip 24 bytes for the player's color
            offset += 24

            pos_x, pos_y, tick_alive = struct.unpack_from('<III', data, offset)
            offset += 12

            teleport_cooldown = struct.unpack_from('<B', data, offset)[0]
            offset += 1

            trail_length = struct.unpack_from('<I', data, offset)[0]
            offset += 4

            trail = struct.unpack_from('<' + 'II' * trail_length, data, offset)
            offset += trail_length * 8

            region_length = struct.unpack_from('<I', data, offset)[0]
            offset += 4

            region = struct.unpack_from('<' + 'II' * region_length, data, offset)
            offset += region_length * 8

            players[name] = Player(name=name, pos=(pos_x, pos_y),
                                  alive=tick_alive,
                                  trail=set(zip(trail[::2], trail[1::2])),
                                  region=set(zip(region[::2], region[1::2])),
                                  teleport_cooldown=teleport_cooldown)

        return cls(rows=rows, cols=cols, players=players)
    
    def __str__(self) -> str:
        return f"GameState(rows={self.rows}, cols={self.cols}, players={self.players})"