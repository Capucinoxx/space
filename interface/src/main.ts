import { BoardGame, Game } from './core/game.js'
import { WebsocketService } from './service/websocket.js';
import { HSL } from './core/color.js';
import { send } from './service/api.js';
import { Canvas } from './core/canvas.js';

// handle register modal events
// ==================================================
// const register_modal = document.getElementById('register-modal') as HTMLElement;
// const register_modal_close = document.getElementById('register-modal-close') as HTMLElement;
// const register_modal_open = document.getElementById('register-modal-open') as HTMLElement;
// const register_form = register_modal.querySelector('form') as HTMLFormElement;

// register_modal_open.onclick = () =>  { register_modal.style.display = 'block'; };
// register_modal_close.onclick = () => { register_modal.style.display = 'none'; };

// register_form.onsubmit = (e: SubmitEvent) => {
//     e.preventDefault();
//     const data = new FormData(register_form);
//     const hsl = HSL.from_hex(data.get('color') as string).to_array();
//     data.set('color', hsl.join(' '));

//     console.log(data);

//     send('http://localhost:8080/subscribe', 
//         { method: 'POST', body: data },
//         (response: ArrayBuffer) => {
//             alert(`secret: ${new TextDecoder().decode(response)}`);
//         });
// };

// Game management
// ==================================================
const board = document.getElementById('game-canvas') as HTMLCanvasElement;
const scoreboard = document.getElementById('posessions') as HTMLElement;
const vacant_tile = document.getElementById('vacant-tile') as HTMLElement;

const canvas = new Canvas(board);
const game = new BoardGame(canvas, scoreboard, vacant_tile);
const game_nav = document.getElementById('game-nav') as HTMLElement;

const ws = new WebsocketService();

new Game(game_nav, (channel: string) => {
    ws.connect(`ws://localhost:8080/${channel}/spectate`);
});
ws.subscribe((data: ArrayBuffer) => game.render(data));


