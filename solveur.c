#include <stdio.h>
#include <stdlib.h>

// Solveur
// lit un fichier map.txt
// renvoie un fichier ...

#define X 15
#define Y 10
#define STAMINA 30
#define STAMINAMOVE 2
#define STAMINAATTACK 4
#define FILENAME "map.txt"

struct Position{
    int x;
    int y;
};

int main(void){
    // Lecture du fichier
    int unitsArray[X*Y];
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
        printf("Error : Can't open the file");
        return EXIT_FAILURE;
    }
    
    // Recherche des unités alliées (seulement le chevalier)
    int allyUnitsNumber = 0;
    struct Position allyUnitsTab[X*Y];

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 1){
                allyUnitsNumber++;
                allyUnitsTab[allyUnitsNumber-1].x = i;
                allyUnitsTab[allyUnitsNumber-1].y = j;
            }
        }
    }
    if (allyUnitsNumber == 0){
        printf("There is no ally");
        return EXIT_SUCCESS;
    }

    // Recherche des unités ennemis
    int enemiesNumber = 0;
    struct Position enemyUnitsTab[X*Y];

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 4 || unitsArray[(j*X)+i] == 5 || unitsArray[(j*X)+i] == 6){
                enemyUnitsTab[enemiesNumber].x = i;
                enemyUnitsTab[enemiesNumber].y = j;
                enemiesNumber++; 
            }
        }
    }
    if (enemiesNumber == 0){
        printf("There is no enemy");
        return EXIT_SUCCESS;
    }

    // Nombre de déplacements
    int stamina = STAMINA - (enemiesNumber*STAMINAATTACK);

    // Création du tableau de l'arbre
    int tabSize = 1;
    for (int rank = 0; rank < (stamina/STAMINAMOVE); rank++){
        tabSize *= 4;
    }
    struct Position * tree = (struct Position *) malloc(++tabSize * sizeof(struct Position));

    // Arbre
    int branches = 1;
    int position = 0;
    int id = 0;

    tree[position].x = allyUnitsTab[0].x;
    tree[position].y = allyUnitsTab[0].y;
    for (int rank = 1; rank < (stamina/STAMINAMOVE); rank++){
        branches *= 4;
        
        for (int i = 0; i < branches; i+=4){
            position++;
            if (tree[position-(id+1)].x >= 0 && (tree[position-(id+1)].y-1) >= 0){
                tree[position].x = tree[position-(id+1)].x;
                tree[position].y = tree[position-(id+1)].y -1;
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            position++;
            if ((tree[position-(id+2)].x+1) >= 0 && tree[position-(id+2)].y >= 0){
                tree[position].x = tree[position-(id+2)].x +1;
                tree[position].y = tree[position-(id+2)].y;
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            position++;
            if (tree[position-(id+3)].x >= 0 && (tree[position-(id+3)].y+1) >= 0){
                tree[position].x = tree[position-(id+3)].x;
                tree[position].y = tree[position-(id+3)].y +1;
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            position++;
            if ((tree[position-(id+4)].x-1) >= 0 && tree[position-(id+4)].y >= 0){
                tree[position].x = tree[position-(id+4)].x -1;
                tree[position].y = tree[position-(id+4)].y;
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            id += 3;
        }
    }

    // Recherche d'une réponse
    int i = 0;
    while (i < tabSize && (tree[i].x != enemyUnitsTab[0].x || tree[i].y != enemyUnitsTab[0].y)){
        i++;
    }

    // Affichage
    printf("(%d,%d)", tree[i].x, tree[i].y);

    return EXIT_SUCCESS;
}