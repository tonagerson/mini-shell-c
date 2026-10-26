# Mini-Shell Unix (Interpréteur de commandes en C)

Implémentation d'un interpréteur de commandes Unix simplifié en **langage C**, réalisé dans le cadre de l'UE de Programmation Système (Licence Mathématiques-Informatique, Université Grenoble Alpes).

---

## Fonctionnalités

- **Boucle de commande (REPL) :** Lecture du buffer utilisateur (`INPUT_BUFFER_SIZE`), analyse lexicale (*tokenization*) et exécution.
- **Gestion des processus :** Création de processus fils via `fork()` et synchronisation du parent via `waitpid()`.
- **Exécution d'instructions :** Recouvrement d'espace mémoire avec `execvp()`.
- **Gestion d'erreurs :** Traitement des codes retour système (`errno`, `perror`).

---

## Aperçu du fonctionnement

<img width="478" height="267" alt="demo_shell" src="https://github.com/user-attachments/assets/7d9db3a0-9e0c-467a-8489-f900a29991cb" />


---

## Compilation et exécution

```bash
# Compilation via le Makefile
make

# Lancement de l'interpréteur
./shell2
