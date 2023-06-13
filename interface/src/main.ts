import { render_with_data } from './core/game'
import { WebsocketService } from './service/websocket';

const board = document.getElementById('game-canvas') as HTMLCanvasElement;
const scoreboard = document.getElementById('scoreboard') as HTMLElement;
const ctx = board.getContext('2d') as CanvasRenderingContext2D;

board.width = board.offsetWidth;
board.height = board.offsetHeight;


const ws = new WebsocketService('ws://localhost:8080/spectate');
ws.subscribe((data: ArrayBuffer) => { render_with_data(ctx, scoreboard, data); });