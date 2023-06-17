import { render_with_data } from './core/game'
import { WebsocketService } from './service/websocket';
import { HSL } from './core/color';
import { send } from './service/api';

// handle register modal events
// ==================================================
const register_modal = document.getElementById('register-modal') as HTMLElement;
const register_modal_close = document.getElementById('register-modal-close') as HTMLElement;
const register_modal_open = document.getElementById('register-modal-open') as HTMLElement;
const register_form = register_modal.querySelector('form') as HTMLFormElement;

register_modal_open.onclick = () =>  { register_modal.style.display = 'block'; };
register_modal_close.onclick = () => { register_modal.style.display = 'none'; };

register_form.onsubmit = (e: SubmitEvent) => {
    e.preventDefault();
    const data = new FormData(register_form);
    const hsl = HSL.from_hex(data.get('color') as string).to_array();
    data.set('color', hsl.join(' '));

    console.log(data);

    send('http://localhost:8080/subscribe', 
        { method: 'POST', body: data },
        (response: ArrayBuffer) => {
            alert(`secret: ${new TextDecoder().decode(response)}`);
        });
};

// Game management
// ==================================================
const board = document.getElementById('game-canvas') as HTMLCanvasElement;
const scoreboard = document.getElementById('scores') as HTMLElement;
const ctx = board.getContext('2d') as CanvasRenderingContext2D;

board.width = board.offsetWidth;
board.height = board.offsetHeight;


const ws = new WebsocketService('ws://localhost:8080/spectate');
ws.subscribe((data: ArrayBuffer) => { render_with_data(ctx, scoreboard, data); });