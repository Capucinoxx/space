import { Position } from './position.js';

interface Rectangle {
  pos: Position,
  width: number,
  height: number
};


// { pos: { x: 0, y: 0 }, width: 0, height: 0 }
const orange_teleport = [
  { pos: { x: 2, y: 8 }, width: 1, height: 5 },
  { pos: { x: 3, y: 6 }, width: 1, height: 13 },
  { pos: { x: 4, y: 4 }, width: 1, height: 5 },
  { pos: { x: 4, y: 12 }, width: 1, height: 10 },
  { pos: { x: 5, y: 3 }, width: 1, height: 8 },
  { pos: { x: 5, y: 19 }, width: 1, height: 5 },
  { pos: { x: 6, y: 2 }, width: 1, height: 5 },
  { pos: { x: 6, y: 11 }, width: 1, height: 1 },
  { pos: { x: 6, y: 21 }, width: 1, height: 5 },
  { pos: { x: 7, y: 1 }, width: 4, height: 3 },
  { pos: { x: 7, y: 22 }, width: 1, height: 6 },
  { pos: { x: 8, y: 26 }, width: 2, height: 4 },
  { pos: { x: 9, y: 27 }, width: 4, height: 1 },
  { pos: { x: 13, y: 26 }, width: 2, height: 1 },
  { pos: { x: 11, y: 2 }, width: 1, height: 3 },
  { pos: { x: 12, y: 3 }, width: 1, height: 3 },
  { pos: { x: 13, y: 4 }, width: 1, height: 3 },
  { pos: { x: 13, y: 2 }, width: 1, height: 1 },
  { pos: { x: 14, y: 5 }, width: 1, height: 3 },
  { pos: { x: 15, y: 7 }, width: 1, height: 4 },
  { pos: { x: 16, y: 5 }, width: 1, height: 9 },
  { pos: { x: 17, y: 9 }, width: 1, height: 19 },
  { pos: { x: 18, y: 12 }, width: 1, height: 15 },
  { pos: { x: 19, y: 16 }, width: 1, height: 8 },
  { pos: { x: 16, y: 26 }, width: 1, height: 3 },
  { pos: { x: 15, y: 27 }, width: 1, height: 3 },
  { pos: { x: 14, y: 28 }, width: 1, height: 2 },
  { pos: { x: 10, y: 29 }, width: 4, height: 1 }
] as Array<Rectangle>;

const draw_teleport = (ctx: CanvasRenderingContext2D, color: 'orange' | 'blue', pos: Position) => {
  ctx.fillStyle = '#FF6900';
  orange_teleport.forEach((section) => {
    ctx.fillRect(section.pos.x, section.pos.y, section.width, section.height);
  });
};

export { draw_teleport };

