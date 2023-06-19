from network.network import Socket
import argparse
import asyncio


DEFAULT_BASE_URL = "ws://localhost:8080/game"

async def loop(secret: str):
    await Socket(DEFAULT_BASE_URL, secret).run()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Starts the bot")
    parser.add_argument("-s", "--secret", help="The secret wich authenticates your bot", required=True)

    args = parser.parse_args()

    asyncio.run(loop(args.secret))
    