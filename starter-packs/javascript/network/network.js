import { WebSocket } from 'ws';
import { GameState }  from '../core/game_state.js';
import { MyBot } from '../src/bot.js';

class Socket {
  constructor(url, secret) {
    this.url = url;
    this.secret = secret;
    this.ws = null;
    this.queue = [];

    this.bot = new MyBot();
    this.isProcessing = false;
  }

  async connect() {
    const ws = new WebSocket(this.url, {
      headers: { 'Authorization': this.secret }
    });
    ws.binaryType = 'arraybuffer';

    console.log(`Connected to ${this.url}`);

    ws.on('open', () => {});

    ws.on('message', async (message) => {
      const state = GameState.deserialize(message);
      try {
        this.queue.push(state);
        if (!this.isProcessing) {
          this.processQueue();
        }
      } catch(err) {
        console.error(err);
      }
    });

    this.ws = ws;
  }

  async processQueue() {
    this.isProcessing = true;
    while (this.queue.length > 0) {
      const state = this.queue.shift();
      const action = this.bot.tick(state);

      this.ws.send(action.serialize());
    }
    this.isProcessing = false;
  }

  async run() {
    await this.connect();
  }
}

export { Socket };
