import { Action, Direction, Pattern, Teleport } from '../core/action.js';
import { GameState } from '../core/game_state.js';

/**
 * (fr)
 * Cette classe représente votre bot. Vous pouvez y définir des attributs et des méthodes qui
 * seront conservés entre chaque appel à la méthode `tick`.
 * 
 * (en)
 * This class represents your bot. You can define attributes and methods that will be kept
 * between each call to the `tick` method.
 */
class MyBot {
  constructor() {
    this.name = "name_of_my_super_cool_bot";
    this.first_turn = true;
  }

  random_action() {
    const directions = [Direction.UP, Direction.DOWN, Direction.LEFT, Direction.RIGHT];
    return new Action(directions[Math.floor(Math.random() * directions.length)]);
  }

  /**
   * (fr)
   * Cette méthode est appelée à chaque tick de jeu. Vous pouvez y définir le comportement de
   * votre bot. Elle doit retourner une instance de `Action` qui sera exécutée par le serveur.
   * 
   * (en)
   * This method is called every game tick. You can define the behavior of your bot. It must
   * return an instance of `Action` that will be executed by the server.
   * 
   * @param {GameState} state 
   * @returns `Action` An action to execute.
   */
  tick(state) {
    if (this.first_turn) {
      this.first_turn = false;
      return new Action(new Pattern([Direction.UP, Direction.RIGHT]));
    }

    return this.random_action();
  }
};

export { MyBot };