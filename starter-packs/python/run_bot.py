from network.network import Socket
import argparse
import asyncio

DEFAULT_BASE_URL = "ws://jdis-ia.dinf.fsci.usherbrooke.ca"

async def loop(secret: str, url: str):
    await Socket(url, secret).run()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Starts the bot")
    parser.add_argument("-s", "--secret", help="The secret that authenticates your bot", required=True)
    parser.add_argument("-r", "--rank", action='store_true', help="If set, the bot will play ranked games")
    parser.add_argument("-u", "--url", help="The url of the server", default=DEFAULT_BASE_URL)

    args = parser.parse_args()

    args.url = args.url.strip()

    channel = "/unranked/game"
    if args.rank:
        channel = "/ranked/game"

    asyncio.run(loop(args.secret, args.url + channel))
    