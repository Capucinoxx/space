/**
 * (fr)
 * Représente un joueur.
 * 
 * (en)
 * Represents a player.
 * 
 * @property name     (fr) Nom du joueur.
 *                    (en) Name of the player.
 * 
 * @property position (fr) Position du joueur. [x, y]
 *                    (en) Position of the player. [x, y]
 * 
 * @property alive    (fr) Nombre de ticks depuis lequel le joueur est en vie.
 *                    (en) Number of ticks since the player is alive.
 * 
 * @property trail    (fr) Liste des traces du joueur. Si un autre joueur passe sur une de ces
 *                         positions, il meurt. Tableau de positions [[x, y], ...].
 *                    (en) List of the player's traces. If another player passes over one of these
 *                         positions, he dies. Array of positions [[x, y], ...].
 * 
 * @property region   (fr) Liste des positions de la région du joueur. Si un autre joueur passe
 *                         sur une de ces positions, il retire cette position de la région du joueur.
 *                         Tableau de positions [[x, y], ...].
 *                    (en) List of the player's region positions. If another player passes over one of these
 *                         positions, he removes this position from the player's region.
 *                         Array of positions [[x, y], ...].
 */
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

/**
 * (fr)
 * Représente l'état du jeu à un instant donné.
 * 
 * (en)
 * Represents the state of the game at a given time.
 * 
 * @property rows     (fr) Nombre de lignes du plateau de jeu.
 *                    (en) Number of rows of the game board.
 * 
 * @property cols     (fr) Nombre de colonnes du plateau de jeu.
 *                    (en) Number of columns of the game board.
 * 
 * @property tick     (fr) Numéro du tick actuel.
 *                    (en) Number of the current tick.
 * 
 * @property players  (fr) Liste des joueurs.
 *                    (en) List of players.
 */
class GameState {
  constructor(frame, rows, cols, players) {
    this.frame = frame;
    this.rows = rows;
    this.cols = cols;
    this.players = players;
  }

  static deserialize(data) {
    let offset = 0;
    const [rows, cols, frame] = new Uint32Array(data.slice(offset, offset + 12));
    offset += 12;

    const decoder = new TextDecoder('unicode-1-1-utf-8');

    const players = [];
    while (offset < data.byteLength) {
      const nameSize = new Uint32Array(data.slice(offset, offset + 4))[0];
      offset += 4;

      const name = decoder.decode(data.slice(offset, offset + nameSize));
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

    return new GameState(frame, rows, cols, players);
  }

  toString() {
    return `GameState(frame=${this.frame}, rows=${this.rows}, cols=${this.cols}, players=${this.players.map(p => p.toString())})`;
  }
}

export { Player, GameState };
