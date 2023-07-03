import { Action, Direction, Pattern } from '../core/action.js';

class MyBot {
  constructor() {
    this.name = "test ";
    this.first_turn = true;
  }

  tick(state) {
    if (this.first_turn) {
      this.first_turn = false;
      return new Action(new Pattern([Direction.UP, Direction.RIGHT]));
    }

    const directions = [Direction.UP, Direction.DOWN, Direction.LEFT, Direction.RIGHT];

    return new Action(directions[Math.floor(Math.random() * directions.length)]);
  }
};

export { MyBot };