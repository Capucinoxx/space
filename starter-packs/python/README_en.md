# Space - JDIS Game 2023

- **Space**
    - [Objective](#objective)
    - [Course of a game](#course-of-a-game)
    - [Actions](#actions)
        - [Movement](#movement)
        - [Teleportation](#teleportation)
        - [Disconnect pattern](#disconnect-pattern)
    - [Territory expansion](#territory-expansion)
    - [Interaction with other players](#interaction-with-other-players)
    - [Score et ranking](#score-and-ranking)
    - [Interaction with the plateform](#interaction-with-the-plateform)
        - How do I register ?
        - Prerequisites
        - How do I log in ?
        - How to modify my bot ?

## Objective
In Space, the player (your agent) controls a small square, leaving trails on the map when he leaves his own territory. The main objective of the game is to conquer as much territory as possible by tracing lines to create closed shapes.

## Course of a game
THe game is in _"Long running"_ format. This means that the game never stops. You will accumulate points **throughout the day**. You must plan the best times to disconnect your agent to update it with a new version. It is essential to design your bot incrementally and to make continuous improvement.

A game of Space takes place over a duration of 1200 ticks, where each tick has a duration of 300 milliseconds.

The server performs the actions according to the first come first served principle.

## Actions
On each tick, bots can perform one of the following actions :
 - [movement](#movement) 🚶‍♂️
 - [teleportation](#teleportation) 🌀 
 - setting up a [disconnection pattern](#disconnection-pattern) ⏸️.

⚠️ If no action is taken by the agent during the tick, the server will play for the agent with the disconnection pattern 🤖. 🕹️

### Movement
Your bot automatically moves in a direction specified by an agent (artificial intelligence) rather than keyboard controls. When you move outside of your zone, a line (trail) of your color will be deposited on the ground. There are four possible movements :

- Up ⬆️
- Down ⬇️
- Left ⬅️
- Right ➡️

### Teleportation
You have the option to teleport yourself on a tile in **your own territory**. This teleport action is extremely useful for escaping any dangers or quickly reaching key locations on the map. However, this ability is subject to a **cooldown of 8 turns** before it can be used again.

When you choose to use teleportation, you have to carefully calculate the right time to activate it, because every usage counts. The teleportation action can be a major strategic asset to surprise your opponents. 🧭🎯

### Disconnect pattern
You can send a disconnect pattern to the server. This pattern is a list of actions including movements and teleportations that the server will use when your agent will not be able to communicate with it, such as disconnection and reconnexion of the bot. The pattern can include a maximum of **20 actions** and will be looped by the server.

By carefully planning the disconnect pattern, you can anticipate different situations and allow your character to take the best decisions during your absence. 📈

Make sure to implement a thoughtful disconnect pattern to optimize you character's actions during these critical moments.

### Territory expansion
To extend your territory, you must make the line you trace return to your own territory, i.e. the zone already captured by your bot. When the line connects your tracks to your existing territory, the inner zone of the traced lines will now become your territory and will be colored with your color. This mechanism will allow you to gradually conquer new territories 🎨.

## Interaction with other players

### How to kill or be killed ?

As long as your line is not closed by returning to your own territory, it remains vulnerable. If another player crosses your trail, you will be eliminated from the game and you lose a part of your territory. Similarly, your bot can kill other agents by passing on their trail 🚶‍♂️💨.

If you pass on your own unfinished trail, you will die 🕳️😵

Plan your moves wisely, plot your path, and make sure you don't trick yourself by avoiding your own trails. 💥🎯

### What happens when I die ?
If you die, your agent will be teleported to a new destination and can start playing again as if nothing had happened. The territory that was acquired by the agent during his lifetime is now removed from him.

If you have not yet taken your turn when another agent kills you, your action will not be taken into account ❌.


## Score and ranking
With each tick of the game, your score will be increased according to the following formula:

**Score for the tick** : (zone_score + kill_bonus + capture_bonus) * multiplier

| Description | Formula |
| :-- | :-- |
| **🏞️ Zone Score**  | The zone score will be calculated according to the number of tiles that make up the player's territory. (The trail doesn't count) |
| **🗡️ Kill Bonus** | If you killed someone during the tick, you get a bonus. The calculation is as follows: <br/> 💀 12 * (1 + length of his trail at the time of the murder) |
| **🏰 Capture Bonus** | If you capture a new region during the round, you will receive a bonus according the following formula: <br/> 🚩 3 * (1 + length of the trail when captured) |
| **🌟 Multiplier** | A multiplier will be used during the day. It will start with a value of 1 at the beginning of the day to gradually increase until it reaches a factor of 5 at the end of the day. |

**🏆 Ranking** : A ranking is present showing the total progress of each team during the day. The overall ranking will be used to determine the winning teams.

## Interaction with the plateform

### 🤝 How do I register ?
1. 🌐 Go to this page http://jdis-ia.dinf.fsci.usherbrooke.ca
2. 🖱️ Click on the button at the bottom left to access the registration form.
3. 📝 In the form, type the name of your bot (20 characters maximum).
4. 🎯 Once the name of the bot is entered, click on the button to register.
5. 🚀 Once registered, you should receive an authentication token alert.
6. ⚠️ Make sure to note the authentication token, you will need it to connect your agent.
7. ❓ If you ever forgot to note it, go see the organizers, they will help you.
8. 🔑 Each team name must be unique.

That's it ! You're ready to participate ! 🎉

### 📋 Prerequisites

For the starter kit with python, you first have to install the project dependencies.

```sh
pip install -r requirements.txt
```

### How do I log in ?

🤝 How do I log in ?

To sign in with your agent, you will need your authentication token. The game offers two parts : an unranked game and a ranked one. The difference between the two is that the points are not counted in the unranked game.

**Unranked game :**
```sh
python run_bot.py -s <MY AUTHENTICATION TOKEN>
```

**Ranked game :**

```sh
python run_bot.py -s <MY AUTHENTICATION TOKEN> -r
```

_⚠️ You can only connect one instance of your bot simultaneously in each game._

### 🛠️ How to modify my bot ?

The only file you will modify in the starting code is the `./src/bot.py` file. You will find the `MyBot` class in this file, which represents the bot you will need to code. When you launch your bot, an instance of this class is created. On each tick, the `tick` function will be called, providing the current state of the map.

**⚙️ Bot modifications :**
1. Open the ./src/bot.py file.
2. Find the MyBot class.
3. Code your bot by implementing the game logic in the tick function.

**⏳ Time limit :**
Make sure your bot returns a response within the 300 ms of receiving data from the server. Otherwise, the server will assume that you are disconnected and will execute your disconnection pattern.