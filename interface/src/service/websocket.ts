import WebSocket from 'ws';

type MessageCallback = (message: string) => void;

class WebsocketService {
  private readonly socket: WebSocket;
  private cb: MessageCallback | null = null;

  constructor(url: string) {
    this.socket = new WebSocket(url);
    this.socket.on('open', this.handle_open)
    this.socket.on('message', this.handle_message)
    this.socket.on('close', this.handle_close)
  }

  private handle_open = () => {};

  private handle_message = (message: WebSocket.Data) => {
    if (typeof message === 'string' && this.cb)
      this.cb(message);
  };

  private handle_close = () => {};

  public subscribe(cb: MessageCallback) {
    this.cb = cb;
  }
};

export { WebsocketService };
