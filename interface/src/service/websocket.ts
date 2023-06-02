type MessageCallback = (message: string) => void;

class WebsocketService {
  private readonly socket: WebSocket;
  private cb: MessageCallback | null = null;

  constructor(url: string) {
    console.log(`Connecting to ${url}`);
    this.socket = new WebSocket(url);
    this.socket.onmessage = this.handle_message.bind(this);
    this.socket.onclose = this.handle_close;
  }

  private handle_message = (message: MessageEvent) => {
    if (this.cb)
      this.cb(message.data);
  };

  private handle_close = () => {
    this.socket.close();
  };

  public subscribe(cb: MessageCallback) {
    this.cb = cb;
  }


};

export { WebsocketService };
