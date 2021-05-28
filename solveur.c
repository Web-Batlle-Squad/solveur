#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Solveur
// lit un fichier map.txt
// renvoie les positions de chaques déplacements

#define X 15
#define Y 10
#define STAMINA 30
#define STAMINAMOVE 2
#define STAMINAATTACK 4
#define FILENAME "map.txt"

struct Position{
    char x;
    char y;
};

struct Leaf{
    char x;
    char y;
    int previous;
    unsigned char rank;
};

int main(void){
    // Lecture du fichier
    unsigned char unitsArray[X*Y];
    FILE * file = fopen(FILENAME, "r");

    if (file){
        char ch;
        int i = 0;
        while((ch = fgetc(file)) != EOF)
        {
            if (ch >= 48 && ch <= 57){
                unitsArray[i++] = ch-48;
            }
        }
        fclose(file);
    } else {
        printf("\nError : Can't open the file");
        return EXIT_FAILURE;
    }
    
    // Recherche des unités alliées         /!\ Déplacement de chevalier pour toutes les unitées
    int allyUnitsNumber = 0;
    struct Position * allyUnitsTab = (struct Position *) malloc(allyUnitsNumber * sizeof(struct Position));

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 1 || unitsArray[(j*X)+i] == 2 || unitsArray[(j*X)+i] == 3){
                allyUnitsNumber++;
                allyUnitsTab = (struct Position *) realloc(allyUnitsTab, allyUnitsNumber * sizeof(struct Position)); // sécurisation !!!!!!!!!!
                allyUnitsTab[allyUnitsNumber-1].x = i;
                allyUnitsTab[allyUnitsNumber-1].y = j;
            }
        }
    }
    if (allyUnitsNumber == 0){
        printf("\nThere is no ally");
        return EXIT_SUCCESS;
    }

    // Recherche des unités ennemis
    int enemiesNumber = 0;
    struct Position * enemyUnitsTab = (struct Position *) malloc(enemiesNumber * sizeof(struct Position));

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 4 || unitsArray[(j*X)+i] == 5 || unitsArray[(j*X)+i] == 6){
                enemiesNumber++;
                enemyUnitsTab = (struct Position *) realloc(enemyUnitsTab, enemiesNumber * sizeof(struct Position)); // sécurisation !!!!!!!!!!
                enemyUnitsTab[enemiesNumber-1].x = i;
                enemyUnitsTab[enemiesNumber-1].y = j;
            }
        }
    }
    if (enemiesNumber == 0){
        printf("\nThere is no enemy");
        return EXIT_SUCCESS;
    }

    // Nombre de déplacements
    unsigned char moveNber = (STAMINA - (enemiesNumber*STAMINAATTACK))/STAMINAMOVE;

    // Arbre des déplacements
    int currentPosition = 0;
    int previousPosition = 0;
    unsigned char rank = 0;
    bool found = false;

    // Création du tableau de l'arbre
    struct Leaf * tree = (struct Leaf *) malloc((currentPosition+1) * sizeof(struct Leaf));
    if (!tree){
        printf("\nError : Can't create an array");
        return EXIT_FAILURE;
    }

    tree[currentPosition].x = allyUnitsTab[0].x;     // /!\ seulement 1 unité alliée
    tree[currentPosition].y = allyUnitsTab[0].y;
    tree[currentPosition].previous = -1;
    tree[currentPosition].rank = 0;

    while (found == false){
        while (tree[previousPosition].rank == rank){
            if (tree[previousPosition].y - 1 >= 0){
                if (unitsArray[(tree[previousPosition].y - 1)*X + tree[previousPosition].x] != 7 && unitsArray[(tree[previousPosition].y - 1)*X + tree[previousPosition].x] != 8){
                    currentPosition++;
                    tree = (struct Leaf *) realloc(tree, (currentPosition+1) * sizeof(struct Leaf));
                    tree[currentPosition].x = tree[previousPosition].x;
                    tree[currentPosition].y = tree[previousPosition].y - 1;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;

                    if (tree[currentPosition].x == enemyUnitsTab[0].x && tree[currentPosition].y == enemyUnitsTab[0].y){      // /!\ seulement 1 unité ennemie
                        found = true;
                        break;
                    }
                }
            }
            if (tree[previousPosition].x + 1 < X){
                if (unitsArray[tree[previousPosition].y*X + tree[previousPosition].x + 1] != 7 && unitsArray[tree[previousPosition].y*X + tree[previousPosition].x + 1] != 8){
                    currentPosition++;
                    tree = (struct Leaf *) realloc(tree, (currentPosition+1) * sizeof(struct Leaf));
                    tree[currentPosition].x = tree[previousPosition].x + 1;
                    tree[currentPosition].y = tree[previousPosition].y;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;

                    if (tree[currentPosition].x == enemyUnitsTab[0].x && tree[currentPosition].y == enemyUnitsTab[0].y){
                        found = true;
                        break;
                    }
                }
            }
            if (tree[previousPosition].y + 1 < Y){
                if (unitsArray[(tree[previousPosition].y + 1)*X + tree[previousPosition].x] != 7 && unitsArray[(tree[previousPosition].y + 1)*X + tree[previousPosition].x] != 8){
                    currentPosition++;
                    tree = (struct Leaf *) realloc(tree, (currentPosition+1) * sizeof(struct Leaf));
                    tree[currentPosition].x = tree[previousPosition].x;
                    tree[currentPosition].y = tree[previousPosition].y + 1;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;

                    if (tree[currentPosition].x == enemyUnitsTab[0].x && tree[currentPosition].y == enemyUnitsTab[0].y){
                        found = true;
                        break;
                    }
                }
            }
            if (tree[previousPosition].x - 1 >= 0){
                if (unitsArray[tree[previousPosition].y*X + tree[previousPosition].x - 1] != 7 && unitsArray[tree[previousPosition].y*X + tree[previousPosition].x - 1] != 8){
                    currentPosition++;
                    tree = (struct Leaf *) realloc(tree, (currentPosition+1) * sizeof(struct Leaf));
                    tree[currentPosition].x = tree[previousPosition].x - 1;
                    tree[currentPosition].y = tree[previousPosition].y;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;

                    if (tree[currentPosition].x == enemyUnitsTab[0].x && tree[currentPosition].y == enemyUnitsTab[0].y){
                        found = true;
                        break;
                    }
                }
            }
            previousPosition++;
        }
        rank++;
    }

    // Affichage
    // Si il y a une solution
    struct Leaf tmp = tree[currentPosition];
    if (tree[currentPosition].x == enemyUnitsTab[0].x && tree[currentPosition].y == enemyUnitsTab[0].y){
        while (tmp.previous != -1){
            tmp = tree[tmp.previous];
            printf("\nposition : (%d,%d)", tmp.x, tmp.y);
        }
    }
    // Si il n'y a pas de solution
    else{
        printf("\nIl n'y a pas de solution !");
    }

    return EXIT_SUCCESS;
}