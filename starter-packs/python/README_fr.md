# Space - JDIS Game 2023

- **Space**
    - [Objectif](#objectif)
    - [Déroulement d'une partie](#déroulement-dune-partie)
    - [Actions](#actions)
        - [Déplacement](#déplacement)
        - [Téléportation](#téléportation)
        - [Pattern de déconnexion](#pattern-de-déconnexion)

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