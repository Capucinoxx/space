import { ArgumentParser } from 'argparse';

import { Socket } from './network/network.js';

const DEFAULT_BASE_URL = 'ws://jdis-ia.dinf.fsci.usherbrooke.ca';

async function loop(secret, url) {
  await new Socket(url, secret).run();
}

const parser = new ArgumentParser({ description: 'Starts the bot' });
parser.add_argument('-s', '--secret', { help: 'The secret that authenticates your bot', required: true });
parser.add_argument('-r', '--rank', { help: 'If set, the bot will play ranked game', action: 'store_true' });
parser.add_argument('-u', '--url', { help: 'The url of the game server', default: DEFAULT_BASE_URL });

const args = parser.parse_args();
args.url = args.url.trim();

let channel = '/unranked/game';

if (args.rank)
  channel = '/ranked/game';

loop(args.secret, args.url + channel).catch((error) => {
  console.error(error);
  process.exit(1);
});