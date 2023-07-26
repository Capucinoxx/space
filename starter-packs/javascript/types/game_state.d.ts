/**
 * (fr)
 * Représente une position sur le plateau de jeu (x, y).
 * 
 * (en)
 * Represents a position on the game board (x, y).
 */
type Position = [number, number];

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
 *                    (en) Position of the player.
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
declare class Player {
  name: string;
  pos: Position;
  alive: number;
  trail: Set<Position>;
  region: Set<Position>;

  constructor(name: string, pos: Position, alive: number, trail: Position[], region: Position[]);
  toString(): string;
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
declare class GameState {
  frame: number;
  rows: number;
  cols: number;
  players: { [name: string]: Player };

  constructor(frame: number, rows: number, cols: number, players: Player[]);
  static deserialize(data: ArrayBuffer): GameState;
  toString(): string;
}

export { Position, Player, GameState };
