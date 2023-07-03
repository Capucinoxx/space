type Position = [number, number];

interface Player {
  name: string
  position: Position
  alive: number
  trail: Position[]
  region: Position[]
}

interface GameState {
  rows: number
  cols: number
  tick: number
  players: Player[]
  inacessible_tiles: Position[]
}

export { Position, Player, GameState };