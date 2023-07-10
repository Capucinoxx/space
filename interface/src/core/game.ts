import { HSL } from './color.js';
import { Position } from './position.js';
import { get_index_array_sorted_by_values } from './util.js';
import { Canvas } from './canvas.js';

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

  inacessible_tile: Array<Position>;
}

class Deserializer {
  private offset: number = 0;
  private data: Uint8Array | undefined = undefined;
  private decoder = new TextDecoder('unicode-1-1-utf-8');
  

  private deserialize_int = (size: number = 4): number => {
    const bytes = new Uint8Array(this.data!.slice(this.offset, this.offset + size));
    const value = new Int32Array(bytes.buffer)[0];
    this.offset += size;
    return value;
  }

  private deserialize_float = (size: number = 8): number => {
    const bytes = new Uint8Array(this.data!.slice(this.offset, this.offset + size));
    const value = new Float64Array(bytes.buffer)[0];
    this.offset += size;
    return value;
  }

  private deserialize_string = (size: number): string => {
    const bytes = new Uint8Array(this.data!.slice(this.offset, this.offset + size));
    const str = this.decoder.decode(bytes);
    this.offset += size;
    return str;
  }

  private deserialize_player = (): [string, HSL, Position, Array<Position>, Array<Position>] => {
    const name_size = this.deserialize_int();

    const name = this.deserialize_string(name_size);
    const color_h = this.deserialize_float();
    const color_s = this.deserialize_float();
    const color_l = this.deserialize_float();
    const px = this.deserialize_int();
    const py = this.deserialize_int();
    const tick_alive = this.deserialize_int();

    const trail_length = this.deserialize_int();
    const trail: Position[] = new Array<Position>(trail_length);
    for (let i = 0; i != trail_length; i++)
      trail[i] = {x: this.deserialize_int(), y: this.deserialize_int()};

    const region_length = this.deserialize_int();
    const region: Position[] = new Array<Position>(region_length);
    for (let i = 0; i != region_length; i++)
      region[i] = {x: this.deserialize_int(), y: this.deserialize_int()};

    return [name, new HSL(color_l, color_s, color_h), { x: px, y: py }, trail, region];
  };

  public deserialize = (data: Uint8Array): BoardgameData => {
    this.offset = 0;
    this.data = data;

    const result: BoardgameData = {
      rows: this.deserialize_int(),
      cols: this.deserialize_int(),
      frame: this.deserialize_int(),
      names: [],
      colors: [],
      positions: [],
      trails: [],
      regions: [],
      regions_length: [],
      inacessible_tile: []
    };

    const inacessible_tile_length = this.deserialize_int();
    for (let i = 0; i != inacessible_tile_length; i++)
      result.inacessible_tile.push({x: this.deserialize_int(), y: this.deserialize_int()});

    const length = data.length;
    while (this.offset < length) {      
      const [name, color, pos, trail, region] = this.deserialize_player();
      result.names.push(name.trim());
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
  private readonly shadow_offset: number = 3;

  private deserializer: Deserializer = new Deserializer();

  private canvas: Canvas;
  private scoreboard: HTMLElement;
  private vancant_tile: HTMLElement;

  private data: BoardgameData | undefined = undefined;

  constructor(canvas: Canvas, scoreboard: HTMLElement, vancant_tile: HTMLElement) {
    this.canvas = canvas;
    this.scoreboard = scoreboard;
    this.vancant_tile = vancant_tile;
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
      ctx.fillStyle = this.data!.colors[i].adjust_luminosity(0.74).to_rgba(0.8);

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

      ctx.fillStyle = dark_color;
      ctx.textAlign = 'center';
      ctx.font = 'bold 12px sans-serif';

      const y_offset = -this.shadow_offset * 2;
      ctx.fillText(name, x + cell_size / 2, y + y_offset);

      // --- render cube
      ctx.fillStyle = dark_color;
      ctx.shadowColor = 'rgba(0, 0, 0, 0.5)';
      ctx.shadowBlur = 10;
      ctx.shadowOffsetX = 3;
      ctx.shadowOffsetY = 3;
      ctx.fillRect(x, y, cell_size, cell_size);

      ctx.shadowColor = 'transparent';

      ctx.fillStyle = light_color;
      ctx.fillRect(x - 1, y - this.shadow_offset, cell_size + 2, cell_size);
    }
  };

  private render_scoreboard = (sorted_idx: Array<number>): void => {
    const { names, colors } = this.data!;

    let occupied_tiles = 0;

    this.scoreboard.replaceChildren(...sorted_idx.reduce((scores, idx): Array<HTMLElement> => {
      const name = names[idx];
      const color = colors[idx];
      const score = this.data!.regions_length[idx] / (this.data!.rows * this.data!.cols) * 100;
      occupied_tiles += this.data!.regions_length[idx];

      const div = document.createElement('div');
      div.classList.add('posession');
      div.style.color = color.to_rgba(1);
      
      const span_name = document.createElement('span');
      span_name.innerText = name;

      const span_score = document.createElement('span');
      span_score.textContent = `${score.toFixed(2)}%`;

      div.appendChild(span_name);
      div.appendChild(span_score);

      return [...scores, div];
    }, [] as Array<HTMLElement>));

    this.vancant_tile.textContent = `${(this.data!.rows * this.data!.cols) - occupied_tiles}`;
  };

  private render_frame = (frame: number): void => {

  }
};

class Game {
  private pos: number = 0;
  private width: number = 0;
  private active_channel_element: HTMLElement | undefined = undefined;
  private line: HTMLElement | undefined = undefined;
  private cb: (channel: string) => void;

  constructor(container: HTMLElement, cb: (channel: string) => void) {
    this.cb = cb;
    this.add_line(container);
    this.active_channel_element = this.get_default_channel(container);

    console.log(this.active_channel_element);
    this.add_event_listener(container);
  }

  private add_line = (container: HTMLElement): void => {
    this.line = document.createElement('div');
    this.line.classList.add('line');
    container.appendChild(this.line);
  };

  private get_default_channel(container: HTMLElement): HTMLElement | undefined {
    const active = (container.querySelector('.active') as HTMLElement | undefined);
    if (active === undefined)
      return undefined;
    
    this.pos = active.offsetLeft;
    this.width = active.offsetWidth;
    this.line!.style.left = `${this.pos}px`;
    this.line!.style.width = `${this.width}px`;

    this.cb(active.querySelector('div')?.dataset.channel!);

    return active;
  }

  private add_event_listener = (container: HTMLElement): void => {
    const channels = container.querySelectorAll('ul li div');
    channels.forEach((channel: Element) => {
      channel.addEventListener('click', this.handle_tab_change);
    });
  };

  private handle_tab_change = (e: Event): void => {
    e.preventDefault();

    const target = e.target as HTMLElement;
    const parent = target.parentNode as HTMLElement;

    if (!parent.classList.contains('active')) {
      this.active_channel_element!.classList.remove('active');
      parent.classList.add('active');
      this.active_channel_element = parent;

      this.animate_line(target);

      const channel = target.dataset.channel;
      if (channel !== undefined)
        this.cb(channel);
    }
  };

  private animate_line(target: HTMLElement): void {
    const position = target.offsetLeft;
    const width = target.offsetWidth;

    this.line!.style.width = this.width + 'px';
    this.line!.style.left = this.pos + 'px';

    this.line!.style.transition = 'none';
    void this.line!.offsetWidth;

    this.line!.style.transition = 'width 300ms ease, left 300ms ease';

    this.line!.style.width = width + 'px';
    this.line!.style.left = position + 'px';

    target.classList.add('active');

    const transitionEndHandler = () => {
      this.line!.removeEventListener('transitionend', transitionEndHandler);
    };
    this.line!.addEventListener('transitionend', transitionEndHandler);
    
    this.pos = position;
    this.width = width;
  }
};

export { BoardGame, Game };