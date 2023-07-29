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

        self.__bot = MyBot()

    async def __connect(self):
        extra_headers = { 'Authorization': self.__secret }
        try:
            async with websockets.connect(self.__url, extra_headers=extra_headers) as self.__ws:
            
                while True:
                    message = await self.__ws.recv()
                    state = GameState.deserialize(message)
                    try:
                        self.__queue.put_nowait(state)
                    except asyncio.QueueFull:
                        self.__queue.get_nowait()
                        self.__queue.put_nowait(state)

        except Exception as e:
            print(e)

    async def __process_queue(self):
        while True:
            state = await self.__queue.get()
            action = self.__bot.tick(state)
            await self.__ws.send(action.serialize())

    async def run(self):
        await asyncio.gather(self.__connect(), self.__process_queue())
