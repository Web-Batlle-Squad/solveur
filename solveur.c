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

struct Ally{
    char x;
    char y;
    char class;
    unsigned char priority;
};

struct Enemy{
    char x;
    char y;
    int found;
    char unit;
    char class;
    unsigned char priority;
};

struct Leaf{
    char x;
    char y;
    int previous;
    unsigned char rank;
    unsigned char priority;
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
    struct Ally * allyUnitsTab = (struct Ally *) malloc(allyUnitsNumber * sizeof(struct Ally));
    if (!allyUnitsTab){ // Securisation
        printf("Dynamic allocation error !");
        return EXIT_FAILURE;
    }

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 1 || unitsArray[(j*X)+i] == 2 || unitsArray[(j*X)+i] == 3){
                allyUnitsNumber++;
                allyUnitsTab = (struct Ally *) realloc(allyUnitsTab, allyUnitsNumber * sizeof(struct Ally));
                if (!allyUnitsTab){ // Securisation
                    printf("Dynamic allocation error !");
                    return EXIT_FAILURE;
                }
                allyUnitsTab[allyUnitsNumber-1].x = i;
                allyUnitsTab[allyUnitsNumber-1].y = j;
                allyUnitsTab[allyUnitsNumber-1].class = unitsArray[(j*X)+i];
                allyUnitsTab[allyUnitsNumber-1].priority = 99;
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
                enemyUnitsTab[enemiesNumber-1].class = unitsArray[(j*X)+i];
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

    while (rank <= moveNber){
        while (tree[previousPosition].rank == rank){
            if (tree[previousPosition].y - 1 >= 0){
                // Si il n'y a pas d'obstacles
                if (unitsArray[(tree[previousPosition].y - 1)*X + tree[previousPosition].x] != 7 && tree[previousPosition].priority < moveNber){
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
                    tree[currentPosition].unit = tree[previousPosition].unit;
                    if (unitsArray[(tree[previousPosition].y)*X + tree[previousPosition].x] == 8){
                        tree[currentPosition].priority = tree[previousPosition].priority + 2;
                    } else {
                        tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    }
                    

                    for (int i = 0; i < enemiesNumber; i++){
                        // Si il y a un ennemi et qu'une unité ne l'a pas déjà tué
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                            // triangle des forces
                            if ((allyUnitsTab[(int) tree[currentPosition].unit].class == 1 && enemyUnitsTab[i].class == 5) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 2 && enemyUnitsTab[i].class == 6) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 3 && enemyUnitsTab[i].class == 4)){
                                if (tree[currentPosition].priority < enemyUnitsTab[i].priority && tree[currentPosition].priority < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = 0;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority;

                                    printf("\n(%d;%d)", tree[currentPosition].x, tree[currentPosition].y);        
                                }
                            } else {
                                if ((tree[currentPosition].priority + STAMINAATTACK/2) < enemyUnitsTab[i].priority && (tree[currentPosition].priority + STAMINAATTACK/2) < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = STAMINAATTACK/2;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority + STAMINAATTACK/2;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority + STAMINAATTACK/2;

                                    printf("\n// (%d;%d)", tree[currentPosition].x, tree[currentPosition].y);                   
                                }
                            }
                        }
                    }
                }
            }
            if (tree[previousPosition].y + 1 < Y){
                // Si il n'y a pas d'obstacles
                if (unitsArray[(tree[previousPosition].y + 1)*X + tree[previousPosition].x] != 7 && tree[previousPosition].priority < moveNber){
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
                    tree[currentPosition].unit = tree[previousPosition].unit;
                    if (unitsArray[(tree[previousPosition].y)*X + tree[previousPosition].x] == 8){
                        tree[currentPosition].priority = tree[previousPosition].priority + 2;
                    } else {
                        tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    }

                    for (int i = 0; i < enemiesNumber; i++){
                        // Si il y a un ennemi et qu'une unité ne l'a pas déjà tué
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                            // triangle des forces
                            if ((allyUnitsTab[(int) tree[currentPosition].unit].class == 1 && enemyUnitsTab[i].class == 5) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 2 && enemyUnitsTab[i].class == 6) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 3 && enemyUnitsTab[i].class == 4)){
                                if ((tree[currentPosition].priority) < enemyUnitsTab[i].priority && tree[currentPosition].priority < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = 0;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority;

                                    printf("\n(%d;%d)", tree[currentPosition].x, tree[currentPosition].y);   
                                }
                            } else {
                                if ((tree[currentPosition].priority + STAMINAATTACK/2) < enemyUnitsTab[i].priority && (tree[currentPosition].priority + STAMINAATTACK/2) < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = STAMINAATTACK/2;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority + STAMINAATTACK/2;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority + STAMINAATTACK/2;

                                    printf("\n// (%d;%d)", tree[currentPosition].x, tree[currentPosition].y);
                                }
                            }
                        }
                    }
                }
            }
            if (tree[previousPosition].x + 1 < X){
                // Si il n'y a pas d'obstacles
                if (unitsArray[tree[previousPosition].y*X + tree[previousPosition].x + 1] != 7 && tree[previousPosition].priority < moveNber){
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
                    tree[currentPosition].unit = tree[previousPosition].unit;
                    if (unitsArray[(tree[previousPosition].y)*X + tree[previousPosition].x] == 8){
                        tree[currentPosition].priority = tree[previousPosition].priority + 2;
                    } else {
                        tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    }

                    for (int i = 0; i < enemiesNumber; i++){
                        // Si il y a un ennemi et qu'une unité ne l'a pas déjà tué
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                            // triangle des forces
                            if ((allyUnitsTab[(int) tree[currentPosition].unit].class == 1 && enemyUnitsTab[i].class == 5) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 2 && enemyUnitsTab[i].class == 6) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 3 && enemyUnitsTab[i].class == 4)){
                                if (tree[currentPosition].priority < enemyUnitsTab[i].priority && tree[currentPosition].priority < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = 0;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority;

                                    printf("\n(%d;%d)", tree[currentPosition].x, tree[currentPosition].y);
                                }
                            } else {
                                if ((tree[currentPosition].priority + STAMINAATTACK/2) < enemyUnitsTab[i].priority && (tree[currentPosition].priority + STAMINAATTACK/2) < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = STAMINAATTACK/2;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority + STAMINAATTACK/2;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority + STAMINAATTACK/2;

                                    printf("\n// (%d;%d)", tree[currentPosition].x, tree[currentPosition].y);
                                }
                            }
                        }
                    }
                }
            }
            if (tree[previousPosition].x - 1 >= 0){
                // Si il n'y a pas d'obstacles
                if (unitsArray[tree[previousPosition].y*X + tree[previousPosition].x - 1] != 7 && tree[previousPosition].priority < moveNber){
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
                    tree[currentPosition].unit = tree[previousPosition].unit;
                    if (unitsArray[(tree[previousPosition].y)*X + tree[previousPosition].x] == 8){
                        tree[currentPosition].priority = tree[previousPosition].priority + 2;
                    } else {
                        tree[currentPosition].priority = tree[previousPosition].priority + 1;
                    }

                    for (int i = 0; i < enemiesNumber; i++){
                        // Si il y a un ennemi et qu'une unité ne l'a pas déjà tué
                        if (tree[currentPosition].x == enemyUnitsTab[i].x && tree[currentPosition].y == enemyUnitsTab[i].y && tree[currentPosition].unit != enemyUnitsTab[i].unit){
                            // triangle des forces
                            if ((allyUnitsTab[(int) tree[currentPosition].unit].class == 1 && enemyUnitsTab[i].class == 5) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 2 && enemyUnitsTab[i].class == 6) || (allyUnitsTab[(int) tree[currentPosition].unit].class == 3 && enemyUnitsTab[i].class == 4)){
                                if (tree[currentPosition].priority < enemyUnitsTab[i].priority && tree[currentPosition].priority < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = 0;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority;

                                    printf("\n(%d;%d)", tree[currentPosition].x, tree[currentPosition].y);
                                }
                            } else {
                                if ((tree[currentPosition].priority + STAMINAATTACK/2) < enemyUnitsTab[i].priority && (tree[currentPosition].priority + STAMINAATTACK/2) < allyUnitsTab[(int) tree[currentPosition].unit].priority){
                                    enemyUnitsTab[i].found = currentPosition;
                                    enemyUnitsTab[i].unit = tree[currentPosition].unit;
                                    tree[previousPosition].priority = STAMINAATTACK/2;
                                    enemyUnitsTab[i].priority = tree[currentPosition].priority + STAMINAATTACK/2;
                                    allyUnitsTab[(int) tree[currentPosition].unit].priority = tree[currentPosition].priority + STAMINAATTACK/2;

                                    printf("\n// (%d;%d)", tree[currentPosition].x, tree[currentPosition].y);
                                }
                            }
                        }
                    }
                }
            }
            previousPosition++;
        }
        rank++;
    }

    // Affichage
    struct Leaf tmp;
    char totalMoveTab[X * Y];
    int totalMove = 0;

    for (int i = 0; i < enemiesNumber; i++) {
        if (enemyUnitsTab[i].found != -1){
            tmp = tree[enemyUnitsTab[i].found];

            while (tree[tmp.previous].previous != -1) {
                tmp = tree[tmp.previous];
                totalMoveTab[(tmp.y * X) + tmp.x] = 1;
            }
        }
    }

    for (int i = 0; i < X * Y; i++) {
        if (totalMoveTab[i] == 1) {
            totalMove++;
        }
    }

    // Si il y a une solution
    if (totalMove <= moveNber) {
        for (int i = 0; i < enemiesNumber; i++) {
            if (enemyUnitsTab[i].found != -1){
                tmp = tree[enemyUnitsTab[i].found];
                printf("\n\nUnit : %d", i + 1);

                while (tree[tmp.previous].previous != -1) {
                    tmp = tree[tmp.previous];
                    printf("\nPosition : (%d,%d)", tmp.x, tmp.y);
                }
            }
        }
    }
    else { // Si il n'y a pas de solution
        printf("\nThere is no solution !");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}