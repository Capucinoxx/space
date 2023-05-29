import { HSL } from './color';
import { Position } from './position';

const SHADOW_OFFSET = 3;
const CELL_WIDTH = 30;

class Player {
  private name: string;

  private trace_color: string;
  private color : string;
  private light_color: string;
  private dark_color: string;

  private current_position: Position;
  private destination_position: Position;

  constructor(name: string, color: HSL) {
    this.name = name

    this.trace_color = color.adjust_luminosity(0.74).to_rgba(0.95);
    this.color = color.to_rgba(0.9);
    this.light_color = color.to_rgba(1);
    this.dark_color = color.adjust_luminosity(0.35).to_rgba(1);

    this.current_position = { x: 30, y: 30 };
    this.destination_position = this.current_position;
  }

  public move(pos: Position): void {
    this.current_position = pos;
  }

  public get position(): Position {
    return this.current_position;
  }

  public set destination(pos: Position) {
    this.destination_position = pos;
  }

  public get destination(): Position {
    return this.destination_position;
  }

  public get delta(): Position {
    return {
      x: this.destination_position.x - this.current_position.x,
      y: this.destination_position.y - this.current_position.y
    }
  }

  public render(ctx: CanvasRenderingContext2D): void {
    this.render_name(ctx)
    this.render_cube(ctx)
  }

  private render_name(ctx: CanvasRenderingContext2D): void {
    const { x, y } = this.current_position;

    ctx.fillStyle = this.color;
    ctx.textAlign = 'center';
    ctx.font = '12px Arial';

    const y_offset = -SHADOW_OFFSET * 2;
    ctx.fillText(this.name, x + CELL_WIDTH / 2, y + y_offset);
  }

  private render_cube(ctx: CanvasRenderingContext2D): void {
    const { x, y } = this.current_position;

    ctx.fillStyle = this.dark_color;
    ctx.fillRect(x, y, CELL_WIDTH, CELL_WIDTH);

    ctx.fillStyle = this.light_color;
    ctx.fillRect(x - 1, y - SHADOW_OFFSET, CELL_WIDTH + 2, CELL_WIDTH);
  }
}

export { Player }