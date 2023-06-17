interface CanvasSize {
  rows: number;
  cols: number;
}

class Canvas {
  private current_size: CanvasSize;
  private next_size: CanvasSize;

  private canvas: HTMLCanvasElement;
  private canvas_ctx: CanvasRenderingContext2D;

  private max_width: number;
  private max_height: number;

  constructor(canvas: HTMLCanvasElement) {
    this.current_size = { rows: -1, cols: -1 };
    this.next_size = { rows: 0, cols: 0 };

    this.canvas = canvas;
    this.canvas_ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

    this.max_width = window.innerWidth * 0.8;
    this.max_height = window.innerHeight;

    this.render();

    window.addEventListener('resize', () => {
      this.max_width = window.innerWidth * 0.8;
      this.max_height = window.innerHeight;
      this.render();
    });
  }

  get ctx(): CanvasRenderingContext2D {
    return this.canvas_ctx;
  }

  public resize(rows: number, cols: number) {
    this.next_size = { rows, cols };
    if (this.has_changed())
      this.render();
  }

  public render() {
    this.current_size = this.next_size;

    const { rows, cols } = this.current_size;

    const cell_width = Math.floor(this.max_width / cols);
    const cell_height = Math.floor(this.max_height / rows);

    const cell_size = Math.min(cell_width, cell_height);

    this.canvas.width = cell_size * cols;
    this.canvas.height = cell_size * rows;

    this.canvas.style.width = `${this.canvas.width}px`;
    this.canvas.style.height = `${this.canvas.height}px`;
  }

  private has_changed(): boolean {
    return this.current_size.rows !== this.next_size.rows || this.current_size.cols !== this.next_size.cols;
  }
};

export { Canvas };