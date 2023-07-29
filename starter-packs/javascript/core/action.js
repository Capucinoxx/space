/**
 * (fr)
 * Crée une action de déplacement dans la direction `value`. Des instances de `Direction` sont
 * déjà définies pour les directions `UP`, `DOWN`, `LEFT` et `RIGHT`. Vous ne devriez pas avoir
 * à créer d'instance de `Direction` vous-même.
 * 
 * (en)
 * Creates a movement action in the direction `value`. Instances of `Direction` are already
 * defined for the directions `UP`, `DOWN`, `LEFT` and `RIGHT`. You should not have to create
 * an instance of `Direction` yourself.
 * 
 * @param value The direction to take.
 */
class Direction {
  static UP = new Direction(0);
  static DOWN = new Direction(1);
  static LEFT = new Direction(2);
  static RIGHT = new Direction(3);

  constructor(value) {
    this.value = value;
  }

  serialize() {
    const buffer = new Uint8Array(1);
    const data_view = new DataView(buffer.buffer);
    data_view.setUint8(0, this.value);
    return buffer;
  }
}
 
/**
 * (fr)
 * Crée une action de téléportation vers la position (x, y).
 * 
 * /!\ Si la position (x, y) n'est pas dans votre région, aucune action ne sera prise 
 *     pendant le tick.
 * 
 * (en)
 * Creates a teleportation action to the position (x, y).
 * 
 * /!\ If the position (x, y) is not in your region, no ation will be taken during the tick.
 * 
 * @param x The x-coordinate of the teleportation.
 * @param y The y-coordinate of the teleportation. 
 */
class Teleport {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
  
  serialize() {
    const buffer = new ArrayBuffer(9);
    const data_view = new DataView(buffer);
    data_view.setUint8(0, 5);
    data_view.setUint32(1, this.x, true);
    data_view.setUint32(5, this.y, true);
    return new Uint8Array(buffer);
  }
}
  
/**
 * (fr)
 * Crée une action de type `Pattern`. Cette action permet de définir une liste d'actions 
 * à effectuer lorsque votre agent est déconnecté du serveur.
 * 
 * /!\ Si vous ne spécifiez pas de pattern de déconnexion lorsque vous vous déconnectez,
 *     votre agent prendra toujours l'action d'aller vers le bas.
 * 
 * /!\ Si dans votre pattern de déconnexion vous spécifiez un autre pattern de déconnexion,
 * 	   une erreur sera levée côté serveur et votre agent ne fera pas d'action pendant le tick.
 * 
 * (en)
 * Creates a `Pattern` action. This action allows you to define a list of actions to perform
 * when your agent is disconnected from the server.
 * 
 * /!\ If you do not specify a disconnection pattern, when you disconnect, your agent will
 * 	 always take the action of going down.
 * 
 * /!\ If in your disconnection pattern you specify another disconnection pattern, an error
 * 	   will be raised on the server side and your agent will not take any action during the tick.
 * 
 * @param actions The actions to perform.
 */
class Pattern {
  constructor(actions) {
    this.actions = actions;
  }
  
  serialize() {
    const serialized_actions = this.actions.reduce((acc, action) => {
      const serialized_action = action.serialize();
      const combined = new Uint8Array(acc.length + serialized_action.length);
      combined.set(acc);
      combined.set(serialized_action, acc.length);
      return combined;
    }, new Uint8Array());

    const buffer = new Uint8Array(serialized_actions.length + 1);
    buffer[0] = 7;
    buffer.set(serialized_actions, 1);

    return buffer;
  }
}
 
/**
 * (fr)
 * Wrapper pour les actions de type `Teleport`, `Direction` et `Pattern`.
 * 
 * (en)
 * Wrapper for actions of type `Teleport`, `Direction` and `Pattern`.
 * 
 * 
 * @param action The action to perform.
 * 
 * @example
 * ```js
 * // (fr) Téléporte l'agent vers la position (0, 0).
 * // (en) Teleports the agent to the position (0, 0).
 * 
 * new Action(new Teleport(0, 0));
 * ```
 * 
 * @example
 * ```js
 * // (fr) Déplace l'agent vers le haut.
 * // (en) Moves the agent up.
 * 
 * new Action(Direction.UP);
 * ```
 * 
 * @example
 * ```js
 * // (fr) Pattern de déconnexion qui téléporte l'agent vers la position (5, 5) et bouge 10 fois en 
 * //      alternant entre le haut et le bas.
 * // (en) Disconnection pattern that teleports the agent to the position (5, 5) and moves 10 times
 * //      alternating between up and down.
 * 
 * new Action(new Pattern([
 * 	 new Teleport(5, 5),
 * 	 ...Array(10).fill().map((_, i) => i % 2 === 0 ? Direction.UP : Direction.DOWN)
 * ]));
 * ```
 */
class Action {
  constructor(actionType) {
    this.actionType = actionType;
  }
  
  serialize() {
    return this.actionType.serialize();
  }
}

export { Action, Direction, Pattern, Teleport };