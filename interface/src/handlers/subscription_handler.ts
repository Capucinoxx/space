const handling_subscription = () => {
	const modal = document.getElementById('subscription-modal') as HTMLElement;
	const open_btn = document.getElementById('open-subscription-modal') as HTMLElement;
	const close_btn = document.getElementById('close-subscription-modal') as HTMLElement;

	const modal_container = modal.querySelector('.modal') as HTMLElement;
	const modal_svg = modal.querySelector('rect') as SVGRectElement;	

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
		})
	};

	close_btn.onclick = () => {
		modal.classList.remove('open');
	};
}

export { handling_subscription }