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

struct Enemy{
    char x;
    char y;
    int found;
    char unit;
    char priority;
};

struct Leaf{
    char x;
    char y;
    int previous;
    unsigned char rank;
    char priority;
    char unit;
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
    
    // Recherche des unités alliées
    int allyUnitsNumber = 0;
    struct Position * allyUnitsTab = (struct Position *) malloc(allyUnitsNumber * sizeof(struct Position));
    if (!allyUnitsTab){ // Securisation
        printf("Dynamic allocation error !");
        return EXIT_FAILURE;
    }

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 1 || unitsArray[(j*X)+i] == 2 || unitsArray[(j*X)+i] == 3){
                allyUnitsNumber++;
                allyUnitsTab = (struct Position *) realloc(allyUnitsTab, allyUnitsNumber * sizeof(struct Position));
                if (!allyUnitsTab){ // Securisation
                    printf("Dynamic allocation error !");
                    return EXIT_FAILURE;
                }
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
    struct Enemy * enemyUnitsTab = (struct Enemy *) malloc(enemiesNumber * sizeof(struct Enemy));
    if (!enemyUnitsTab){ // Securisation
        printf("Dynamic allocation error !");
        return EXIT_FAILURE;
    }

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 4 || unitsArray[(j*X)+i] == 5 || unitsArray[(j*X)+i] == 6){
                enemiesNumber++;
                enemyUnitsTab = (struct Enemy *) realloc(enemyUnitsTab, enemiesNumber * sizeof(struct Enemy));
                if (!enemyUnitsTab){ // Securisation
                    printf("Dynamic allocation error !");
                    return EXIT_FAILURE;
                }
                enemyUnitsTab[enemiesNumber-1].x = i;
                enemyUnitsTab[enemiesNumber-1].y = j;
                enemyUnitsTab[enemiesNumber-1].found = -1;
                enemyUnitsTab[enemiesNumber-1].priority = 99;
                enemyUnitsTab[enemiesNumber-1].unit = -1;
            }
        }
    }
    if (enemiesNumber == 0){
        printf("\nThere is no enemy");
        return EXIT_SUCCESS;
    }

    // Nombre de déplacements
    char moveNber = (STAMINA - (enemiesNumber*STAMINAATTACK))/STAMINAMOVE;

    // Arbre des déplacements
    int currentPosition = allyUnitsNumber-1;
    int previousPosition = 0;
    unsigned char rank = 0;
    bool found = false;

    // Création du tableau de l'arbre
    struct Leaf * tree = (struct Leaf *) malloc(allyUnitsNumber * sizeof(struct Leaf));
    if (!tree){ // Securisation
        printf("Dynamic allocation error !");
        return EXIT_FAILURE;
    }

    // Création du tronc
    for (int i = 0; i < allyUnitsNumber; i++){
        tree[i].x = allyUnitsTab[i].x;
        tree[i].y = allyUnitsTab[i].y;
        tree[i].previous = -1;
        tree[i].rank = 0;
        tree[i].priority = 0;
        tree[i].unit = i;
    }

    while (found == false && rank <= moveNber){
        while (tree[previousPosition].rank == rank){
            if (tree[previousPosition].y - 1 >= 0){
                if (unitsArray[(tree[previousPosition].y - 1)*X + tree[previousPosition].x] != 7 && unitsArray[(tree[previousPosition].y - 1)*X + tree[previousPosition].x] != 8){
                    currentPosition++;

                    tree = (struct Leaf *) realloc(tree, (currentPosition+allyUnitsNumber) * sizeof(struct Leaf));
                    if (!tree){ // Securisation
                        printf("Dynamic allocation error !");
                        return EXIT_FAILURE;
                    }

                    tree[currentPosition].x = tree[previousPosition].x;
                    tree[currentPosition].y = tree[previousPosition].y - 1;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    tree[currentPosition].unit = tree[previousPosition].unit;

                    for (int i = 0; i < enemiesNumber; i++){
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y){
                            if (tree[currentPosition].priority < enemyUnitsTab[i].priority && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                                enemyUnitsTab[i].found = currentPosition;
                                enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                tree[previousPosition].priority = 0;
                            }
                        }
                    }
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                }
            }
            if (tree[previousPosition].y + 1 < Y){
                if (unitsArray[(tree[previousPosition].y + 1)*X + tree[previousPosition].x] != 7 && unitsArray[(tree[previousPosition].y + 1)*X + tree[previousPosition].x] != 8){
                    currentPosition++;

                    tree = (struct Leaf *) realloc(tree, (currentPosition+allyUnitsNumber) * sizeof(struct Leaf));
                    if (!tree){ // Securisation
                        printf("Dynamic allocation error !");
                        return EXIT_FAILURE;
                    }

                    tree[currentPosition].x = tree[previousPosition].x;
                    tree[currentPosition].y = tree[previousPosition].y + 1;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    tree[currentPosition].unit = tree[previousPosition].unit;

                    for (int i = 0; i < enemiesNumber; i++){
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y){
                            if (tree[currentPosition].priority < enemyUnitsTab[i].priority && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                                enemyUnitsTab[i].found = currentPosition;
                                enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                tree[previousPosition].priority = 0;
                            }
                        }
                    }
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                }
            }
            if (tree[previousPosition].x + 1 < X){
                if (unitsArray[tree[previousPosition].y*X + tree[previousPosition].x + 1] != 7 && unitsArray[tree[previousPosition].y*X + tree[previousPosition].x + 1] != 8){
                    currentPosition++;

                    tree = (struct Leaf *) realloc(tree, (currentPosition+allyUnitsNumber) * sizeof(struct Leaf));
                    if (!tree){ // Securisation
                        printf("Dynamic allocation error !");
                        return EXIT_FAILURE;
                    }

                    tree[currentPosition].x = tree[previousPosition].x + 1;
                    tree[currentPosition].y = tree[previousPosition].y;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    tree[currentPosition].unit = tree[previousPosition].unit;

                    for (int i = 0; i < enemiesNumber; i++){
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y){
                            if (tree[currentPosition].priority < enemyUnitsTab[i].priority && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                                enemyUnitsTab[i].found = currentPosition;
                                enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                tree[previousPosition].priority = 0;
                            }
                        }
                    }
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                }
            }
            if (tree[previousPosition].x - 1 >= 0){
                if (unitsArray[tree[previousPosition].y*X + tree[previousPosition].x - 1] != 7 && unitsArray[tree[previousPosition].y*X + tree[previousPosition].x - 1] != 8){
                    currentPosition++;

                    tree = (struct Leaf *) realloc(tree, (currentPosition+allyUnitsNumber) * sizeof(struct Leaf));
                    if (!tree){ // Securisation
                        printf("Dynamic allocation error !");
                        return EXIT_FAILURE;
                    }
                    
                    tree[currentPosition].x = tree[previousPosition].x - 1;
                    tree[currentPosition].y = tree[previousPosition].y;
                    tree[currentPosition].previous = previousPosition;
                    tree[currentPosition].rank = rank + 1;
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    tree[currentPosition].unit = tree[previousPosition].unit;

                    for (int i = 0; i < enemiesNumber; i++){
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y){
                            if (tree[currentPosition].priority < enemyUnitsTab[i].priority && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                                enemyUnitsTab[i].found = currentPosition;
                                enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                tree[previousPosition].priority = 0;
                            }
                        }
                    }
                    tree[currentPosition].priority = tree[previousPosition].priority + 1;
                }
            }

            previousPosition++;

            /*for (int i = 0; i < enemiesNumber; i++){
                if (enemyUnitsTab[i].found == -1){
                    found = false;
                    break;
                }
                else{
                    found = true;
                }
            }

            if (found == true){
                break;
            }*/
        }
        rank++;
    }

    // Affichage
    // Si il y a une solution
    struct Leaf tmp;
    /*if (found == true){
    }
    // Si il n'y a pas de solution
    else{
        printf("\nIl n'y a pas de solution !");
    }*/

    for (int i = 0; i < enemiesNumber; i++){
        tmp = tree[enemyUnitsTab[i].found];
        printf("\n\nUnité : %d", i+1);
        while (tmp.previous != -1){
            tmp = tree[tmp.previous];
            printf("\nposition : (%d,%d)", tmp.x, tmp.y);
        }
    }

    return EXIT_SUCCESS;
}