import { Player } from './core/player'
import { HSL } from './core/color'
import { Position } from './core/position'
import { WebsocketService } from './service/websocket';

const ws = new WebsocketService('ws://localhost:8030/spectate');
ws.subscribe((message: string) => {
  console.log(message);
});

const board = document.getElementById('game-canvas') as HTMLCanvasElement;
const ctx = board.getContext('2d') as CanvasRenderingContext2D;


const player = new Player("Test", new HSL(217, 0.90, 0.61));
player.render(ctx);

function animate() {
  ctx.clearRect(0, 0, board.width, board.height);
  player.render(ctx);

  requestAnimationFrame(animate);
}

function moveRandomly(position: Position): Position {
  const directions = ["x+", "x-", "y+", "y-"];
  const randomDirection = directions[Math.floor(Math.random() * directions.length)];

  switch (randomDirection) {
    case "x+": position.x += 30; break;
    case "x-": position.x -= 30; break;
    case "y+": position.y += 30; break;
    case "y-": position.y -= 30; break;
    default: break;
  }

  return position;
}

setInterval(() => {
  player.destination = moveRandomly(player.destination)
  player.move(player.destination)
}, 300);

animate();
