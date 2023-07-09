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
    const buffer = new ArrayBuffer(9);
    const data_view = new DataView(buffer);
    data_view.setUint8(0, 5, true);
    data_view.setUint32(1, this.x, true);
    data_view.setUint32(5, this.y, true);
    return new Uint8Array(buffer);
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