import { HSL } from './color';

const SHADOW_OFFSET = 3;
const CELL_WIDTH = 30;

class Player {
  private name: string;

  private trace_color: string;
  private color : string;
  private light_color: string;
  private dark_color: string;

  private pos_x: number;
  private pos_y: number;

  constructor(name: string, color: HSL) {
    this.name = name

    this.trace_color = color.to_rgba(0.65);
    this.color = color.to_rgba(0.9);
    this.light_color = color.to_rgba(1);
    this.dark_color = color.adjust_luminosity(0.35).to_rgba(1);

    this.pos_y = this.pos_x = 40;
  }

  public render(ctx: CanvasRenderingContext2D): void {
    this.render_name(ctx)
    this.render_cube(ctx)
  }

  private render_name(ctx: CanvasRenderingContext2D): void {
    ctx.fillStyle = this.color;
    ctx.textAlign = 'center';
    ctx.font = '12px Arial';

    const y_offset = -SHADOW_OFFSET * 2;
    ctx.fillText(this.name, this.pos_x + CELL_WIDTH / 2, this.pos_y + y_offset);
  }

  private render_cube(ctx: CanvasRenderingContext2D): void {
    ctx.fillStyle = this.dark_color;
    ctx.fillRect(this.pos_x, this.pos_y, CELL_WIDTH, CELL_WIDTH);

    ctx.fillStyle = this.light_color;
    ctx.fillRect(this.pos_x - 1, this.pos_y - SHADOW_OFFSET, CELL_WIDTH + 2, CELL_WIDTH);
  }
}

export { Player }