import { Canvas } from './canvas.js';
import { Position } from './position.js';

interface Rectangle {
  pos: Position,
  width: number,
  height: number
};

const death = [
  { pos: { x: 0, y: 1 }, width: 3, height: 2 },
  { pos: { x: 1, y: 0 }, width: 2, height: 1 },
  { pos: { x: 2, y: 3 }, width: 1, height: 1 },
  { pos: { x: 3, y: 2 }, width: 1, height: 1 },

  { pos: { x: 5, y: 3 }, width: 5, height: 3 },
  { pos: { x: 4, y: 4 }, width: 2, height: 3 },
  { pos: { x: 3, y: 5 }, width: 1, height: 4 },
  { pos: { x: 3, y: 10 }, width: 9, height: 1 },
  { pos: { x: 4, y: 9 }, width: 3, height: 1 },
  { pos: { x: 8, y: 9 }, width: 3, height: 1 },
  { pos: { x: 6, y: 8 }, width: 3, height: 1 },
  { pos: { x: 4, y: 8 }, width: 1, height: 1 },
  { pos: { x: 7, y: 6 }, width: 1, height: 2 },
  { pos: { x: 7, y: 4 }, width: 1, height: 3 },
  { pos: { x: 11, y: 5 }, width: 1, height: 4 },
  { pos: { x: 10, y: 8 }, width: 1, height: 1 },
  { pos: { x: 5, y: 11 }, width: 5, height: 1 },
  { pos: { x: 5, y: 12 }, width: 1, height: 1 },
  { pos: { x: 7, y: 12 }, width: 1, height: 1 },
  { pos: { x: 9, y: 12 }, width: 1, height: 1 },

  { pos: { x: 12, y: 0 }, width: 3, height: 3 },
  { pos: { x: 14, y: 1 }, width: 1, height: 2 },
  { pos: { x: 11, y: 2 }, width: 1, height: 1 },
  { pos: { x: 12, y: 3 }, width: 1, height: 1 },

  { pos: { x: 0, y: 13 }, width: 1, height: 2 },
  { pos: { x: 1, y: 13 }, width: 2, height: 3 },
  { pos: { x: 2, y: 12 }, width: 1, height: 1 },
  { pos: { x: 3, y: 13 }, width: 1, height: 1 },

  { pos: { x: 14, y: 13 }, width: 1, height: 2 },
  { pos: { x: 12, y: 13 }, width: 2, height: 3 },
  { pos: { x: 11, y: 13 }, width: 1, height: 1 },
  { pos: { x: 13, y: 12 }, width: 1, height: 1 }
] as Array<Rectangle>;

const draw_death_icon = (ctx: CanvasRenderingContext2D, color: string, pos: Position, cell_width: number): void => {
  let { x: px , y: py } = pos;
  px = px * cell_width;
  py = py * cell_width;

  ctx.fillStyle = color;
  death.forEach(rect => {
    ctx.fillRect(px + rect.pos.x, py + rect.pos.y, rect.width, rect.height);
  });
}

export { draw_death_icon };