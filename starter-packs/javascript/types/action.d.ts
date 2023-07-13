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
declare class Direction {
	static UP: Direction;
	static DOWN: Direction;
	static LEFT: Direction;
	static RIGHT: Direction;

	constructor(value: number);
	serialize(): Uint8Array;
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
declare class Teleport {
	constructor(x: number, y: number);
	serialize(): Uint8Array;
}

/**
 * (fr)
 * Crée une action de type `Pattern`. Cette action permet de définir une liste d'actions 
 * à effectuer lorsque votre agent est déconnecté du serveur.
 * 
 * /!\ Si vous ne spécifiez pas de pattern de déconnexion, lorsque vous vous déconnectez,
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
declare class Pattern {
	constructor(actions: Action[]);
	serialize(): Uint8Array;
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
declare class Action {
	constructor(action: Teleport | Direction | Pattern);
	serialize(): Uint8Array;
}
