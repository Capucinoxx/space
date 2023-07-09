import { ArgumentParser } from 'argparse';

import { Socket } from './network/network.js';

let DEFAULT_BASE_URL = 'ws://localhost:8080/unranked/game';

async function loop(secret, url) {
  await new Socket(url, secret).run();
}

const parser = new ArgumentParser({ description: 'Starts the bot' });
parser.add_argument('-s', '--secret', { help: 'The secret which authenticates your bot', required: true });
parser.add_argument('-r', '--rank', { help: 'If set, the bot will play ranked game', action: 'store_true' });

const args = parser.parse_args();

if (args.rank)
  DEFAULT_BASE_URL = 'ws://localhost:8080/ranked/game';

loop(args.secret, DEFAULT_BASE_URL).catch((error) => {
  console.error(error);
  process.exit(1);
});