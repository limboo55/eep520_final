# **Jumper Game**

## **Project Introduction**
This is a 2D  game developed using the **ENVIRO** framework. Players control a **robot** character to navigate obstacles and platforms, avoid **enemies**, and reach the **goal door**. The game features classic platforming mechanics, including moving platforms, tracking enemies, and physics-based interactions.

## **Game Features & Mechanics**
### **Characters**
- **Robot (Player)** – Moves left/right, jumps, slows mid-air, resets on enemy contact.
- **Ghost** – Moves with certain trajectory, changes direction on collision, resets player on contact.
- **Tracker** – Chases player, accelerates when near, passes through obstacles.

### **Level Elements**
- **Static platforms**
- **Pushable boxes** – Used for creating paths.
- **Jump pads** – Propel players to higher areas.
- **Goal door** – Reaching it completes the game.

### **Controls**
- **A/D** → Move left/right
- **Spacebar** → Jump (ground only)
- **Reset Button** → Restores all objects
- **Cheat Mode** → Help you reach door quickly
### **Objective**
Push **boxes**, time **jumps**, and dodge **enemies** to reach the **goal door** and **win**!
![game demo](/media/demo1.gif)

when you reach the door:
![](/media/victory.jpg)
## Installation

### Requirements
Please make sure that you have docker and Git installed before proceeding to the installation steps.

1. [Docker](https://docs.docker.com/get-docker/)
2. [Git](https://git-scm.com/)

### Step by step installation instruction


1. Open up command prompt or bash.
2. Type in 
```bash
git clone https://github.com/limboo55/eep520_final.git
```
3. Change to the project directory
```bash
cd eep520_final/project
```
4. Before running the Docker container, you can replace **$PWD** with your project folder path. This command will automatically download the required Docker image if it’s not available locally.

```bash
docker run -p80:80 -p8765:8765 -v $PWD:/source -it klavins/enviro:v1.61 bash
```
5. To start the project, 
```bash
esm start
```
```bash
enviro
```
6. Open your web browser and type in **localhost:80** and you should be able to see this.
![](/media/start.png)

## Key challenges and Solutions

### Physics-Based Movement Control
**Challenge**: Creating responsive character movement that feels natural while respecting physics constraints.

**Solution**: Implemented different movement parameters for ground and air states, with reduced control while airborne. Adjusted acceleration and friction values to achieve the right feel.

### Tracking System
**Challenge**: Developing a track that could pursue the player through the environment.

**Solution** : Created a tracking system that:

- Uses an event-based mechanism where the player broadcasts its ID on startup
- Calculates distance and direction to the player in real-time
- Adjusts tracking speed based on distance from the player
- Implements appropriate collision detection to reset the player upon capture
- Provides visual feedback through color changes as the tracker gets closer

## Acknowledgement

This system is built on top of [Elma](https://github.com/klavinslab/elma) and [Enviro](https://github.com/klavinslab/enviro). 

- Elma is an event loop and process manager for embedded and reactive systems. Elma is structured as a C++ library of classes on top of which you build your own C++ project. It keeps track of processes, finite state machines, events, and communication channels, executing them at specified frequencies. 
- Enviro is a multi-agent, multi-user, multi-everything simulator for users to play around and have fun coding C++.

The installation and running this is made easy by using a dockerhub image built by [Klavins Lab](https://github.com/klavinslab).

