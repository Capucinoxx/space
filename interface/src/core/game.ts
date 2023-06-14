import { HSL } from './color';
import { Position } from './position';
import { get_index_array_sorted_by_values } from './util';

const UUID_SIZE = 15;
const SHADOW_OFFSET = 3;
const CELL_WIDTH = 30;

const render_with_data = (ctx: CanvasRenderingContext2D, scoreboard: HTMLElement, message: ArrayBuffer): void => {
  const data = new Uint8Array(message);
  console.log(data.length);
  let offset = 0;

  const deserialize_value = <T>(size: number = 4): T => {
    const value = data[offset] as T;
    offset += size;
    return value;
  };

  const deserialize_string = (size: number): string => {
    const bytes = data.slice(offset, offset + size);
    const str = String.fromCharCode(...bytes);
    offset += size;
    return str;
  };

  const deserialize_player = (): [string, HSL, Position, Array<Position>, Array<Position>] => {
    const id = deserialize_string(UUID_SIZE);
    const px = deserialize_value<number>() * 30;
    const py = deserialize_value<number>() * 30;
    const frame_alive = deserialize_value<number>();

    const trail_length = deserialize_value<number>(1);
    const trail: Array<Position> = new Array<Position>(trail_length);
    for (let i = 0; i != trail_length; i++)
      trail[i] = {x: deserialize_value<number>() * 30, y: deserialize_value<number>() * 30};

    const region_length = deserialize_value<number>();
    const region: Array<Position> = new Array<Position>(region_length);
    for (let i = 0; i != region_length; i++)
      region[i] = {x: deserialize_value<number>() * 30, y: deserialize_value<number>() * 30};

    return [id, new HSL(217, 0.9, 0.61), { x: px, y: py }, trail, region];
  };

  const rows = deserialize_value<number>();
  const cols = deserialize_value<number>();
  const frame = deserialize_value<number>();

  const names: Array<string> = new Array<string>();
  const colors: Array<HSL> = [];
  const positions: Array<Position> = [];
  const trails: Array<Array<Position>> = [];
  const regions: Array<Array<Position>> = [];
  const regions_length: Array<number> = [];

  while (offset < data.length) {
    const [name, color, pos, trail, region] = deserialize_player();
    names.push(name);
    colors.push(color);
    positions.push(pos);
    trails.push(trail);
    regions.push(region);
    regions_length.push(region.length);

    console.log(offset, data.length);
  }

  const sorted_idx = get_index_array_sorted_by_values(regions_length);
  const n_cells = rows * cols;

  ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

  const scoreboard_childrens: Array<HTMLElement> = [];

  for (let i = 0; i != regions.length; i++)
    render_region(ctx, colors[i], regions[i]);

  for (let i = 0; i != trails.length; i++)
    render_trail(ctx, colors[i], trails[i]);
    
  for (let i = 0; i != names.length; i++) {
    render_player(ctx, names[i], colors[i], positions[i]);

    const score_idx = sorted_idx.indexOf(i);

    scoreboard_childrens.push(
      create_player_score_el(
        names[score_idx], 
        regions_length[score_idx] / n_cells * 100, 
        colors[score_idx].to_rgba(1)));
  }

  scoreboard.replaceChildren(...scoreboard_childrens);
    
};

const render_player = (ctx: CanvasRenderingContext2D, name: string, hsl: HSL, pos: Position): void => {
  console.log(name, hsl, pos);
  // --- render name
  const { x, y } = pos;

  const color = hsl.to_rgba(0.9);
  const dark_color = hsl.adjust_luminosity(0.35).to_rgba(1);
  const light_color = hsl.to_rgba(1);


  ctx.fillStyle = color;
  ctx.textAlign = 'center';
  ctx.font = 'bold 12px sans-serif';

  const y_offset = -SHADOW_OFFSET * 2;
  ctx.fillText(name, x + CELL_WIDTH / 2, y + y_offset);

  // --- render cube
  ctx.fillStyle = dark_color;
  ctx.fillRect(x, y, CELL_WIDTH, CELL_WIDTH);

  ctx.fillStyle = light_color;
  ctx.fillRect(x - 1, y - SHADOW_OFFSET, CELL_WIDTH + 2, CELL_WIDTH);
};

const render_region = (ctx: CanvasRenderingContext2D, hsl: HSL, region: Array<Position>): void => {
  ctx.fillStyle = hsl.to_rgba(1);

  region.forEach(pos => {
    ctx.fillRect(pos.x, pos.y, CELL_WIDTH, CELL_WIDTH);
  });
}

const render_trail = (ctx: CanvasRenderingContext2D, hsl: HSL, trail: Array<Position>): void => {
  ctx.fillStyle = hsl.adjust_luminosity(0.74).to_rgba(0.95);

  trail.forEach(pos => {
    ctx.fillRect(pos.x, pos.y, CELL_WIDTH, CELL_WIDTH);
  });
};

const create_player_score_el = (name: string, score: number, color: string): HTMLElement => {
  const container = document.createElement('div');
  container.classList.add('score');
  container.style.color = color;

  const name_element = document.createElement('span');
  name_element.textContent = name;

  const score_element = document.createElement('span');
  score_element.textContent = `${score.toFixed(2)} %`;

  container.appendChild(name_element);
  container.appendChild(score_element);

  return container;
};

export { render_with_data }