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
 * @property alive             (fr) Nombre de ticks depuis que le joueur est en vie.
 *                             (en) Number of ticks since the player is alive.
 * 
 * @property trail             (fr) Liste des positions des traces du joueur. Si un autre joueur passe sur une de ces
 *                                  positions, il meurt. Set de positions [[x, y], ...].
 *                             (en) List of the player's traces. If another player passes over one of these
 *                                  positions, he dies. Set of positions [[x, y], ...].
 * 
 * @property region            (fr) Liste des positions de la région du joueur. Si un autre joueur passe
 *                                  sur une de ces positions, il retire cette position de la région du joueur.
 *                                  Set de positions [[x, y], ...].
 *                             (en) List of the player's region positions. If another player passes over one of these
 *                                  positions, he removes this position from the player's region.
 *                                  Set of positions [[x, y], ...].
 *
 * @property teleportCooldown  (fr) Nombre de de ticks avant que le joueur puisse réutiliser son action de téléportation.
 *                             (en) Number of ticks before the player can use the teleport action again.
 */
class Player {
  constructor(name, pos, alive, trail, region, teleportCooldown) {
    this.name = name;
    this.pos = pos;
    this.alive = alive;
    this.trail = trail;
    this.region = region;
    this.teleportCooldown = teleportCooldown;
  }

  toString() {
    return `Player(name=${this.name}, pos=${this.pos}, alive=${this.alive}, trail=${this.trail}, region=${this.region}, teleportCooldown=${this.teleportCooldown})`;
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
 * @property players  (fr) Dictionnaire des joueurs. Clé: nom du joueur, Valeur: joueur.
 *                    (en) Dictionary of players. Key: player name, Value: player.
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

    const players = {};
    while (offset < data.byteLength) {
      const nameSize = new Uint32Array(data.slice(offset, offset + 4))[0];
      offset += 4;

      const name = decoder.decode(data.slice(offset, offset + nameSize));
      offset += nameSize;

      offset += 24; // Skip 24 bytes for the player's color

      const [posX, posY, tickAlive] = new Uint32Array(data.slice(offset, offset + 12));
      offset += 12;

      const teleportCooldown = new Uint8Array(data.slice(offset, offset + 1))[0];
      offset += 1;

      const trailLength = new Uint32Array(data.slice(offset, offset + 4))[0];
      offset += 4;

      const trail = new Set();
      for (let i = 0; i < trailLength; i++) {
        const [x, y] = new Uint32Array(data.slice(offset, offset + 8));
        trail.add([x, y]);
        offset += 8;
      }

      const regionLength = new Uint32Array(data.slice(offset, offset + 4))[0];
      offset += 4;

      const region = new Set();
      for (let i = 0; i < regionLength; i++) {
        const [x, y] = new Uint32Array(data.slice(offset, offset + 8));
        region.add([x, y]);
        offset += 8;
      }

      players[name] = new Player(name, [posX, posY], tickAlive, trail, region, teleportCooldown);
    }

    return new GameState(frame, rows, cols, players);
  }

  toString() {
    return `GameState(frame=${this.frame}, rows=${this.rows}, cols=${this.cols}, players=${this.players.map(p => p.toString())})`;
  }
}

export { Player, GameState };
