#include <stdio.h>
#include <stdlib.h>

// Solveur
// lit un fichier map.txt
// renvoie un fichier ...

#define X 15
#define Y 10
#define FILENAME "map.txt"

int main(void){
    // Array
    int unitArray[X*Y];

    // Read the file
    FILE * file = fopen(FILENAME, "r");
    char ch;

    if (file){
        int i = 0;
        while((ch = fgetc(file)) != EOF)
        {
            if (ch >= 48 && ch <= 57){
                unitArray[i++] = ch-48;
            }
        }
    } else {
        printf("Error : Can't open the file");
        return EXIT_FAILURE;
    }

    // Display
    for (int i = 0; i < (X*Y); i++){
        printf("\n%d", unitArray[i]);
    }
    

    return EXIT_SUCCESS;
}