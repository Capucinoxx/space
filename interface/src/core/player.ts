import { HSL } from './color.js';
import { Position } from './position.js';

const SHADOW_OFFSET: number = 3;

const render_player = (ctx: CanvasRenderingContext2D, cell_size: number, name: string, hsl: HSL, pos: Position): void => {
  // --- render name
  let { x, y } = pos;
  x *= cell_size;
  y *= cell_size;

  const color = hsl.to_rgba(0.9);
  const dark_color = hsl.adjust_luminosity(0.35).to_rgba(1);
  const light_color = hsl.to_rgba(1);

  ctx.fillStyle = dark_color;
  ctx.textAlign = 'center';
  ctx.font = 'bold 12px sans-serif';

  const y_offset = -SHADOW_OFFSET * 2;
  ctx.fillText(name, x + cell_size / 2, y + y_offset);

  // --- render cube
  ctx.fillStyle = dark_color;
  ctx.shadowColor = 'rgba(0, 0, 0, 0.5)';
  ctx.shadowBlur = 10;
  ctx.shadowOffsetX = 3;
  ctx.shadowOffsetY = 3;
  ctx.fillRect(x, y, cell_size, cell_size);

  ctx.shadowColor = 'transparent';

  ctx.fillStyle = light_color;
  ctx.fillRect(x - 1, y - SHADOW_OFFSET, cell_size + 2, cell_size);
};

const render_trail = (ctx: CanvasRenderingContext2D, cell_size: number, color: string, pos: Position[]): void => {
  ctx.fillStyle = color;

  pos.forEach(pos => ctx.fillRect(pos.x * cell_size, pos.y * cell_size, cell_size, cell_size));
}

export { render_player, render_trail };