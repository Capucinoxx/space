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
    buffer[0] = this.value;
    return buffer;
  }
}
  
class Teleport {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }
  
  serialize() {
    const buffer = new Uint8Array(9);
    buffer[0] = 5;
    new DataView(buffer.buffer, 1, 4).setInt32(0, this.x, true);
    new DataView(buffer.buffer, 5, 4).setInt32(0, this.y, true);
    return buffer;
  }
}
  
class Pattern {
  constructor(actions) {
    this.actions = actions;
  }
  
  serialize() {
    const serializedActions = Uint8Array.from(
      this.actions.flatMap(action => action.serialize())
    );
    const buffer = new Uint8Array(serializedActions.length + 1);
    buffer[0] = 7;
    buffer.set(serializedActions, 1);
    return buffer;
  }
}
  
class Action {
  constructor(actionType) {
    this.actionType = actionType;
  }
  
  serialize() {
    return this.actionType.serialize();
  }
}

export { Action, Direction, Pattern, Teleport };