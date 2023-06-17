import { HSL } from './color';
import { Position } from './position';
import { get_index_array_sorted_by_values } from './util';
import { Canvas } from './canvas';

interface BoardgameData {
  rows: number;
  cols: number;
  frame: number;
  names: Array<string>;
  colors: Array<HSL>;
  positions: Array<Position>;
  trails: Array<Array<Position>>;
  regions: Array<Array<Position>>;
  regions_length: Array<number>;
}

class Deserializer {
  private readonly name_size: number = 15;
  private offset: number = 0;
  private data: Uint8Array | undefined = undefined;
  
  private deserialize_value = <T>(size: number = 4): T => {
    const value = this.data![this.offset] as T;
    this.offset += size;
    return value;
  };

  private deserialize_string = (size: number): string => {
    const bytes = this.data!.slice(this.offset, this.offset + size);
    const str = String.fromCharCode(...bytes);
    this.offset += size;
    return str;
  }

  private deserialize_player = (): [string, HSL, Position, Array<Position>, Array<Position>] => {
    const id = this.deserialize_string(this.name_size);
    const px = this.deserialize_value<number>();
    const py = this.deserialize_value<number>();
    const frame_alive = this.deserialize_value<number>();

    const trail_length = this.deserialize_value<number>(1);
    const trail: Array<Position> = new Array<Position>(trail_length);
    for (let i = 0; i != trail_length; i++)
      trail[i] = {x: this.deserialize_value<number>(), y: this.deserialize_value<number>()};

    const region_length = this.deserialize_value<number>();
    const region: Array<Position> = new Array<Position>(region_length);
    for (let i = 0; i != region_length; i++)
      region[i] = {x: this.deserialize_value<number>(), y: this.deserialize_value<number>()};

    return [id, new HSL(217, 0.9, 0.61), { x: px, y: py }, trail, region];
  };

  public deserialize = (data: Uint8Array): BoardgameData => {
    this.offset = 0;
    this.data = data;

    const result: BoardgameData = {
      rows: this.deserialize_value<number>(),
      cols: this.deserialize_value<number>(),
      frame: this.deserialize_value<number>(),
      names: [],
      colors: [],
      positions: [],
      trails: [],
      regions: [],
      regions_length: []
    };


    while (this.offset < data.length) {
      const [name, color, pos, trail, region] = this.deserialize_player();
      result.names.push(name);
      result.colors.push(color);
      result.positions.push(pos);
      result.trails.push(trail);
      result.regions.push(region);
      result.regions_length.push(region.length);
    }

    return result;
  }
}

class BoardGame {
  private readonly name_size: number = 15;
  private readonly shadow_offset: number = 3;

  private deserializer: Deserializer = new Deserializer();

  private canvas: Canvas;
  private scoreboard: HTMLElement;

  private data: BoardgameData | undefined = undefined;

  constructor(canvas: Canvas, scoreboard: HTMLElement) {
    this.canvas = canvas;
    this.scoreboard = scoreboard;
  }

  public render(message: ArrayBuffer): void {
    this.data = this.deserializer.deserialize(new Uint8Array(message));
    this.canvas.resize(this.data.rows, this.data.cols);

    const sorted_idx = get_index_array_sorted_by_values(this.data.regions_length);
    const ctx = this.canvas.ctx;

    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

    this.render_regions();
    this.render_trails();
    this.render_players();

    this.render_scoreboard(sorted_idx);
  }

  private render_regions = (): void => {
    const { ctx, cell_size } = this.canvas;

    for (let i = 0; i != this.data!.regions.length; i++) {
      ctx.fillStyle = this.data!.colors[i].to_rgba(1);

      this.data!.regions[i].forEach(pos => ctx.fillRect(pos.x * cell_size, pos.y * cell_size, cell_size, cell_size));
    }
  };

  private render_trails = (): void => {
    const { ctx, cell_size } = this.canvas;

    for (let i = 0; i != this.data!.trails.length; i++) {
      ctx.fillStyle = this.data!.colors[i].adjust_luminosity(0.74).to_rgba(0.95);

      this.data!.trails[i].forEach(pos => ctx.fillRect(pos.x * cell_size, pos.y * cell_size, cell_size, cell_size));
    }
  };

  private render_players = (): void => {
    const { ctx, cell_size } = this.canvas;

    for (let i = 0; i != this.data!.names.length; i++) {
      const name = this.data!.names[i];
      const hsl = this.data!.colors[i];
      const pos = this.data!.positions[i];

      // --- render name
      let { x, y } = pos;
      x *= cell_size;
      y *= cell_size;

      const color = hsl.to_rgba(0.9);
      const dark_color = hsl.adjust_luminosity(0.35).to_rgba(1);
      const light_color = hsl.to_rgba(1);

      ctx.fillStyle = color;
      ctx.textAlign = 'center';
      ctx.font = 'bold 12px sans-serif';

      const y_offset = -this.shadow_offset * 2;
      ctx.fillText(name, x + cell_size / 2, y + y_offset);

      // --- render cube
      ctx.fillStyle = dark_color;
      ctx.fillRect(x, y, cell_size, cell_size);

      ctx.fillStyle = light_color;
      ctx.fillRect(x + this.shadow_offset, y + this.shadow_offset, cell_size - this.shadow_offset * 2, cell_size - this.shadow_offset * 2);
    }
  };

  private render_scoreboard = (sorted_idx: Array<number>): void => {
    const { names, colors } = this.data!;

    this.scoreboard.replaceChildren(...sorted_idx.reduce((scores, idx): Array<HTMLElement> => {
      const name = names[idx];
      const color = colors[idx];
      const score = this.data!.regions_length[idx] / (this.data!.rows * this.data!.cols) * 100;

      const div = document.createElement('div');
      div.classList.add('score');
      div.style.color = color.to_rgba(1);
      div.style.textShadow = `0 0 ${this.shadow_offset}px ${color.adjust_luminosity(0.5).to_rgba(1)}`;
      
      const span_name = document.createElement('span');
      span_name.innerText = name;

      const span_score = document.createElement('span');
      span_score.textContent = `${score.toFixed(2)}%`;

      div.appendChild(span_name);
      div.appendChild(span_score);

      return [...scores, div];
    }, [] as Array<HTMLElement>));
  };
};

export { BoardGame }