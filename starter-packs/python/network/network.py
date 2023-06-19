import asyncio
import websockets

from core.game_state import GameState
from core.action import Action
from src.bot import MyBot

class Socket:
    def __init__(self, url: str, secret: str) -> None:
        self.__url = url
        self.__secret = secret
        self.__ws = None
        self.__queue = asyncio.Queue[GameState](maxsize=1)

    async def __connect(self):
        extra_headers = { 'Authorization': self.__secret }
        async with websockets.connect(self.__url, extra_headers=extra_headers) as self.__ws:
            while True:
                message = await self.__ws.recv()
                state = GameState.deserialize(message)
                print(f"Received {len(message)}")
                try:
                    self.__queue.put_nowait(state)
                except asyncio.QueueFull:
                    self.__queue.get_nowait()
                    self.__queue.put_nowait(state)

    async def __process_queue(self):
        while True:
            state = await self.__queue.get()
            action = MyBot().tick(state)
            print(f"Sending {action.serialize()}")
            await self.__ws.send(action.serialize())

    async def run(self):
        await asyncio.gather(self.__connect(), self.__process_queue())

# class Socket:
#     def __init__(self, websocket_url: str, secret: str) -> None:
#         self.__websocket_url = websocket_url
#         self.__secret = secret
#         self.__ws = None
#         self.__queue = Queue[GameState](maxsize=1)
#         self.__bot = MyBot()

#     async def process_queue(self) -> None:
#         print("TOTO")
#         while True:
#             state: GameState = self.__queue.get()
#             action = self.__bot.tick(state)
#             print(f"Sending {action.serialize()}")
#             self.__ws.send(action.serialize())


#     def __enter__(self) -> 'Socket':
#         self.__ws = websocket.create_connection(self.__websocket_url, header=[f"Authorization: {self.__secret}"])
#         asyncio.run(self.receive_message())
#         return self

#     def __exit__(self, exc_type, exc_value, traceback) -> None:
#         self.__ws.close()

#     async def receive_message(self) -> None:
#         process_task= asyncio.create_task(self.process_queue())
#         while True:
#             # print("en attente de receception")
#             message = self.__ws.recv()
#             print(f"Received {len(message)}")

#             if not self.__queue.empty():
#                 self.__queue.get_nowait()

#             self.__queue.put_nowait(GameState.deserialize(message))

#         await process_task