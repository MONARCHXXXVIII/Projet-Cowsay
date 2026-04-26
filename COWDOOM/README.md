# COWDOOM (prototype ASCII)

Prototype d'un mini-FPS ASCII en C inspire de DOOM, avec des vaches.

## Objectif actuel

- carte 2D en grille
- projection pseudo-3D via raycasting simplifie
- deplacements joueur avec collisions
- rotation camera a la souris
- affichage ASCII en fenetre SDL2

## Build

Dependances requises (Linux): SDL2 + outils de detection.

```bash
sudo apt update
sudo apt install -y libsdl2-dev pkg-config
```

```bash
cd /home/colin/projects/Projet-Cowsay/COWDOOM
make
```

## Lancer

```bash
./cowdoom
```

Le jeu s'ouvre dans une fenetre redimensionnable. Touche `F11` pour basculer en plein ecran.
La camera se controle a la souris (axe X) ou avec les fleches gauche/droite.
Au lancement, un menu propose deux modes:
- `Mode Survie`: vagues infinies de vaches qui accelerent
- `Mode Attaque`: grande carte, nombre cible de vaches a eliminer, puis boss final
Un curseur `>>>` indique clairement le mode selectionne.


## Commandes

- `z` ou `w`: avancer
- `q` ou `a`: deplacement gauche (strafe)
- `s`: reculer
- `d`: deplacement droite (strafe)
- `souris`: rotation camera (axe X inverse)
- `fleche gauche` / `fleche droite`: rotation camera (alternative clavier)
- `espace` ou clic gauche: tirer
- `r`: recharger l'arme
- `fleche haut` / `fleche bas`: naviguer dans le menu de lancement
- `Entree`: valider le mode dans le menu
- `f11`: plein ecran / fenetre
- `x` ou `Echap`: quitter

## Structure

- `src/main.c`: boucle de jeu et timing
- `src/input.c`: lecture clavier SDL2 instantanee (pas de validation par Entree)
- `src/game.c`: logique, raycasting, rendu ASCII, mini-carte
- `src/video.c`: fenetre SDL2 + affichage des caracteres ASCII
- `include/`: interfaces


