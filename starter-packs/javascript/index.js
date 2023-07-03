import { ArgumentParser } from 'argparse';

import { Socket } from './network/network.js';

const DEFAULT_BASE_URL = 'ws://localhost:8080/game';

async function loop(secret) {
  await new Socket(DEFAULT_BASE_URL, secret).run();
}

const parser = new ArgumentParser({ description: 'Starts the bot' });
parser.add_argument('-s', '--secret', { help: 'The secret which authenticates your bot', required: true });

const args = parser.parse_args();

loop(args.secret).catch((error) => {
  console.error(error);
  process.exit(1);
});