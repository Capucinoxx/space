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
        self.id = data[:self.UUID_SIZE]
        self.pos = struct.unpack('<II', data[self.UUID_SIZE:self.UUID_SIZE + 8])
        self.alive = struct.unpack('<I', data[self.UUID_SIZE + 8:self.UUID_SIZE + 12])[0]
        data = data[self.UUID_SIZE + 12:]

        trail_length = struct.unpack("<B", data[:1])[0]
        data = data[1:]

        for _ in range(trail_length):
            self.trail.append(struct.unpack('<II', data[:8]))
            data = data[8:]

        region_length = struct.unpack("<I", data[:4])[0]
        data = data[4:]

        for _ in range(region_length):
            self.region.append(struct.unpack('<II', data[:8]))
            data = data[8:]

    def __str__(self) -> str:
        return f"Player(id={self.id}, pos={self.pos}, alive={self.alive}, trail={self.trail})"


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

def receive_response(ws):
    data = ws.recv_frame().data
    print(f"Received {len(data)}")
    rows, cols, players = Deserializer(data).deserialize()
    print(f"Received {rows} {cols}", ", ".join([f"{player}" for player in players]))

def send_request(ws):
    request = "\x03"
    ws.send(request)
    print(f"Send {request}")

import random


ws = websocket.create_connection("ws://localhost:8080/game", header=[f"Authorization: test1234"])

while True:
    send_request(ws)
    receive_response(ws)
