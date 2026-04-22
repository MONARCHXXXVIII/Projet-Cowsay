# Projet Cowsay - Partie Bash

Ce répertoire contient une collection de scripts bash qui utilisent la commande `cowsay` pour faire afficher des séries de nombres par une vache ASCII.

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

## Prérequis

- Bash shell
- Commande `cowsay` installée sur le système
- Permissions d'exécution sur les scripts (déjà configurées)

## Installation de cowsay

Si `cowsay` n'est pas installé:

**Sur Ubuntu/Debian:**
```bash
sudo apt-get install cowsay
```

**Sur macOS:**
```bash
brew install cowsay
```

---

## Notes d'implémentation

- Tous les scripts utilisent `clear` pour rafraîchir l'écran à chaque itération
- Tous les scripts utilisent `sleep 1` pour une pause d'une seconde
- La validation des arguments est implémentée dans les scripts qui en ont besoin
- Les scripts utilisent des approches différentes pour les boucles (for, while) selon les besoins
- Le calcul du résultat dans `smart_cow` utilise l'expansion arithmétique bash `$(( ))`

---

## Auteur

Projet réalisé dans le cadre du projet Cowsay.
