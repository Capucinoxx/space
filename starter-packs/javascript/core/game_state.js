class Player {
  constructor(name, pos, alive, trail, region) {
    this.name = name;
    this.pos = pos;
    this.alive = alive;
    this.trail = trail;
    this.region = region;
  }

  toString() {
    return `Player(name=${this.name}, pos=${this.pos}, alive=${this.alive}, trail=${this.trail}, region=${this.region})`;
  }
}

class GameState {
  constructor(frame, rows, cols, players, inaccessibleTiles) {
    this.frame = frame;
    this.rows = rows;
    this.cols = cols;
    this.players = players;
    this.inaccessibleTiles = inaccessibleTiles;
  }

  static deserialize(data) {
    let offset = 0;
    const [rows, cols, frame] = new Uint32Array(data.slice(offset, offset + 12));
    offset += 12;

    const inacessibleTilesLength = new Uint32Array(data.slice(offset, offset + 4))[0];
    offset += 4;

    const inacessibleTiles = [];
    for (let i = 0; i < inacessibleTilesLength; i++) {
      const [x, y] = new Uint32Array(data.slice(offset, offset + 8));
      inacessibleTiles.push([x, y]);
      offset += 8;
    }    

    const players = [];
    while (offset < data.byteLength) {
      const nameSize = new Uint32Array(data.slice(offset, offset + 4))[0];
      offset += 4;

      const name = new TextDecoder().decode(data.slice(offset, offset + nameSize));
      offset += nameSize;

      offset += 24; // Skip 24 bytes for the player's color

      const [posX, posY, tickAlive] = new Uint32Array(data.slice(offset, offset + 12));
      offset += 12;

      const trailLength = new Uint32Array(data.slice(offset, offset + 4))[0];
      offset += 4;

      const trail = [];
      for (let i = 0; i < trailLength; i++) {
        const [x, y] = new Uint32Array(data.slice(offset, offset + 8));
        trail.push([x, y]);
        offset += 8;
      }

      const regionLength = new Uint32Array(data.slice(offset, offset + 4))[0];
      offset += 4;

      const region = [];
      for (let i = 0; i < regionLength; i++) {
        const [x, y] = new Uint32Array(data.slice(offset, offset + 8));
        region.push([x, y]);
        offset += 8;
      }

      console.log(`Player ${name} at (${posX}, ${posY})`);

      players.push(new Player(name, [posX, posY], tickAlive, trail, region));
    }

    return new GameState(frame, rows, cols, players, inacessibleTiles);
  }

  toString() {
    return `GameState(frame=${this.frame}, rows=${this.rows}, cols=${this.cols}, players=${this.players.map(p => p.toString())})`;
  }
}

export { Player, GameState };