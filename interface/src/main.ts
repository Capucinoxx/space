import { render_with_data } from './core/game'
import { WebsocketService } from './service/websocket';

// handle register modal events
// ==================================================
const register_modal = document.getElementById('register-modal') as HTMLElement;
const register_modal_close = document.getElementById('register-modal-close') as HTMLElement;
const register_modal_open = document.getElementById('register-modal-open') as HTMLElement;

register_modal_open.onclick = () =>  { register_modal.style.display = 'block'; };
register_modal_close.onclick = () => { register_modal.style.display = 'none'; };

// Game management
// ==================================================
const board = document.getElementById('game-canvas') as HTMLCanvasElement;
const scoreboard = document.getElementById('scores') as HTMLElement;
const ctx = board.getContext('2d') as CanvasRenderingContext2D;

board.width = board.offsetWidth;
board.height = board.offsetHeight;


const ws = new WebsocketService('ws://localhost:8080/spectate');
ws.subscribe((data: ArrayBuffer) => { render_with_data(ctx, scoreboard, data); });