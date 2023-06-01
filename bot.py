import struct
import websocket

from typing import List, Tuple

class Player:
    UUID_SIZE = 15

    def __init__(self):
        self.id: str | None = None
        self.pos: Tuple[int, int] | None = None
        self.trail: List[tuple[int, int]] = []

    
    def deserialize(self, data: bytes) -> None:
        self.id = data[:self.UUID_SIZE]
        self.pos = struct.unpack('<II', data[self.UUID_SIZE:self.UUID_SIZE + 8])
        data = data[self.UUID_SIZE + 8:]

        count = struct.unpack("<B", data[:1])[0]
        data = data[1:]

        for _ in range(count):
            self.trail.append(struct.unpack('<II', data[:8]))
            data = data[8:]

    def __str__(self) -> str:
        return f"Player(id={self.id}, pos={self.pos}, trail={self.trail})"


class Deserializer:
    def __init__(self, data: bytes) -> None:
        self.__data: bytes = data
        self.__offset: int = 0

    def deserialize(self) -> Tuple[int, int, List[Player]]:
        rows, cols = struct.unpack('<II', self.__data[self.__offset:self.__offset + 8])
        self.__offset += 8

        players: List[Player] = []
        while self.__offset < len(self.__data):
            player = Player()
            player.deserialize(self.__data[self.__offset:])
            players.append(player)
            self.__offset += Player.UUID_SIZE + 8 + 1 + len(player.trail) * 8

        return rows, cols, players

def receive_response(ws):
    data = ws.recv_frame().data
    rows, cols, players = Deserializer(data).deserialize()
    print(f"Received {rows} {cols}", ", ".join([f"{player}" for player in players]))

def send_request(ws):
    request = "\x03"
    ws.send(request)
    print(f"Send {request}")


ws = websocket.create_connection("ws://localhost:8030/game", header=["Authorization: test1234"])

while True:
    send_request(ws)
    receive_response(ws)
