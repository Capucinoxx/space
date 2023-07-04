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

  private cell_width: number;

  constructor(canvas: HTMLCanvasElement) {
    this.current_size = { rows: -1, cols: -1 };
    this.next_size = { rows: 0, cols: 0 };
    this.cell_width = 0;

    this.canvas = canvas;
    this.canvas_ctx = canvas.getContext('2d') as CanvasRenderingContext2D;

    this.max_width = this.canvas.parentElement!.clientWidth;
    this.max_height = this.canvas.parentElement!.clientHeight - 150;

    this.render();

    window.addEventListener('resize', () => {
      this.max_width = this.canvas.parentElement!.clientWidth;
      this.max_height = this.canvas.parentElement!.clientHeight - 150;
      this.render();
    });
  }

  get ctx(): CanvasRenderingContext2D {
    return this.canvas_ctx;
  }

  get cell_size(): number {
    return this.cell_width;
  }

  public resize(rows: number, cols: number) {
    this.next_size = { rows, cols };
    if (this.has_changed())
      this.render();
  }

  public render() {
    this.current_size = this.next_size;

    const { rows, cols } = this.current_size;

    const cell_width = Math.floor(this.max_width / rows);
    const cell_height = Math.floor(this.max_height / cols);

    this.cell_width = Math.min(cell_width, cell_height);

    this.canvas.width = this.cell_width * rows;
    this.canvas.height = this.cell_width * cols;

    this.canvas.style.width = `${this.canvas.width}px`;
    this.canvas.style.height = `${this.canvas.height}px`;
  }

  private has_changed(): boolean {
    return this.current_size.rows !== this.next_size.rows || this.current_size.cols !== this.next_size.cols;
  }
};

export { Canvas };