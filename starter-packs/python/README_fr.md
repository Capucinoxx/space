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
    - [Score et classement](#score-et-classement)
    - [Interaction avec la plateforme](#interaction-avec-la-plateforme)
        - Comment m'inscrire ?
        - Prérequis
        - Comment me connecter ?
        - Comment modifier mon bot ?

## Objectif
L'objectif principal de Space est de conquérir le plus de territoire possible en traçant 
des lignes avec votre agent.

## Déroulement d'une partie
Le jeu est sous format _"Long running"_. Cela signifie que le jeu n'arrête jamais. Vous devez prévoir les meilleur moment pour déconnecter votre agent pour y mettre une nouvelle version.

Une partie de space se déroule sur une durée de 1200 ticks, où chaque tick a une durée
de 300 millisecondes.

## Actions
À chaque tick, les bots peuvent effectuer une action parmi 
 - le [déplacement](#déplacement) 🚶‍♂️
 - la [téléportation](#téléportation) 🌀 
 - la mise en place d'un [pattern de déconnexion](#pattern-de-déconnexion) ⏸️. 
 
⚠️ Si aucune action n'est prise durant le tick par l'agent, le serveur jouera à la place de l'agent en jouant avec le pattern de déconnexion 🤖. 🕹️

### Déplacement
Votre bot se déplace automatiquement dans une direction spécifiée par un agent (intelligence artificielle) plutôt que par les contrôles du clavier. Lorsque vous vous déplacez à l'extérieur de votre zone, une ligne (trace) de votre couleur sera déposée au sol. Il existe quatre mouvements possibles lors d'un déplacement :

- Haut ⬆️
- Bas ⬇️
- Gauche ⬅️
- Droite ➡️

### Téléportation
Vous avez la possibilité de vous téléporter sur une case de votre propre territoire. Cette action de téléportation est extrêmement utile pour échapper aux dangers ou pour atteindre rapidement des endroits clés sur la carte. Cependant, cette capacité est soumise à un temps de recharge **(cooldown) de 8 tours** avant de pouvoir l'utiliser à nouveau.

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


## Score et classement
À chaque tick du jeu, votre score sera augmenté selon la formule suivante:

**Score pour le tick tick** : (zone_score + kill_bonus + capture_bonus) * multiplicateur

**🏞️ Score de zone** : Le score de zone sera calculé selon le nombre de tuiles qui composent le territoire du joueur. (La trail ne compte pas)

**🗡️ Bonus de Kill** : Si vous avez tué quelqu'un durant le tick, vous obtenez un bonus. Le calcul est le suivant:

💀 12 * (1 + longueur de sa trail au moment du meurtre)

**🏰 Bonus de Capture** : Si vous capturez une nouvelle région pendant le tour, vous recevez un bonus selon la formule suivante:

🚩 3 * (1 + longueur de la trail lors de la capture)

**🌟 Multiplicateur** : Un multiplicateur sera utilisé durant la journée. Il commencera avec une valeur de 1 en début de journée pour augmenter progressivement jusqu'à atteindre un facteur de 5 en fin de journée.

**🏆 Classement** : Un classement sera présent montrant l'avancement total de chaque équipe durant la journée. Le classement global sera utilisé pour déterminer les équipes gagnantes.

## Interaction avec la plateforme 

### 🤝 Comment m'inscrire ?
1. 🌐 Rendez-vous sur la page http://jdis-ia.dinf.fsci.usherbrooke.ca
2. 🖱️ Cliquez sur le bouton en bas à droite pour accéder au formulaire d'inscription.
3. 📝 Dans le formulaire, inscrivez le nom de votre Bot (maximum de 20 caractères).
4. 🎯 Une fois le nom du bot entré, cliquez sur le bouton pour vous enregistrer.
5. 🚀 Une fois enregistré, vous devriez recevoir un jeton d'authentification en alerte.
6. ⚠️ Assurez-vous de prendre note du jeton d'authentification, vous en aurez besoin pour connecter votre agent.
7. ❓ Si jamais vous avez oublié de le noter, allez voir les organisateurs, ils vous aideront.
8. 🔑 Chaque nom d'équipe doit être unique.

C'est tout ! Vous êtes prêt.e à participer ! 🎉

### 📋 Prérequis

Pour le kit de démarraqge avec python, vous devez dans un premier temps installer les dépendances du projet.

```sh
pip install -r .
```

### Comment me connecter ?

🤝 Comment me connecter ?

Pour vous connecter avec votre agent, vous aurez besoin de votre jeton d'authentification. Le jeu propose deux parties : une partie non classée et une partie classée. La différence entre les deux réside dans le fait que les points ne sont pas comptabilisés dans la partie non classée.

**Partie non classée :**
```sh
python run_bot.py -s <MON JETON D'AUTHENTIFICATION>
```

**Partie classée :**

```sh
python run_bot.py -s <MON JETON D'AUTHENTIFICATION> -r
```

_⚠️ Vous ne pouvez connecter qu'une seule instance de votre bot simultanément dans chacune des parties._

### 🛠️ Comment modifier mon bot ?

Le seul fichier que vous devriez modifier dans le code de départ est le fichier `./src/bot.py`. Dans ce fichier se trouve la classe `MyBot`, qui représente le bot que vous devrez coder. Lorsque vous lancez votre bot, une instance de cette classe est créée. À chaque tick, la fonction `tick` sera appelée, fournissant l'état actuel de la carte.

**⚙️ Modifications du bot :**
1. Ouvrez le fichier ./src/bot.py.
2. Recherchez la classe MyBot.
3. Codez votre bot en implémentant la logique de jeu dans la fonction tick.

**⏳ Limite de temps :**
Assurez-vous que votre bot renvoie une réponse dans les 300 ms suivant la réception des données du serveur. Sinon, le serveur considérera que vous êtes déconnecté et exécutera votre modèle de déconnexion.