# Space - JDIS Game 2023

- **Space**
    - [Objectif](#objectif)
    - [Déroulement d'une partie](#déroulement-dune-partie)
    - [Actions](#actions)
        - [Déplacement](#déplacement)
        - [Téléportation](#téléportation)
        - [Pattern de déconnexion](#pattern-de-déconnexion)
    - [Expansion de terroire](#expansion-du-territoire)
    - [Interaction avec les autres joueurs](#interaction-avec-les-autres-joueurs)

## Objectif
L'objectif principal de Space est de conquérir le plus de territoire possible en traçant 
des lignes avec votre personnage.

## Déroulement d'une partie
Une partie de space se déroule sur une durée de 1200 ticks, où chaque tick a une durée
de 300 millisecondes.

## Actions
À chaque tick, les joueurs peuvent effectuer une action parmi 
 - le [déplacement](#déplacement) 🚶‍♂️
 - la [téléportation](#téléportation) 🌀 
 - la mise en place d'un [pattern de déconnexion](#pattern-de-déconnexion) ⏸️. 
 
⚠️ Si aucune action n'est prise durant le tick par l'agent, le serveur jouera à la place de l'agent en jouant avec le pattern de déconnexion 🤖. 🕹️

### Déplacement
Votre personnage se déplace automatiquement dans une direction spécifiée par un agent (intelligence artificielle) plutôt que par les contrôles du clavier. Lorsque vous vous déplacez à l'extérieur de votre zone, une ligne (trace) de votre couleur sera déposée au sol. Il existe quatre mouvements possibles lors d'un déplacement :

- Haut ⬆️
- Bas ⬇️
- Gauche ⬅️
- Droite ➡️

### Téléportation
Dans ce jeu, vous avez la possibilité de vous téléporter sur une case de votre propre territoire. Cette action de téléportation est extrêmement utile pour échapper aux dangers ou pour atteindre rapidement des endroits clés sur la carte. Cependant, cette capacité est soumise à un temps de recharge **(cooldown) de 8 tours** avant de pouvoir l'utiliser à nouveau.

Lorsque vous choisissez d'utiliser cette téléportation, vous devez bien calculer le bon moment pour l'activer, car chaque utilisation compte. L'action de téléportation peut être un atout stratégique majeur pour surprendre vos adversaires ou pour capturer des zones éloignées en un clin d'œil. 🧭🎯

### Pattern de déconnexion
vous avez la possibilité d'envoyer un pattern de déconnexion au serveur. Ce pattern est une liste d'actions comprenant des déplacements et des téléportations que le serveur utilisera lorsque votre agent ne sera pas en mesure de communiquer avec lui, comme lors d'une déconnexion et reconnexion du bot. Le pattern peut comporter jusqu'à un maximum de **20 actions** et sera bouclé par le serveur.

En planifiant soigneusement le pattern de déconnexion, vous pouvez anticiper différentes situations et permettre à votre personnage de prendre les meilleures décisions en votre absence. 📈

Assurez-vous de mettre en place un pattern de déconnexion réfléchi pour optimiser les actions de votre personnage pendant ces moments critiques.

## Expansion de territoire
Lorsque votre personnage boucle une zone en traçant une ligne, celle-ci sera coloriée à votre couleur, vous donnant ainsi le contrôle sur cette zone. Cette mécanique vous permet de conquérir progressivement de nouveaux territoires 🎨.

## Interaction avec les autres joueurs

### Comment tuer ou se faire tuer ?

Vous pouvez tuer d'autres agents en marchant sur leurs traces qu'ils laissent derrière eux. 🚶‍♂️💨

Lorsque vous sortez de votre territoire, vous laissez une trail colorée. Si un autre joueur marche dessus, vous mourrez. 🕳️😵

Planifier astucieusement vos déplacement, tracez votre chemin et assurez-vous de ne pas vous piéger vous-même
en évitant vos propres traces. 💥🎯

### Qu'arrive-t-il lorsque l'on meurt ?
Si vous n'avez pas encore jouer votre tour lorsqu'un autre agent vous tue, votre action ne sera pas prise en compte ❌.

À la fin de ce tick, votre agent sera téléporter à une nouvelle destination et pourra recommencer à jouer comme si de rien était. 

