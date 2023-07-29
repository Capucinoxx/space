from enum import IntEnum
from dataclasses import dataclass
from typing import List, Union

import struct

class Direction(IntEnum):
    """
    (fr)
    Représente une direcion de déplacement.

    (en)
    Represents a movement direction.
    """

    UP = 0
    DOWN = 1
    LEFT = 2
    RIGHT = 3

    def serialize(self) -> bytes:
        return bytes([self.value])


@dataclass
class Teleport:
    """
    (fr)
    Crée une action de téléportation vers la position (x, y).

    /!\ Si la position (x, y) n'est pas dans votre région, aucune action ne sera prise pendant le tick.

    (en)
    Creates a teleportation action to the position (x, y).

    /!\ If the position (x, y) is not in your region, no action will be taken during the tick.

    Attributes:
        x (int):    (fr) L'abscisse de la téléportation.
                    (en) The x-coordinate of the teleportation.

        y (int):    (fr) L'ordonnée de la téléportation.
                    (en) The y-coordinate of the teleportation.
    """

    x: int
    y: int

    def serialize(self) -> bytes:
        return b'\x05' + struct.pack('<ii', self.x, self.y)


@dataclass
class Pattern:
    """
    (fr)
    Crée une action de type `Pattern`. Cette action permet de définir une liste d'actions à effectuer
    lorsque votre agent est déconnecté du serveur.

    /!\ Si vous ne spécifiez pas de pattern de déconnexion lorsque vous vous déconnectez,
        votre agent prendra toujours l'action d'aller vers le bas.
  
    /!\ Si dans votre pattern de déconnexion vous spécifiez un autre pattern de déconnexion,
        une erreur sera levée côté serveur et votre agent ne fera pas d'action pendant le tick.

    (en)
    Creates a `Pattern` action. This action allows you to define a list of actions to perform
    when your agent is disconnected from the server.
 
    /!\ If you do not specify a disconnection pattern, when you disconnect, your agent will
 	    always take the action of going down.
 
    /!\ If in your disconnection pattern you specify another disconnection pattern, an error
 	    will be raised on the server side and your agent will not take any action during the tick.
         
    Attributes:
        actions (List[Union[Teleport, Direction]]):     (fr) La liste des actions à effectuer.
                                                        (en) The list of actions to perform.
    """

    actions: List[Union[Teleport, Direction]]

    def serialize(self) -> bytes:
        serialized_actions = b''.join([action.serialize() for action in self.actions])
        return b'\x07' + serialized_actions


@dataclass
class Action:
    """
    (fr)
    Wrapper pour les actions de type `Teleport`, `Direction` et `Pattern`.

    (en)
    Wrapper for `Teleport`, `Direction` and `Pattern` actions.

    Attributes:
        action_type (Union[Teleport, Direction, Pattern]):  (fr) L'action à effectuer.
                                                            (en) The action to perform.

    Example:
        >>> # (fr) Téléporte vers la position (0, 0).
        >>> # (en) Teleports to the position (0, 0).
        >>> action = Action(Teleport(0, 0))

    Example:
        >>> # (fr) Déplace l'agent vers le haut.
        >>> # (en) Moves the agent up.
        >>> action = Action(Direction.UP)

    Example:
        >>> # (fr) Pattern de déconnexion qui téléporte l'agent vers la position (5, 5) et bouge 10 fois en alternant entre le haut et le bas.
        >>> # (en) Disconnection pattern that teleports the agent to the position (5, 5) and moves 10 times alternating between up and down.
        >>> action = Action(Pattern([Teleport(5, 5), *[Direction.UP, Direction.DOWN] * 5]))
    """

    action_type: Union[Teleport, Direction, Pattern]

    def serialize(self) -> bytes:
        return self.action_type.serialize()
