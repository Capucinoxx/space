from network.network import Socket
import argparse
import asyncio


DEFAULT_BASE_URL = "ws://localhost:8080/unranked/game"

async def loop(secret: str, url: str = DEFAULT_BASE_URL):
    await Socket(DEFAULT_BASE_URL, secret).run()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Starts the bot")
    parser.add_argument("-s", "--secret", help="The secret wich authenticates your bot", required=True)
    parser.add_argument("-r", "--rank", action='store_true', help="If set, the bot will play ranked games")

    args = parser.parse_args()

    if args.rank:
        DEFAULT_BASE_URL = "ws://localhost:8080/ranked/game"

    asyncio.run(loop(args.secret, DEFAULT_BASE_URL))
    