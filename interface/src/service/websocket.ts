type MessageCallback = (message: ArrayBuffer) => void;

class WebsocketService {
  private readonly socket: WebSocket;
  private cb: MessageCallback | null = null;

  constructor(url: string) {
    this.socket = new WebSocket(url);
    this.socket.binaryType = 'arraybuffer';
    
    this.socket.onmessage = this.handle_message;
  }

  private handle_message = (message: MessageEvent) => {
    if (this.cb && message.data instanceof ArrayBuffer)
      this.cb(message.data);
  };

  public subscribe(cb: MessageCallback) {
    this.cb = cb;
  }
}

export { WebsocketService };

