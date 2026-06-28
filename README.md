# Last Shelter

**Last Shelter** is an Unreal Engine 5.4 C++ third-person zombie survival shooter developed by **Glass Key Games**. The player explores a forest survival map, collects resources, builds shelter pieces, finds weapons, fights zombies, and destroys enemy spawners to survive.

The project focuses on survival gameplay, resource collection, weapon and tool systems, building mechanics, enemy AI, and step-based objective progression.

## Features

* Unreal Engine 5.4 project
* C++ gameplay systems with Blueprint integration
* Third-person zombie shooter combat
* Rifle weapon system with ammo, reloading, projectile firing, and HUD ammo display
* Axe and pickaxe tools for collecting resources
* Wood and stone resource collection
* Build mode for placing shelter pieces
* Door placement and interactive open/close behavior
* Tree and environment collision polish
* Step-based objective progression
* Player HUD with health, ammo, resources, interaction prompts, and objectives
* Enemy AI encounters
* Zombie spawners that must be destroyed to win
* Win and lose game flow
* Debug log cleanup and gameplay polish

## Gameplay Overview

The player starts by exploring the environment and following objective prompts. Tools are collected and used to gather resources such as wood and stone. Those resources can be used with the build system to place shelter pieces. As the player progresses, they find a rifle, fight zombies, destroy spawners, and clear the remaining enemies to complete the demo objective.

The game is designed as a playable survival shooter prototype with a focus on clear progression, combat, resource gathering, and basic building.

## Objective Flow

The current gameplay loop includes objectives such as:

1. Find the axe.
2. Collect wood from trees.
3. Find the pickaxe.
4. Collect stone from rocks.
5. Build shelter pieces.
6. Find the abandoned shelter.
7. Find the rifle.
8. Find ammo.
9. Destroy all spawners and remaining zombies.

## Controls

| Action        | Key               |
| ------------- | ----------------- |
| Move          | WASD              |
| Look          | Mouse             |
| Jump          | Space             |
| Interact      | E                 |
| Build Mode    | B                 |
| Attack / Fire | Left Mouse Button |
| Hotbar Slots  | 1 - 4             |
| Pause         | Esc               |

## Project Info

* **Project Name:** Last Shelter
* **Engine:** Unreal Engine 5.4
* **Language:** C++
* **Genre:** Third-Person Zombie Survival Shooter
* **Developer:** Glass Key Games

## Repository Notes

This project uses **Git LFS** for Unreal Engine assets such as `.uasset` and `.umap` files.

Packaged builds are not stored in this repository. Playable builds should be uploaded separately to a platform such as itch.io or cloud storage.

## How to Open

1. Clone the repository.
2. Make sure Git LFS is installed.
3. Run `git lfs install`.
4. Run `git lfs pull`.
5. Open `LastShelter.uproject` with Unreal Engine 5.4.
6. Let Unreal rebuild project files if needed.
7. Open the project and play from the main test level.

## Git LFS

If cloning this project, run:

```bash
git lfs install
git lfs pull
```

## Status

This project is currently in development as a playable Unreal Engine survival shooter prototype.