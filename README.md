Partie C — Projet Cowsay · INF203 · UGA 2021-2022
Ce README documente l'intégralité de la partie C du projet Cowsay.
Il couvre l'architecture des trois programmes, les choix d'implémentation,
les lignes de code délicates, et des exemples complets d'exécution.
---
Table des matières
Compilation
newcow.c — Recoder cowsay en C
wildcow.c — La vache animée
reading_cow.c — La vache qui lit
Difficultés rencontrées et choix techniques
---
1. Compilation
Tous les programmes se compilent avec le `Makefile` fourni :
```bash
make all        # compile newcow, wildcow, reading_cow
make clean      # supprime les binaires
```
Ou individuellement :
```bash
gcc -Wall -Wextra -std=c99 -o newcow       newcow.c
gcc -Wall -Wextra -std=c99 -o wildcow      wildcow.c
gcc -Wall -Wextra -std=c99 -o reading_cow  reading_cow.c
```
> **Note sur `-std=c99`** : on utilise le standard C99 pour pouvoir déclarer
> des variables au milieu d'un bloc (ex. `int i` dans un `for`), ce que C89
> interdit. Sans ce flag, gcc peut refuser certaines déclarations.
---
2. newcow.c — Recoder cowsay en C
2.1 Vue d'ensemble
`newcow` reproduit le comportement de base de `cowsay` et y ajoute plusieurs
options originales. Son architecture repose sur trois fonctions principales :
`affiche_bulle()` — dessine la bulle de texte au-dessus de la vache
`affiche_vache()` — dessine le corps de la vache avec paramètres variables
`affiche_troupeau()` — affiche N vaches à la suite (option `--herd`)
2.2 Options disponibles
Option courte	Option longue	Argument	Description
`-e`	`--eyes`	`XX`	Remplace les yeux `oo` par deux caractères
`-T`	`--tongue`	`XX`	Remplace la langue (deux caractères)
`-t`	`--tail`	`N`	Allonge la queue de N espaces
`-W`	`--wrap`	`N`	Largeur de retour à la ligne (défaut : 40)
—	`--think`	—	Bulle de pensée (flèches en `o` au lieu de `\`)
—	`--herd`	`N`	Affiche un troupeau de N vaches
`-h`	`--help`	—	Affiche l'aide
2.3 Exemples d'exécution
Exemple 1 — appel basique
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
Exemple 2 — yeux personnalisés
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
Exemple 3 — queue allongée
```
$ ./newcow --tail 6 "Longue queue !"
        \  ^__^
         \ (oo)\_______
           (__)\       )\/\/\/\/\/\/\
              ||----w |
              ||     ||
```
Exemple 4 — mode pensée
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
Exemple 5 — troupeau
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
Exemple 6 — texte long avec retour à la ligne
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
> Cela reproduit exactement le comportement de `cowsay` original.
2.4 Lignes délicates dans newcow.c
La fonction `affiche_bulle` et les formats `printf`
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
`-` : alignement à gauche
`*` : largeur minimale fournie par `line_len`
`.*s` : largeur maximale (tronque si trop long) aussi fournie par `line_len`
Le `\\` en fin produit un seul backslash `\` dans la sortie (le backslash
doit toujours être échappé dans une chaîne C).
Parsing des arguments avec `argc/argv`
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
L'incrémentation de `i` "consomme" l'argument suivant pour qu'il ne soit pas
réinterprété comme un autre flag.
Le paramètre `__attribute__((unused))`
```c
void affiche_bulle(const char *texte, int wrap, int think __attribute__((unused)))
```
Le paramètre `think` est prévu pour une évolution future (bulle de pensée
dans `affiche_bulle` elle-même) mais n'est pas encore utilisé dans cette
fonction. Sans `__attribute__((unused))`, `gcc -Wextra` émet un warning.
C'est une façon propre de signaler l'intention sans supprimer les warnings
globalement.
Copie sécurisée des yeux et de la langue
```c
char e[3] = "oo";
if (eyes && strlen(eyes) >= 2) { e[0] = eyes[0]; e[1] = eyes[1]; }
```
On vérifie d'abord que le pointeur `eyes` n'est pas `NULL` (guard `eyes &&`)
puis que la chaîne fait au moins 2 caractères avant d'y accéder. Accéder à
`eyes[1]` sans cette vérification provoquerait un comportement indéfini si
la chaîne est vide. Les deux caractères sont copiés dans un tableau local
`e[3]` (taille 3 pour le `\0` final), ce qui isole la vache du contenu
potentiellement long fourni par l'utilisateur.
---
3. wildcow.c — La vache animée
3.1 Vue d'ensemble
`wildcow` utilise les séquences d'échappement ANSI pour manipuler le
curseur du terminal et produire des animations frame par frame. Quatre modes
sont disponibles.
3.2 Les séquences ANSI : fonctionnement
```c
void clear_screen() { printf("\033[H\033[J"); fflush(stdout); }
void goto_xy(int x, int y) { printf("\033[%d;%dH", y, x); fflush(stdout); }
void hide_cursor()  { printf("\033[?25l"); fflush(stdout); }
void show_cursor()  { printf("\033[?25h"); fflush(stdout); }
```
`\033` est le caractère ESC (code ASCII 27) en notation octale.
Les séquences ANSI sont de la forme `ESC [ ... lettre` :
Séquence	Effet
`\033[H\033[J`	`H` = curseur en haut à gauche ; `J` = efface l'écran vers le bas
`\033[L;CH`	Positionne le curseur à la ligne `L`, colonne `C`
`\033[?25l`	Cache le curseur (lettre `l` = lower, désactive)
`\033[?25h`	Affiche le curseur (lettre `h` = high, active)
> **Pourquoi `fflush(stdout)` systématiquement ?**
> `printf` utilise un buffer interne. Sans `fflush`, les caractères peuvent
> rester en mémoire et ne pas s'afficher immédiatement. Dans une animation,
> cela provoquerait des "sauts" ou un écran vide jusqu'au prochain flush
> automatique. On force donc le flush après chaque opération visuelle.
> **Pourquoi cacher le curseur ?**
> Sans `hide_cursor()`, le curseur clignotant saute d'une position à l'autre
> à chaque frame et crée un scintillement désagréable. On le cache au début
> et on le restaure à la fin avec `show_cursor()`.
3.3 La macro `_DEFAULT_SOURCE`
```c
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
```
`usleep()` (pause en microsecondes) n'est pas dans le standard C99 strict :
c'est une extension POSIX/BSD. Sans `_DEFAULT_SOURCE`, gcc en mode strict
(`-std=c99`) refuse de déclarer `usleep` et émet une erreur implicit
declaration. Cette macro "déverrouille" les extensions POSIX dans les
en-têtes système. Elle doit impérativement être placée avant tout
`#include`, sinon les en-têtes ont déjà été lus sans elle.
3.4 Les couleurs ANSI
```c
const char *colors[] = {"\033[1;33m", "\033[1;31m", "\033[0;37m"};
// puis à l'usage :
printf("%s%s\033[0m", colors[f], frames[f]);
```
Format : `\033[ STYLE ; COULEUR m`
Code	Signification
`0`	Réinitialise tous les attributs
`1`	Gras
`31`	Rouge
`33`	Jaune
`35`	Magenta
`36`	Cyan
`37`	Blanc/gris
Le `\033[0m` à la fin de chaque `printf` coloré réinitialise la couleur.
Sans lui, tout le texte qui suit serait affiché dans la couleur courante.
3.5 Modes et exemples d'exécution
Mode par défaut — marche droite → gauche
```bash
$ ./wildcow
```
La vache apparaît côté droit et traverse l'écran vers la gauche. Elle est
dessinée "orientée gauche" (miroir) pour que son regard soit cohérent avec
sa direction de marche.
```
            ^__^  /
    _______/(oo) /
/\/(       /(__)
    | w----||
    ||     ||
```
À chaque frame, `x` décroît de 1 et l'écran est effacé + redessiné :
```c
for (int x = TERM_WIDTH - COW_WIDTH; x >= 1; x--) {
    clear_screen();
    draw_cow(x, y, -1, "oo", -1);
    usleep(80000);  // 80 ms entre chaque frame → ~12 fps
}
```
Mode `-hee_hee` — Moonwalk
```bash
$ ./wildcow -hee_hee
```
L'effet moonwalk repose sur un paradoxe visuel : la vache regarde à gauche
(dessin miroir, `direction = -1`) mais avance vers la droite (`x` croît).
C'est exactement ce que fait Michael Jackson : le corps recule pendant que les
pieds semblent avancer. Le texte `♪ hee hee ! ♪` s'affiche en magenta à
chaque frame. La séquence se termine sur une pose finale avec les yeux `**`
et la légende "Michael Moo-Jackson".
```c
for (int x = 1; x <= TERM_WIDTH - COW_WIDTH; x++) {
    draw_cow(x, y, -1, "oo", -1);  // direction -1 = gauche, x croît = droite
    usleep(70000);
}
```
Mode `-boom` — Décompte et explosion
```bash
$ ./wildcow -boom
```
Phase 1 — La vache entre en marchant depuis la droite jusqu'au centre.
Phase 2 — La fonction `draw_countdown(5, x, y)` lance le décompte.
À chaque seconde, la mèche raccourcit d'un `~` :
```c
// Appel avec i-1 : quand i=5, fuse=4 ; quand i=1, fuse=0
draw_cow(x, y, 1, "oo", i - 1);
```
La mèche est dessinée ainsi dans `draw_cow` :
```c
if (boom_fuse >= 0) {
    goto_xy(x + 17, y - 1);
    printf("o");
    for (int i = 0; i < boom_fuse; i++) printf("~");
    printf("*");
}
```
Sortie au décompte 3 :
```
                  o~~*
        \  ^__^
 EXPLOSION DANS 3...
```
Phase 3 — `draw_explosion` affiche 3 frames successives en couleurs
différentes (jaune → rouge → gris) avec une pause de 400 ms entre chacune,
pour simuler un flash d'explosion.
Phase 4 — Écran final en rouge : `*** MOO IN PEACE ***`.
Mode `-go_crazy` — Va-et-vient fou
```bash
$ ./wildcow -go_crazy
```
Phase 1 (200 steps) — La vitesse augmente progressivement :
```c
speed = 50000 - (steps * 200);
if (speed < 5000) speed = 5000;  // plancher à 5ms (200 fps max)
```
La direction change aux bords ET aléatoirement avec une probabilité de 1/15 :
```c
} else if (rand() % 15 == 0) {
    dir = -dir;
}
```
`rand() % 15 == 0` : sur 15 tirages, environ 1 provoquera un demi-tour
inattendu. Plus `max_steps` est grand, plus la vache devient folle longtemps.
Les yeux sont tirés au hasard parmi 8 options :
```c
const char *crazy_eyes[] = {"@@", "**", "OO", "^^", "??", "!!", "##", "oo"};
int ei = rand() % 8;
```
Phase 2 — La vache s'immobilise avec les yeux `xx` :
```c
draw_cow(final_x, y, 1, "xx", -1);
```
Sortie finale :
```
  x_x  ~  trop de vertige...  ~  x_x

        \  ^__^
         \ (xx)\_______
           (__)\       )\/\
               ||----w |
               ||     ||

  *tourne en rond*
```
3.6 Initialisation du générateur aléatoire
```c
srand((unsigned)time(NULL));
```
`rand()` produit une séquence déterministe : si on ne l'initialise pas,
le programme donnera toujours la même séquence de nombres à chaque exécution.
`srand(time(NULL))` initialise la graine avec l'heure courante (secondes
depuis le 1er janvier 1970), ce qui garantit une séquence différente à
chaque lancement. Le cast `(unsigned)` évite un warning sur les plateformes
où `time_t` est signé et `srand` attend un `unsigned int`.
---
4. reading_cow.c — La vache qui lit
4.1 Vue d'ensemble
`reading_cow` lit un fichier (ou stdin) caractère par caractère avec
`fgetc`. Chaque caractère est d'abord affiché dans la "gueule" de la vache,
puis "avalé" et ajouté à la bulle de texte. Une pause d'une seconde sépare
chaque caractère.
4.2 Exemples d'exécution
Préparation :
```bash
echo "bonjour" > fichier.txt
./reading_cow fichier.txt
```
Étape 1 — La vache montre le `b` dans sa gueule, bulle vide :
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
Étape 4 — La vache a avalé `bon`, affiche `j` dans sa gueule :
```
 _______
< bon   >
 -------
        \  ^__^
         \ (oo)\_______
           (__)\       )\/\
            j  ||----w |
               ||     ||

 Caractères lus : 4
```
Fin — Tout avalé, yeux satisfaits `^^` :
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
Lecture depuis stdin :
```bash
echo "moo" | ./reading_cow
# ou interactif :
./reading_cow
# Puis taper du texte, Ctrl+D pour terminer
```
4.3 Lignes délicates dans reading_cow.c
Lire caractère par caractère avec `fgetc`
```c
int ch;
while ((ch = fgetc(fp)) != EOF) {
```
Pourquoi `int ch` et pas `char ch` ?
`fgetc` retourne un `int` pouvant valoir `EOF` (généralement -1). Si on
stockait le résultat dans un `char`, et que `char` est signé sur la
plateforme, la valeur -1 correspondrait au caractère `ÿ` (255 en unsigned)
et la comparaison `ch != EOF` pourrait ne jamais être vraie, provoquant une
boucle infinie. Utiliser `int` évite ce piège classique du C.
Gestion de la fin de fichier et de stdin
```c
if (fp != stdin) fclose(fp);
```
On ne ferme `stdin` que si on a ouvert un vrai fichier. Fermer `stdin`
explicitement n'est pas une erreur en soi, mais c'est une bonne pratique
de ne pas toucher aux descripteurs standards qu'on n'a pas ouverts soi-même.
Ignorer les `\r` (retours chariot Windows)
```c
if (ch == '\r') continue;
```
Les fichiers créés sous Windows ont des fins de ligne `\r\n` (CRLF).
Sans ce filtre, le `\r` apparaîtrait dans la gueule de la vache et dans la
bulle, produisant un affichage incorrect (retour au début de la ligne sans
saut de ligne). On l'ignore silencieusement.
Affichage glissant de la bulle
```c
const char *display = (len > bub_len) ? swallowed + len - bub_len : swallowed;
```
Quand le texte avalé dépasse la largeur de la bulle (`BUBBLE_W = 40`), on
affiche uniquement les derniers `bub_len` caractères. On avance le
pointeur `display` de `len - bub_len` positions dans la chaîne, ce qui
revient à "décaler une fenêtre" sur les caractères récents. C'est une
technique d'arithmétique de pointeurs : `swallowed + k` pointe sur le
k-ième caractère du tableau.
La bulle minimale
```c
if (bub_len < 5) bub_len = 5;
```
Sans ce plancher, si le texte avalé est vide ou très court (1-2 caractères),
la bulle serait minuscule et l'affichage serait laid. On impose une largeur
minimale de 5 pour que la bulle ait toujours une taille raisonnable dès le
début de la lecture.
Initialisation sûre du buffer
```c
char swallowed[MAX_TEXT + 1];
memset(swallowed, 0, sizeof(swallowed));
```
`memset` remplit toute la zone mémoire avec des zéros. Sans cela, le tableau
contiendrait des valeurs indéterminées ("garbage") et un `printf("%s", swallowed)`
lirait des octets aléatoires jusqu'au premier zéro rencontré par hasard,
avec risque de segfault. `sizeof(swallowed)` vaut `MAX_TEXT + 1` = 257 octets,
incluant le `\0` terminal.
---
5. Difficultés rencontrées et choix techniques
5.1 La vache "miroir" pour les animations
Dessiner une vache orientée à gauche impose de "retourner" manuellement
le dessin ASCII, car il n'existe pas de fonction standard pour miroir du
texte. La solution retenue est de maintenir deux versions du dessin dans
`draw_cow` (un `if/else` sur `direction`). Une alternative aurait été de
stocker le dessin dans un tableau de chaînes et de les inverser
caractère par caractère, mais cela aurait été plus complexe pour peu de
gain.
5.2 Absence d'effacement partiel
La technique choisie (`clear_screen()` + redessinage complet) est la plus
simple mais aussi la plus basique : elle entraîne un léger scintillement
sur certains terminaux. Une approche plus avancée consisterait à
n'effacer que les lignes occupées par la vache précédente grâce à des
séquences ANSI de déplacement de curseur, mais cela complexifie
considérablement le code pour un résultat marginal dans le cadre du projet.
5.3 Dimensions du terminal en dur
```c
#define TERM_WIDTH  80
#define TERM_HEIGHT 24
```
La taille du terminal est codée en dur. La solution propre serait d'utiliser
`ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)` pour interroger les dimensions réelles
à l'exécution. Ce mécanisme (`<sys/ioctl.h>`) a été écarté pour rester dans
le cadre du cours et ne pas introduire de dépendances système supplémentaires.
Si le terminal est plus large que 80 colonnes, les animations fonctionnent
correctement mais n'utilisent pas toute la largeur disponible.
5.4 Gestion des signaux (`Ctrl+C`)
Si l'utilisateur interrompt `wildcow` avec `Ctrl+C` en pleine animation, le
curseur reste caché (car `show_cursor()` n'a pas été appelé). Pour corriger
cela proprement, il faudrait installer un gestionnaire de signal :
```c
#include <signal.h>
void handler(int sig) { show_cursor(); exit(0); }
signal(SIGINT, handler);
```
Ce n'est pas implémenté ici pour rester concis, mais c'est une amélioration
possible. En pratique, on peut toujours restaurer le curseur avec `reset`
dans le terminal.
5.5 Pourquoi `fflush(stdout)` et pas `\n` pour vider le buffer ?
`printf` avec un `\n` flush automatiquement uniquement si `stdout` est
connecté à un terminal en mode line-buffered. Or, dans une animation
où on positionne le curseur avec `goto_xy`, on n'écrit pas toujours de `\n`.
Il faut donc appeler `fflush` explicitement pour garantir l'affichage
immédiat de chaque frame.
---
