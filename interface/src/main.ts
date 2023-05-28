import { Player } from './core/player'
import { HSL } from './core/color'

const board = document.getElementById('game-canvas') as HTMLCanvasElement;
const ctx = board.getContext('2d') as CanvasRenderingContext2D;

new Player("Test", new HSL(217, 0.90, 0.61)).render(ctx);