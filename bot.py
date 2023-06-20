import struct
import websocket

from typing import List, Tuple

class Player:
    UUID_SIZE = 15

    def __init__(self):
        self.id: str | None = None
        self.pos: Tuple[int, int] | None = None
        self.alive: int | None = None
        self.trail: List[tuple[int, int]] = []
        self.region: List[tuple[int, int]] = []

    
    def deserialize(self, data: bytes) -> None:
        self.id = str(data[:self.UUID_SIZE]).rstrip('\0')
        self.pos = struct.unpack('<II', data[self.UUID_SIZE:self.UUID_SIZE + 8])
        self.alive = struct.unpack('<I', data[self.UUID_SIZE + 8:self.UUID_SIZE + 12])[0]
        data = data[self.UUID_SIZE + 12:]

        trail_length = struct.unpack("<I", data[:1])[0]
        data = data[4:]

        for _ in range(trail_length):
            self.trail.append(struct.unpack('<II', data[:8]))
            data = data[8:]

        region_length = struct.unpack("<I", data[:4])[0]
        data = data[4:]

        for _ in range(region_length):
            self.region.append(struct.unpack('<II', data[:8]))
            data = data[8:]

    def __str__(self) -> str:
        return f"Player(id={self.id}, pos={self.pos}, alive={self.alive}, trail={self.trail}, region={self.region})"


class Deserializer:
    def __init__(self, data: bytes) -> None:
        self.__data: bytes = data
        self.__offset: int = 0

    def deserialize(self) -> Tuple[int, int, List[Player]]:
        print(f"Deserializing {len(self.__data)}")
        rows, cols, frame = struct.unpack('<III', self.__data[self.__offset:self.__offset + 12])
        self.__offset += 12

        players: List[Player] = []
        while self.__offset < len(self.__data):
            player = Player()
            player.deserialize(self.__data[self.__offset:])
            players.append(player)
            self.__offset += Player.UUID_SIZE + 12 + 1 + 4 + (len(player.trail) + len(player.region)) * 8

        return rows, cols, players


from enum import IntEnum

# class Teleportation:
    

class Direction(IntEnum):
    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3

# class Action(IntEnum):
#     def do(action: Direction | Teleportation) -> str:
#         pass

class Path:
    def __init__(self) -> None:
        self.__actions: List[Direction] = [
            Direction.UP,
            Direction.UP,
            Direction.UP,
            Direction.UP,
            Direction.UP,
            Direction.UP,
            Direction.LEFT,
            Direction.LEFT,
            Direction.LEFT,
            Direction.LEFT,
            Direction.DOWN,
            Direction.DOWN,
            Direction.DOWN,
            Direction.DOWN,
            Direction.DOWN,
            Direction.DOWN,
            Direction.RIGHT,
            Direction.RIGHT,
            Direction.RIGHT,
            Direction.RIGHT,
        ]
        self.__idx: int = 0

    def next(self) -> str:
        self.__idx += 1
        return chr(self.__actions[self.__idx % len(self.__actions)])


ws = websocket.create_connection("ws://localhost:8080/game", header=[f"Authorization: 001XXLYEMKFYYMZ"])

path = Path()

def receive_response(ws):
    data = ws.recv()
    print(f"Received {len(data)}")
    # rows, cols, players = Deserializer(data).deserialize()
    # print(f"Received {rows} {cols}", ", ".join([f"{player}" for player in players]))

def send_request(ws):
    ws.send(path.next())


while True:
    send_request(ws)
    receive_response(ws)

