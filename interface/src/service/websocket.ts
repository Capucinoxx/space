type MessageCallback = (message: ArrayBuffer) => void;

class WebsocketService {
  private socket: WebSocket | null = null;
  private cb: MessageCallback | null = null;

  constructor() {}

  public connect(url: string) {
    this.disconnect();

    console.log(`Connecting to ${url}`);

    this.socket = new WebSocket(url);
    this.socket.binaryType = 'arraybuffer';

    this.socket.onmessage = this.handle_message;
  }

  public disconnect() {
    if (this.socket) {
      this.socket.close();
      this.socket = null;
    }
  }

  private handle_message = (message: MessageEvent) => {
    if (this.cb && message.data instanceof ArrayBuffer) {
      this.cb(message.data);
    }
  };

  public subscribe(cb: MessageCallback) {
    this.cb = cb;
  }
}

export { WebsocketService };
