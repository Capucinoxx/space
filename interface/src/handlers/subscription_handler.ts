import { HSL } from '../core/color.js';
import { send } from '../service/api.js';
import { render_player, render_trail } from '../core/player.js';

const handling_subscription = () => {
	const modal = document.getElementById('subscription-modal') as HTMLElement;
	const open_btn = document.getElementById('open-subscription-modal') as HTMLElement;
	const close_btn = document.getElementById('close-subscription-modal') as HTMLElement;
	const submit_btn = document.getElementById('submit-subscription-modal') as HTMLButtonElement;

	const modal_container = modal.querySelector('.modal') as HTMLElement;
	const modal_svg = modal.querySelector('rect') as SVGRectElement;	

	const name_input = document.querySelector('input[id="username"]') as HTMLInputElement;

	const canvas_preview = document.getElementById('bot-preview') as HTMLCanvasElement;
	const ctx = canvas_preview.getContext('2d') as CanvasRenderingContext2D;

	submit_btn.onclick = () => {
		const name = name_input.value;
		if (name.length == 0 || name.length > 20) return;

		const color = HSL.from_name(name).to_array();

		const data = new FormData();
		data.set('name', name);
		data.set('color', color.join(' '));

		send('http://localhost:8080/subscribe',
			{ method: 'POST', body: data },
			(response: ArrayBuffer) => {
				alert(`secret: ${new TextDecoder().decode(response)}`);
			});
	};
	

	name_input.addEventListener('keyup', (e: Event) => {
		const name = (e.target as HTMLInputElement).value;
		bot_preview(canvas_preview, ctx, name);
	});

	open_btn.onclick = () => {
		modal.classList.add('open');
		
		const rect = modal_svg.getBoundingClientRect();
		const length = (rect.width + rect.height) * 2;

		modal_svg.style.strokeDasharray = `${length}`;
		modal_svg.style.strokeDashoffset = `${length}`;

		modal_svg.animate([
			{ strokeDashoffset: length },
			{ strokeDashoffset: 0 }
		], {
			duration: 500,
			delay: 300,
			easing: 'cubic-bezier(0.165, 0.84, 0.44, 1)',
			fill: 'forwards'
		});

		bot_preview(canvas_preview, ctx, name_input.value);
	};

	close_btn.onclick = () => {
		modal.classList.remove('open');
	};
}

const bot_preview = (canvas: HTMLCanvasElement,ctx: CanvasRenderingContext2D, name: string) => {
	ctx.clearRect(0, 0, canvas.width, canvas.height);

	const hsl = HSL.from_name(name);

	render_player(ctx, 20, name, hsl, { x: 7, y: 2 });
	render_trail(ctx, 20, hsl.adjust_luminosity(0.74).to_rgba(0.8), [{ x: 7, y: 3 }, { x: 7, y: 4 }, { x: 8, y: 4 }, { x: 9, y: 4 }, { x: 9, y: 3 }, { x: 10, y: 3 }]);
};

export { handling_subscription }