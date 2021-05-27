#include <stdio.h>
#include <stdlib.h>

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
    struct Position allyUnitsTab[X*Y];

    for (int j = 0; j < Y; j++){
        for (int i = 0; i < X; i++){
            if (unitsArray[(j*X)+i] == 1 || unitsArray[(j*X)+i] == 2 || unitsArray[(j*X)+i] == 3){
                allyUnitsNumber++;
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
        printf("\nThere is no enemy");
        return EXIT_SUCCESS;
    }

    // Nombre de déplacements
    int moveNber = (STAMINA - (enemiesNumber*STAMINAATTACK))/STAMINAMOVE;

    // Création du tableau de l'arbre
    int tabSize = 4;
    for (int rank = 0; rank <= moveNber; rank++){
        tabSize *= 4;
    }

    struct Leaf * tree = (struct Leaf *) malloc((tabSize+1) * sizeof(struct Leaf));
    if (!tree){
        printf("\nError : Can't create an array");
        return EXIT_FAILURE;
    }

    // Arbre des déplacements
    int branches = 1;
    int position = 0;
    int id = 0;

    tree[position].x = allyUnitsTab[0].x;                // /!\ seulement 1 unité alliée
    tree[position].y = allyUnitsTab[0].y;
    tree[position].previous = -1; // début de l'arbre

    for (int rank = 0; rank <= moveNber; rank++){
        branches *= 4;
        
        for (int i = 0; i < branches; i+=4){
            position++;
            if (tree[position-(id+1)].x >= 0 && (tree[position-(id+1)].y-1) >= 0){ // si les coordonnées ne sortent pas du tableau
                if (unitsArray[((tree[position-(id+1)].y-1)*X) + tree[position-(id+1)].x] == 7 || unitsArray[((tree[position-(id+1)].y-1)*X) + tree[position-(id+1)].x] == 8){ // si il y a des obstacles
                    tree[position].x = -1;
                    tree[position].y = -1;
                } else {
                    tree[position].x = tree[position-(id+1)].x;
                    tree[position].y = tree[position-(id+1)].y -1;
                    tree[position].previous = position-(id+1);
                }
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            position++;
            if ((tree[position-(id+2)].x+1) >= 0 && tree[position-(id+2)].y >= 0){
                if (unitsArray[(tree[position-(id+2)].y*X) + tree[position-(id+2)].x+1] == 7 || unitsArray[(tree[position-(id+2)].y*X) + tree[position-(id+2)].x+1] == 8){
                    tree[position].x = -1;
                    tree[position].y = -1;
                } else {
                    tree[position].x = tree[position-(id+2)].x +1;
                    tree[position].y = tree[position-(id+2)].y;
                    tree[position].previous = position-(id+2);
                }
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            position++;
            if (tree[position-(id+3)].x >= 0 && (tree[position-(id+3)].y+1) >= 0){
                if (unitsArray[((tree[position-(id+3)].y+1)*X) + tree[position-(id+3)].x] == 7 || unitsArray[((tree[position-(id+3)].y+1)*X) + tree[position-(id+3)].x] == 8){
                    tree[position].x = -1;
                    tree[position].y = -1;
                } else {
                    tree[position].x = tree[position-(id+3)].x;
                    tree[position].y = tree[position-(id+3)].y +1;
                    tree[position].previous = position-(id+3);
                }
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            position++;
            if ((tree[position-(id+4)].x-1) >= 0 && tree[position-(id+4)].y >= 0){
                if (unitsArray[(tree[position-(id+4)].y*X) + tree[position-(id+4)].x-1] == 7 || unitsArray[(tree[position-(id+4)].y*X) + tree[position-(id+4)].x-1] == 8){
                    tree[position].x = -1;
                    tree[position].y = -1;
                } else {
                    tree[position].x = tree[position-(id+4)].x -1;
                    tree[position].y = tree[position-(id+4)].y;
                    tree[position].previous = position-(id+4);
                }
            } else {
                tree[position].x = -1;
                tree[position].y = -1;
            }
            id += 3;
        }
    }

    // Recherche d'une réponse
    int i = 0;
    while (i < tabSize && (tree[i].x != enemyUnitsTab[0].x || tree[i].y != enemyUnitsTab[0].y)){ // /!\ seulement 1 unité ennemie
        i++;
    }

    // Affichage
    // Si il y a une solution
    if (tree[i].x == enemyUnitsTab[0].x && tree[i].y == enemyUnitsTab[0].y){
        while (tree[i].previous != -1){
            i = tree[i].previous;
            printf("\nposition : (%d,%d)", tree[i].x, tree[i].y);
        }
    }
    // Si il n'y a pas de solution
    else{
        printf("\nIl n'y a pas de solution !");
    }

    return EXIT_SUCCESS;
}