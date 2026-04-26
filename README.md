# Projet Cowsay

Voici le compte rendu documentant l'intégralité du projet Cowsay réalisé par Riyad BOUDALIA (MONARCHXXXVIII sur Github) et Arthur COLIN (Ayvem). Le projet est divisé en trois parties complémentaires plus des idées de Arthur qui n'avais rien à faire (IA, DOOM?):
- **Partie Bash** : Scripts shell utilisant la commande `cowsay` pour afficher des suites mathématiques
- **Partie C** : Réimplémentation et extensions de `cowsay` en C pur avec animations et fonctionnalités avancées
- **Partie Automates** : Implémentation du jeu `Tamagoshi` en C a l'aide d'automates
- **Partie IA** : Inférence locale d'un modèle GGUF avec `llama.cpp` via le programme `vache_folle`
- **COWDOOM** : Mini-FPS ASCII en C inspiré de DOOM, avec des vaches, rendu raycasting et fenêtre SDL2 (Parce que pourquoi pas)

---

## Table des matières

1. [Prérequis et Installation](#prérequis-et-installation)
2. [Partie Bash](#partie-bash)
   - [Vue d'ensemble](#vue-densemble-bash)
   - [Scripts disponibles](#scripts-disponibles)
   - [Notes d'implémentation](#notes-dimplémentation-bash)
3. [Partie C](#partie-c)
   - [newcow.c](#newcowc--recoder-cowsay-en-c)
   - [wildcow.c](#wildcowc--la-vache-animée)
   - [reading_cow.c](#reading_cowc--la-vache-qui-lit)
   - [Difficultés rencontrées](#difficultés-rencontrées-et-choix-techniques)
4. [Partie Automates](#partie-automates)
   - [Automate.h](#automateh--header-important)
   - [Automate.c](#automatec--implementation-structure-automate)
   - [Tamagoshi.c](#tamagoshic--programe-principal)
   - [Difficultés rencontrées](#difficultés-rencontrées-et-choix-techniques)
5. [Partie IA](#partie-ia)
  - [Vue d'ensemble](#vue-densemble-ia)
  - [Compilation](#compilation-ia)
  - [Exécution](#execution-ia)
6. [COWDOOM](#cowdoom)
   - [Vue d'ensemble](#vue-densemble-cowdoom)
   - [Installation](#installation-cowdoom)
   - [Compilation et lancement](#compilation-et-lancement)
   - [Commandes](#commandes)
   - [Structure du code](#structure-du-code)



---

**Note importante sur l'Utilisation de l'IA** : Afin de mener a bien ce projet, nous avons utilisé certaines intelligences artificielles afin de nous documenter sur certains aspects du Bash et du C, en particulier lorsque des forums comme *Stack Overflow* ne repondaient pas a ce que l'on voulait savoir. **Cependant** il n'y a absolument **AUCUN** morceau de code (meme partiel) qui aurait été copié/collé d'une IA Générative, y compris ce fichier Markdown qui a chaque partie a été rédigée par le membre l'ayant implémenté d'ou les fautes d'aurtograf (c'est arthur qui a écrit ca.).

## Prérequis et Installation

### Prérequis généraux

- **Bash shell** (pour les scripts de la Partie Bash)
- **gcc** (compilateur C, pour la Partie C)
- **make** (optionnel, pour faciliter la compilation de la Partie C)
- **cmake** et **git** (pour compiler `llama.cpp` en Partie IA)
- **Python 3** (si vous utilisez le workflow `IA/finetune_fr`)
- Commande `cowsay` installée (pour la Partie Bash)

### Installation de cowsay

**Sur Ubuntu/Debian:**
```bash
sudo apt-get install cowsay
```

**Sur macOS:**
```bash
brew install cowsay
```

### Configuration des permissions

**Partie Bash :**
```bash
chmod +x Partie_Bash/cow_*
```

**Partie C :**

Compilez avec le `Makefile` fourni :
```bash
cd Partie_C
make all        # compile newcow, wildcow, reading_cow
make clean      # supprime les binaires
```

Ou compilez individuellement :
```bash
gcc -Wall -Wextra -std=c99 -o newcow       newcow.c
gcc -Wall -Wextra -std=c99 -o wildcow      wildcow.c
gcc -Wall -Wextra -std=c99 -o reading_cow  reading_cow.c
```

> **Note sur `-std=c99`** : on utilise le standard C99 pour pouvoir déclarer
> des variables au milieu d'un bloc (ex. `int i` dans un `for`), ce que C89
> interdit. Sans ce flag, gcc peut refuser certaines déclarations.

**Partie Automates**

```bash
make
make clean
```

**Partie IA**

```bash
sudo apt update
sudo apt install -y build-essential cmake git ninja-build
```

# Partie Bash

## Vue d'ensemble Bash

La Partie Bash consiste en une collection de scripts qui utilisent la commande `cowsay` pour afficher des suites numériques progressivement plus complexes. Les scripts suivent une progression pédagogique :

1. **Kindergarten** : Introduction simple (boucle 1-10)
2. **Primaryschool** : Paramétrage (boucle 1-n)
3. **Highschool** : Calculs (carrés)
4. **College** : Suites mathématiques (Fibonacci)
5. **University** : Algorithmes (nombres premiers)
6. **Smart_cow** : Interactivité (calculs avec résultats visuels)
7. **Crazy_cow** : Créativité (quiz interactif)

## Scripts disponibles

### 1. `cow_kindergarten`
**Description:** La vache prononce les chiffres de 1 à 10, avec une pause d'une seconde entre chaque chiffre, puis tire la langue à la fin.

**Usage:**
```bash
./cow_kindergarten
```

**Caractéristiques:**
- Boucle simple sur les nombres 1 à 10
- Utilise `sleep 1` pour une pause d'une seconde
- Utilise `clear` pour rafraîchir l'écran
- Affiche un message spécial à la fin avec une vache dragon

---

### 2. `cow_primaryschool`
**Description:** La vache prononce les chiffres de 1 à n, où n est fourni en argument du script.

**Usage:**
```bash
./cow_primaryschool 15
```

**Caractéristiques:**
- Prend un nombre en argument (obligatoire)
- Validation de l'entrée (doit être un nombre positif)
- Boucle de 1 à n
- Message d'erreur en cas d'argument invalide ou manquant

**Exemple:**
```bash
./cow_primaryschool 5
# Affichera 1, 2, 3, 4, 5 avec une pause d'une seconde chacun
```

---

### 3. `cow_highschool`
**Description:** La vache prononce la suite des carrés: 1, 4, 9, 16, ..., jusqu'à n².

**Usage:**
```bash
./cow_highschool 100
```

**Caractéristiques:**
- Prend un nombre n en argument
- Calcule et affiche i² pour chaque i où i² ≤ n
- Pause d'une seconde entre chaque nombre
- Validation de l'entrée

**Exemple:**
```bash
./cow_highschool 50
# Affichera: 1, 4, 9, 16, 25, 36, 49
```

---

### 4. `cow_college`
**Description:** La vache prononce la suite de Fibonacci (0, 1, 1, 2, 3, 5, 8, 13, ...) jusqu'à n.

**Usage:**
```bash
./cow_college 100
```

**Caractéristiques:**
- Prend un nombre n en argument (limite supérieure)
- Implémente l'algorithme de Fibonacci
- Affiche tous les nombres de Fibonacci inférieurs à n
- Pause d'une seconde entre chaque nombre

**Exemple:**
```bash
./cow_college 50
# Affichera: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34
```

---

### 5. `cow_university`
**Description:** La vache prononce la suite des nombres premiers inférieurs à n.

**Usage:**
```bash
./cow_university 30
```

**Caractéristiques:**
- Prend un nombre n en argument (n ≥ 2)
- Fonction `is_prime()` pour vérifier si un nombre est premier
- Teste tous les nombres de 2 à n-1
- Optimisation: teste seulement jusqu'à √n pour améliorer les performances

**Exemple:**
```bash
./cow_university 30
# Affichera: 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
```

---

### 6. `smart_cow`
**Description:** La vache résout un calcul numérique simple (addition, soustraction, multiplication, division) et affiche le résultat dans ses yeux.

**Usage:**
```bash
./smart_cow "3+11"
./smart_cow "15-7"
./smart_cow "4*5"
./smart_cow "20/4"
```

**Caractéristiques:**
- Prend une expression mathématique en argument
- Accepte les opérateurs: +, -, *, /
- Valide le format de l'expression
- Calcule le résultat et l'affiche dans les yeux de la vache (au lieu des "oo")
- Affiche le résultat tronqué à 2 caractères s'il est trop long

**Exemple:**
```bash
./smart_cow "3+11"
# Affichera:
# ________
# < 3+11 >
# --------
# \ ^__^
# \ (14)\_______
# (__)\ )\/\
# ||----w |
# || ||
```

---

### 7. `crazy_cow`
**Description:** Un quiz mathématique interactif où la vache pose 5 questions aléatoires et affiche le score final.

**Usage:**
```bash
./crazy_cow
```

**Caractéristiques:**
- Quiz interactif avec 5 questions
- Questions générées aléatoirement (nombres de 1 à 100)
- Opérateurs aléatoires: +, -, *, /
- La vache félicite ou encourage l'utilisateur selon le score
- Scores spéciaux avec différentes vaches:
  - 5/5: Message avec vache dragon 🚀
  - 3-4/5: Message d'encouragement 📈
  - <3/5: Message de motivation 💪

**Exemple:**
```bash
./crazy_cow
# Bienvenue dans le quiz fou de la vache arithmétique!
# Question 1: 45 + 23 = ?
# Ta réponse: 68
# Correct! La réponse est 68 🎉
# ...
# Score final: 4 / 5
```

---

## Notes d'implémentation Bash

- Tous les scripts utilisent `clear` pour rafraîchir l'écran à chaque itération
- Tous les scripts utilisent `sleep 1` pour une pause d'une seconde
- La validation des arguments est implémentée dans les scripts qui en ont besoin
- Les scripts utilisent des approches différentes pour les boucles (for, while) selon les besoins
- Le calcul du résultat dans `smart_cow` utilise l'expansion arithmétique bash `$(( ))`

---

# Partie C

Cette partie réimplémente `cowsay` en C pur et ajoute trois nouveaux programmes avec des fonctionnalités avancées.

## newcow.c – Recoder cowsay en C

### Vue d'ensemble

`newcow` reproduit le comportement de base de `cowsay` et y ajoute plusieurs options originales. Son architecture repose sur trois fonctions principales :

- `affiche_bulle()` — dessine la bulle de texte au-dessus de la vache
- `affiche_vache()` — dessine le corps de la vache avec paramètres variables
- `affiche_troupeau()` — affiche N vaches à la suite (option `--herd`)

### Options disponibles

| Option courte | Option longue | Argument | Description |
|---|---|---|---|
| `-e` | `--eyes` | `XX` | Remplace les yeux `oo` par deux caractères |
| `-T` | `--tongue` | `XX` | Remplace la langue (deux caractères) |
| `-t` | `--tail` | `N` | Allonge la queue de N espaces |
| `-W` | `--wrap` | `N` | Largeur de retour à la ligne (défaut : 40) |
| — | `--think` | — | Bulle de pensée (flèches en `o` au lieu de `\`) |
| — | `--herd` | `N` | Affiche un troupeau de N vaches |
| `-h` | `--help` | — | Affiche l'aide |

### Exemples d'exécution

**Exemple 1 — appel basique:**
```
$ ./newcow "Bonjour !"
 ___________
< Bonjour ! >
 -----------
        \  ^__^
         \ (oo)\_______
           (__)\       )\/\
              ||----w |
              ||     ||
```

**Exemple 2 — yeux personnalisés:**
```
$ ./newcow --eyes "@@" "Je suis hypnotisée"
 ____________________
< Je suis hypnotisée >
 --------------------
        \  ^__^
         \ (@@)\_______
           (__)\       )\/\
              ||----w |
              ||     ||
```

**Exemple 3 — queue allongée:**
```
$ ./newcow --tail 6 "Longue queue !"
        \  ^__^
         \ (oo)\_______
           (__)\       )\/\/\/\/\/\/\
              ||----w |
              ||     ||
```

**Exemple 4 — mode pensée:**
```
$ ./newcow --think "Hmm..."
 _______
< Hmm... >
 -------
        o  ^__^
         o (oo)\_______
           (__)\       )\/\
```
> Les flèches `\` deviennent des `o`, rappelant les bulles de pensée en BD.

**Exemple 5 — troupeau:**
```
$ ./newcow --herd 3 "Meuh"
 ______
< Meuh >
 ------
  Vache 1 :
        \  ^__^  ...
  Vache 2 :
        \  ^__^  ...
  Vache 3 :
        \  ^__^  ...
```

**Exemple 6 — texte long avec retour à la ligne:**
```
$ ./newcow --wrap 20 "Voici un très long message qui doit être coupé"
 ______________________
/ Voici un très long m \
| essage qui doit être |
\ coupé               /
 ----------------------
```
> Quand le texte est plus long que `wrap`, les bordures changent :
> `/` et `\` pour la première et dernière ligne, `|` pour les lignes du milieu.

**Exemple 7 — Sans aucun argument:**
```
$ ./newcow
 ___________________
< yo,riyad m'a cree >
 -------------------
        \  ^__^
         \ (oo)\_______
           (__)\       )\/\
               ||----w |
               ||     ||
```
> Le message par défaut est "riyad m'a cree" car c'est Riyad qui s'est occupé de cette partie :)


### Lignes délicates dans newcow.c

**La fonction `affiche_bulle` et les formats `printf`**

```c
printf("< %.*s >\n", len, texte);
```
`%.*s` est un format peu courant : le `*` signifie que la largeur de précision
est fournie dynamiquement par l'argument `len` juste avant `texte`. Cela permet
d'afficher exactement `len` caractères du texte sans avoir besoin d'une chaîne
null-terminée à la bonne longueur. C'est plus sûr qu'un `%s` brut.

```c
printf("/ %-*.*s \\\n", line_len, line_len, texte + pos);
```
Ici `%-*.*s` combine trois choses :
- `-` : alignement à gauche
- `*` : largeur minimale fournie par `line_len`
- `.*s` : largeur maximale (tronque si trop long) aussi fournie par `line_len`

Le `\\` en fin produit un seul backslash `\` dans la sortie (le backslash
doit toujours être échappé dans une chaîne C).

**Parsing des arguments avec `argc/argv`**

```c
for (int i = 1; i < argc; i++) {
    if ((strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eyes") == 0)
        && i + 1 < argc) {
        i++;
        ...
    }
}
```
La vérification `i + 1 < argc` avant de faire `i++` est cruciale : elle
garantit qu'on ne lit pas `argv[argc]` (pointeur invalide) si l'option est
fournie sans argument (ex. `./newcow -e` sans caractères derrière).

**Le paramètre `__attribute__((unused))`**

```c
void affiche_bulle(const char *texte, int wrap, int think __attribute__((unused)))
```
Le paramètre `think` est prévu pour une évolution future mais n'est pas encore utilisé.
Sans `__attribute__((unused))`, `gcc -Wextra` émet un warning.

**Copie sécurisée des yeux et de la langue**

```c
char e[3] = "oo";
if (eyes && strlen(eyes) >= 2) { e[0] = eyes[0]; e[1] = eyes[1]; }
```
On vérifie d'abord que le pointeur `eyes` n'est pas `NULL` et que la chaîne
fait au moins 2 caractères avant d'y accéder.

---

## wildcow.c – La vache animée

### Vue d'ensemble

`wildcow` utilise les séquences d'échappement ANSI pour manipuler le curseur du terminal et produire des animations frame par frame. Quatre modes sont disponibles.

### Les séquences ANSI : fonctionnement

```c
void clear_screen() { printf("\033[H\033[J"); fflush(stdout); }
void goto_xy(int x, int y) { printf("\033[%d;%dH", y, x); fflush(stdout); }
void hide_cursor()  { printf("\033[?25l"); fflush(stdout); }
void show_cursor()  { printf("\033[?25h"); fflush(stdout); }
```

`\033` est le caractère ESC (code ASCII 27) en notation octale.

Les séquences ANSI sont de la forme `ESC [ ... lettre` :

| Séquence | Effet |
|---|---|
| `\033[H\033[J` | `H` = curseur en haut à gauche ; `J` = efface l'écran vers le bas |
| `\033[L;CH` | Positionne le curseur à la ligne `L`, colonne `C` |
| `\033[?25l` | Cache le curseur (lettre `l` = lower, désactive) |
| `\033[?25h` | Affiche le curseur (lettre `h` = high, active) |

> **Pourquoi `fflush(stdout)` systématiquement ?**
> `printf` utilise un buffer interne. Sans `fflush`, les caractères peuvent
> rester en mémoire et ne pas s'afficher immédiatement. Dans une animation,
> cela provoquerait des "sauts" ou un écran vide. On force donc le flush après
> chaque opération visuelle.

### Macro `_DEFAULT_SOURCE`

```c
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
```

`usleep()` (pause en microsecondes) n'est pas dans le standard C99 strict :
c'est une extension POSIX/BSD. Sans `_DEFAULT_SOURCE`, gcc en mode strict
refuse de déclarer `usleep` et émet une erreur. Cette macro doit être placée
avant tout `#include`, sinon les en-têtes ont déjà été lus sans elle.

### Les couleurs ANSI

```c
const char *colors[] = {"\033[1;33m", "\033[1;31m", "\033[0;37m"};
printf("%s%s\033[0m", colors[f], frames[f]);
```

Format : `\033[ STYLE ; COULEUR m`

| Code | Signification |
|---|---|
| `0` | Réinitialise tous les attributs |
| `1` | Gras |
| `31` | Rouge |
| `33` | Jaune |
| `35` | Magenta |
| `36` | Cyan |
| `37` | Blanc/gris |

Le `\033[0m` à la fin de chaque `printf` coloré réinitialise la couleur.

### Modes et exemples d'exécution

**Mode par défaut — marche droite → gauche:**
```bash
$ ./wildcow
```

La vache traverse l'écran de droite à gauche avec un dessin miroir cohérent.

```c
for (int x = TERM_WIDTH - COW_WIDTH; x >= 1; x--) {
    clear_screen();
    draw_cow(x, y, -1, "oo", -1);
    usleep(80000);  // 80 ms entre chaque frame → ~12 fps
}
```

**Mode `-hee_hee` — Moonwalk:**
```bash
$ ./wildcow -hee_hee
```

L'effet moonwalk repose sur un paradoxe visuel : la vache regarde à gauche
(dessin miroir, `direction = -1`) mais avance vers la droite (`x` croît).
C'est exactement ce que fait Michael Jackson. Le texte `♪ hee hee ! ♪`
s'affiche en magenta à chaque frame.

**Mode `-boom` — Décompte et explosion:**
```bash
$ ./wildcow -boom
```

Phase 1 — La vache entre en marchant depuis la droite jusqu'au centre.
Phase 2 — La fonction `draw_countdown(5, x, y)` lance le décompte.
À chaque seconde, la mèche raccourcit d'un `~` :

```c
if (boom_fuse >= 0) {
    goto_xy(x + 17, y - 1);
    printf("o");
    for (int i = 0; i < boom_fuse; i++) printf("~");
    printf("*");
}
```

Phase 3 — `draw_explosion` affiche 3 frames successives en couleurs
différentes (jaune → rouge → gris) avec une pause de 400 ms.

Phase 4 — Écran final en rouge : `*** MOO IN PEACE ***`.

**Mode `-go_crazy` — Va-et-vient fou:**
```bash
$ ./wildcow -go_crazy
```

Phase 1 — La vitesse augmente progressivement et la direction change aléatoirement.
La vache accélère avec une probabilité de 1/15 de changer de direction à chaque step.
Les yeux sont tirés au hasard parmi 8 options.

Phase 2 — La vache s'immobilise avec les yeux `xx`.

### Initialisation du générateur aléatoire

```c
srand((unsigned)time(NULL));
```

`rand()` produit une séquence déterministe. `srand(time(NULL))` initialise
la graine avec l'heure courante, ce qui garantit une séquence différente
à chaque lancement.

---

## reading_cow.c – La vache qui lit

### Vue d'ensemble

`reading_cow` lit un fichier (ou stdin) caractère par caractère avec `fgetc`.
Chaque caractère est d'abord affiché dans la "gueule" de la vache,
puis "avalé" et ajouté à la bulle de texte. Une pause d'une seconde sépare
chaque caractère.

### Exemples d'exécution

**Préparation:**
```bash
echo "bonjour" > fichier.txt
./reading_cow fichier.txt
```

**Étape 1 — La vache montre le `b` dans sa gueule:**
```
 _______
<        >
 -------
        \  ^__^
         \ (oo)\_______
           (__)\       )\/\
            b  ||----w |
               ||     ||

 Caractères lus : 1
```

**Fin — Tout avalé, yeux satisfaits `^^`:**
```
 _________
< bonjour >
 ---------
        \  ^__^
         \ (^^)\_______
           (__)\       )\/\
               ||----w |
               ||     ||

 La vache a tout lu ! (7 caractères avalés)
```

**Lecture depuis stdin:**
```bash
echo "moo" | ./reading_cow
# ou interactif :
./reading_cow
# Puis taper du texte, Ctrl+D pour terminer
```

### Lignes délicates dans reading_cow.c

**Lire caractère par caractère avec `fgetc`:**

```c
int ch;
while ((ch = fgetc(fp)) != EOF) {
```

Pourquoi `int ch` et pas `char ch` ?
`fgetc` retourne un `int` pouvant valoir `EOF` (généralement -1). Si on stockait
dans un `char` signé, -1 correspondrait au caractère `ÿ` et la comparaison
`ch != EOF` pourrait ne jamais être vraie, provoquant une boucle infinie.

**Gestion de la fin de fichier et de stdin:**

```c
if (fp != stdin) fclose(fp);
```

On ne ferme `stdin` que si on a ouvert un vrai fichier.

**Ignorer les `\r` (retours chariot Windows):**

```c
if (ch == '\r') continue;
```

Les fichiers créés sous Windows ont des fins de ligne `\r\n` (CRLF).
Sans ce filtre, le `\r` apparaîtrait dans la gueule de la vache.

**Affichage glissant de la bulle:**

```c
const char *display = (len > bub_len) ? swallowed + len - bub_len : swallowed;
```

Quand le texte avalé dépasse la largeur de la bulle, on affiche uniquement
les derniers `bub_len` caractères.

**La bulle minimale:**

```c
if (bub_len < 5) bub_len = 5;
```

On impose une largeur minimale de 5 pour que la bulle ait toujours une taille raisonnable.

**Initialisation sûre du buffer:**

```c
char swallowed[MAX_TEXT + 1];
memset(swallowed, 0, sizeof(swallowed));
```

`memset` remplit toute la zone mémoire avec des zéros, évitant les valeurs indéterminées.

---

## Difficultés rencontrées et choix techniques

### La vache "miroir" pour les animations

Dessiner une vache orientée à gauche impose de "retourner" manuellement le dessin ASCII.
La solution retenue est de maintenir deux versions du dessin dans `draw_cow` (un `if/else`
sur `direction`). Une alternative aurait été de stocker le dessin dans un tableau de
chaînes et de les inverser caractère par caractère, mais cela aurait été plus complexe.

### Absence d'effacement partiel

La technique choisie (`clear_screen()` + redessinage complet) est la plus simple mais
entraîne un léger scintillement sur certains terminaux. Une approche plus avancée
consisterait à n'effacer que les lignes occupées par la vache précédente, mais cela
complexifie considérablement le code.

### Dimensions du terminal en dur

```c
#define TERM_WIDTH  80
#define TERM_HEIGHT 24
```

La taille du terminal est codée en dur. La solution propre serait d'utiliser
`ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)` pour interroger les dimensions réelles
à l'exécution, mais cela introduit des dépendances système supplémentaires.

### Gestion des signaux (`Ctrl+C`)

Si l'utilisateur interrompt `wildcow` avec `Ctrl+C` en pleine animation, le curseur
reste caché. Pour corriger cela proprement, il faudrait installer un gestionnaire de
signal :

```c
#include <signal.h>
void handler(int sig) { show_cursor(); exit(0); }
signal(SIGINT, handler);
```

En pratique, on peut toujours restaurer le curseur avec `reset` dans le terminal.

### Pourquoi `fflush(stdout)` et pas `\n` pour vider le buffer ?

`printf` avec un `\n` flush automatiquement uniquement si `stdout` est connecté à un
terminal en mode line-buffered. Or, dans une animation où on positionne le curseur
avec `goto_xy`, on n'écrit pas toujours de `\n`. Il faut donc appeler `fflush`
explicitement.

---

# Partie Automates
Cette Dernière partie implemente le jeu du `Tamagoshi` avec une vache, a l'aide d'une structure d'automate et de 3 fichiers (C'est d'ailleurs la seule implémentation de ce projet qui necessitera l'utilisation de plusieurs fichiers).  


## Automate.h - Header Important
Avant de se lancer dans l'implémentation du `Tamagoshi`, il faut d'abord definir une structure `Automate` dans un fichier d'en-tete `Automate.h` pour pouvoir l'utiliser dans le fichier principal `Tamagoshi.c`.  

Pour cela, nous avons choisi de le faire avec un `struct`, ayant pour attributs `etat_courant`, `etat_suivant` et `fitness`.  

Or un automate a besoin d'une fonction de transition pour passer d'un etat a un autre.
On ecrit donc egalement dans cet en-tete le prototype de cette fonction `transition`.  

Au debut de l'en-tete, il faut s'assurer que l'en-tete ne sera inclus qu'une seule fois par les autres fichiers, donc on met la directive `#pragma once` au tout debut, qui a exactement le meme role que 
```
#ifndef AUTOMATE_H
#define AUTOMATE_H
...
#endif
```
mais en étant plus court.

## Automate.c - Implementation de la structure de l'Automate
Une fois le header crée, il faut maintenant implémenter la fonction de transition de l'automate dans un fichier source C `Automate.c`.  
On n'oublie pas d'inclure le header, puis on implemente `transition`, en passant bien en parametre **un pointeur** sur un Automate pour eviter d'ecraser les changements.  

NB : Ici l'etat suivant ne se calcule pas directement a partir de l'etat actuel (ce qui nous avais surpris) mais a partir de la variable `fitness`, qui en realité est liée a l'etat actuel de la vache.


## Tamagoshi.c - Programme principal
Il s'agit maintenant de coder le jeu principal dans `Tamagoshi.c` a partir de l'automate ainsi que de nouvelles fonctionalités.  

### Reprise de la Partie C
Puisque nous afficherons a nouveau une vache ici, nous avons repris les fonctions `affiche_bulle` et `affiche_vache` de la Partie C, comme conseillé dans l'enoncé du projet.  
Nous les utiliseront afin d'afficher des messages, ainsi que pour créer des visuels de l'etat actuel de la vache.  

### Idée Principale du Jeu
Outre les fonctionalités fondamentales du Tamagoshi decrites dans l'enoncé, nous avons eu quelques bonnes (ou pas :/) idées pour améliorer le jeu.  
  * Pour commencer, si la vache se sent bien (etat `LIFEROCKS`), elle propose au joueur un mini-jeu qui peut lui faire gagner deux bottes de foin.  
  * Ensuite, lorsque le joueur perd (ou dans d'autres cas spéciaux), le joueur débloque plusieurs fins ayant différents noms.  
  * Faire un titre ASCII stylé grace a ascii-art.com (bon au final c'est de l'UTF-8).  
  * Il y a aussi quelques `cheat-codes` qui permettent de débloquer des fins spéciales, ou un mode admin qui permet d'afficher la variable d'etat de santé de la vache (On vous les donnera un peu plus tard).  

*Fun Fact* : Arthur a eu l'idee de connecter un mini LLM de 100Mo a la Tamagoshi Cow pour pouvoir parler a la vache. On a cependant du oublier cette idée car le LLM était tellement léger qu'il répondait toujours 40km a coté de la plaque......UPDATE : IL L'A VRAIMENT MIS 💀💀

### Fonctionnement du Code
Premierement, on définit quelques fonctions auxilliaires qui nous seront tres utiles pour la suite de la gestion de l'automate, comme `lire_entree`.  
Ensuite la fonction `main` est une gestion assez classique d'automate:
 1.*On déclare/definit quelques variables utiles.  
 2.*On initialise l'Automate de sorte a ce que la vache soit en bonne santé au départ.  
 3.*Dans une boucle while on applique la fonction de transition, puis on actualise les variables ainsi que l'etat graphique de la vache a l'aide d'un `switch ... case ...`.  

### Format & Encodage
Dans ce programme comme dans celui de `wildcow.c`, on affiche a l'ecran des caracteres non `ASCII`.  
Or pour rappel les caracteres `UTF-8` ne sont pas supportés sur tous les terminaux de commande (notamment sur Windows).  
Pour y remédier, on réutilise
```c
system("chcp 65001");
```
afin de forcer les terminaux a supporter les caracteres `UTF-8`.  

Naturellement il faut bien veiller a sauvegarder `Tamagoshi.c` en `UTF-8` au risque d'avoir des caracteres bizarre a l'execution (au fait si vous avez des erreurs de ce type apres avoir téléchargé le projet essayez d'ouvrir les fichiers `.c` et `.h` et ré-enregistrez les manuellement en UTF-8).  

Si toute fois dans certains cas cela ne marche toujours pas (en particulier a cause de `gcc`), 
l'option de compilateur 
```
 -finput-charset=UTF-8
```
résoudra définitivement le probleme (enfin normalement).  

### Codes de triche & Options cachées  

Voici les Codes de triche que nous avons implémenté afin de rendre le jeu plus drole.  

| Code | Signification | Ou le mettre | Effet |
|---|---|---|---|
| 28082007 | Date d'anniversaire de riyad | Quand la vache est en surpoids | Débloque une fin spéciale |
| 07072007 | Date d'anniversaire d'Arthur | Pendant le mini-jeu | Débloque une autre fin spéciale |
| 1W4NTH4YB4L3 | *i want hay bale*, je veux du foin | Quand il faut nourrir la vache | Donne +5 foins |

En plus de ces codes, il existe une option qui active un mode administrateur.  
Si au moment de lancer le jeu vous mettez l'option
```
 ./Tamagoshi --mode-admin
```
le jeu se lance en mode administrateur, un mode que nous avons implémenté pour rendre les débugages moins pénibles. Avec ce mode activé, la variable `fitness` vous sera affichée a chaque tour, et pendant le mini-jeu, la réponse vous sera révélée. On a décidée de la laisser dans le programme, juste au cas ou vous auriez envie de tester.  

## Difficultés rencontrés
Bien que la partie automate ait été moins difficile que la partie C, nous avons quand meme rencontré quelques difficultés, dont voici les principales  

### Buffer statique buggé
Pour rappel, la fonction `lire_entree` est définie comme ceci:  

```c
char* lire_entree(const char* message){
    static char entree[100];
    printf("%s", message);
    fgets(entree, sizeof(entree), stdin);
    entree[strcspn(entree, "\n")] = 0;
    return entree;
}
```
On utilise ici un buffer statique (donc qui n'est pas réinitialisé a chaque appel) afin d'empecher un BufferOverflow. Cependant lorsque l'on utilise la fonction pour initialiser une chaine de caractere, la chaine initialisée changeait a chaque autre appel de `lire_entree` a cause du buffer statique.
La solution etait d'initialiser les chaines de caracteres par allocation dynamique, par exemple : 

```c
char* nom_vache = malloc(100);
strcpy(nom_vache, lire_entree("Quel nom donnerez vous a votre vache ? "));
```

### Fuite mémoire

Puisque nous avons utilisé l'allocation dynamique, nous devons libérer chaque élément initialisé par un `malloc` en mettant
```c
free(...); 
```
avant de terminer le programme.  

Cependant, certains évenements menait le programme a s'arreter avant l'arret standard de l'automate.
Ainsi si les éléments n'étaient pas égalements libérés avant ces arrets, cela causait une fuite de mémoire.  
Pour résoudre ce probleme il fallait simplement regarder tous les `return` de la fonction `main` et libérer la mémoire avant.  

### Caracteres UTF-8 non supportés

Comme expliqué plus tot, les séquences UTF-8 ne sont pas supportés sur tous les terminaux, ce qui conduisait a un affichage bizarre en console. La solution a ce probleme a déjà été détaillée plus haut.

### IA stupide 

Il s'avère que 87Mb pour un model LLM c'est très petit donc pour que ça fonctionne j'ai été obligé de fine-tuner le model ce qui as été particulierement compliqué (je plaide coupable j'ai fine-tuné avec python, en C c'était sucidaire)

### DOOM ?

A l'origine je voulais faire un jeu en 2d vue de dessus ou on affronte des vaches avec notre tamagoshi, et puis je me suis dis que la 3d c'est stylé, et puis que ça ressemblais beaucoup à DOOM et la je suis partit un peu loin. J'espere que vous allez passer un bon moment !  

---
## Message au lecteur :
Ce qui nous a été demandé dans les consignes du projet a déja été rempli au dessus. A partir de maintenant tout ce qui suit écoule des idées foireuses d'Arthur
---

# Partie IA

## Vue d'ensemble IA

La Partie IA fournit une inférence locale en C via `llama.cpp`. (normalement `.cpp` est une extension de fichiers source `C++`, mais ici ca marche tres bien en C)
Le programme principal est `IA/vache_folle.c` et charge un modèle GGUF pour
répondre en français à des prompts utilisateur. 

J'aime bien l'IA et je voulais faire un truc avec mais github m'a pas mal restreint. (Arthur Colin)

## Compilation IA

Depuis le dossier `IA/` :

```bash
make vache_folle
```

Le `Makefile` clone et compile `llama.cpp` automatiquement si nécessaire.

## Exécution IA

```bash
./vache_folle "Tu as faim ?"
```

En un seul morceau :

```bash
cd IA
make vache_folle
./vache_folle "Qui es-tu ?"
```

## Exemple de prompt : 
```text
Prompt   : Qui es-tu ?

Réponse  : Oui, je suis VYA, une vache Tamagotchi.


Prompt   : Ou est tu ?

Réponse  : Tu es ! Oui, tu es une vache Tamagotchi.


Prompt   : Explique moi la radioactivité avec des termes de vache

Réponse  : La radioactivité est un phénomène qui se produit lorsque
 la température de l'air change entre 24 et 35°C. 
Cette température aide à la perte d'énergie, à la récupération et à 
la récupération des muscles.
Tu peux dire que la radioactivité est un phénomène naturel, c'est une 
sensation qui peut varier un peu selon le type d'air.
Tu fais des sons de vache :
- **Vache**, **Tache**, **Tacho**, **Tacho-tache**, **Tacho-vache**, 
**Vache-tache**, **Tache-tache**, **Vache-tacho**, **Tache-vache**, 
**Vache-tache-tache**.
```

Le model est Falcon_FineTune.Q6_K.gguf dans le dossier /IA. 
Ce model est particulierement petit (87Mb) et donc possede une intélligence TRES TRES TRES limitée.
Pour éviter qu'il ne se mette à écrire du texte illisible dans un dialecte étrange (Espagnol) j'ai été forcé de le finetuner en python sur des exemples de phrases en français style "vache".
On va dire que cela à fonctionné à 50% voilà pourquoi le titre de vache folle :)  


## Connection LLM - Vache

Afin de connecter le mini LLM fine-tuné a notre vache (ici c'est riyad qui ecrit ca), on a eu l'idée de réutiliser les fonctions de `reading-cow`, car au final le procédé de lecture de `reading_cow` est assez semblable a celui de la génération de Chat-GPT.  
L'idee generale est que ce modele génere du texte par morceaux (un morceau par token), donc afin de faire réciter la sortie du LLM a la vache, on pouvait juste pipe la sortie de `vache_folle` dans `reading_cow`.....sauf que c'est penible vu que ca force a utiliser deux programmes juste pour ca.  
A la place, on réimplémente juste les fonctions de `reading_cow` dans `vache_folle`, et on affiche chaque fragment de la sortie du LLM dans la vache.  
Or il y avait encore un petit soucis technique :  ChatGPT (ou autre LLM c'est pareil) est codé en python de A a Z, donc sa génération est suffisament lente pour que le lecteur puisse la lire, car python est un langage a execution lente.  
Cependant ici notre programme C générait la sortie beaucoup trop rapidement, donc on ne pouvait rien lire (on n'arrivait meme pas a comprendre pourquoi tellement c'etait rapide).  
La solution était juste de mettre un `usleep` entre l'affichage de chaque caractere pour que la sortie soit visible, mais fluide.  

---

# COWDOOM

## Vue d'ensemble COWDOOM

COWDOOM est un prototype de mini-FPS ASCII en C inspiré de DOOM, dans lequel le joueur affronte des vagues de vaches ennemies. Le rendu pseudo-3D est obtenu par raycasting simplifié et affiché caractère par caractère dans une fenêtre SDL2.

Deux modes de jeu sont proposés au lancement via un menu interactif :
- **Mode Survie** : vagues infinies de vaches dont la vitesse augmente progressivement
- **Mode Attaque** : grande carte avec un nombre cible de vaches à éliminer, puis un boss final

Un curseur `>>>` indique clairement le mode sélectionné dans le menu.

## Installation COWDOOM

Dépendances requises (Linux) :

```bash
sudo apt update
sudo apt install -y libsdl2-dev pkg-config
```

## Compilation et lancement

```bash
cd COWDOOM
make
./cowdoom
```

Le jeu s'ouvre dans une fenêtre redimensionnable. La touche `F11` bascule en plein écran.
ATTENTION : prendre en main les déplacements est un peut contraignant il est normal de se faire massacrer au début. Aussi il n'est pas obligatoire de tirer pile sur l'ennemi le jeu est plustot généreux sur les hitbox. Et on ne juge PAS la qualitée de dessin de mes vaches (oui elles sont malades, même à l'article de la mort, et alors?)

## Commandes

| Touche | Action |
|---|---|
| `z` / `w` | Avancer |
| `s` | Reculer |
| `q` / `a` | Déplacement gauche (strafe) |
| `d` | Déplacement droite (strafe) |
| Flèche gauche / droite | Rotation caméra (la souris m'a fait pe*er un cable sur WSL) |
| Espace | Tirer |
| `r` | Recharger l'arme |
| Flèche haut / bas | Naviguer dans le menu |
| Entrée | Valider le mode dans le menu |
| `F11` | Plein écran / fenêtre |
| `x` / Échap | Quitter |

## Structure du code

| Fichier | Rôle |
|---|---|
| `src/main.c` | Boucle de jeu et timing |
| `src/input.c` | Lecture clavier SDL2 instantanée (sans validation par Entrée) |
| `src/game.c` | Logique, raycasting, rendu ASCII, mini-carte |
| `src/video.c` | Fenêtre SDL2 et affichage des caractères ASCII |
| `include/` | Interfaces (`game.h`, `input.h`, `video.h`) |
| `tools/cowdoom_mouse_lock.py` | Utilitaire de verrouillage souris |

---

## Conclusion

Le projet Cowsay nous a permis de developper nos compétences techniques en bash et en C, ainsi que notre capacité a travailler et a s'organiser pour le mener a bien. Arthur étant habitué du bash et Riyad ayant deja pas mal programmé en C/C++, nous avons pu mettre en oeuvres nos acquis afin de pousser le projet un peu (BEAUCOUP) plus loin dans les implemetations.  
On espere que vous avez apprécié car ca nous a pris...ENORMEMENT de temps pour faire ce projet.

Riyad BOUDALIA & Arthur COLIN, IMA04
