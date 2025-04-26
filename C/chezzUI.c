/*
This file contains function nessary for chezz.c to interact with the outside world
There are files to read/write board states to files as well as helper function to
print out the board in a way that is easier to undersatnd as a human
*/
#include "chezz.h"


void readBoard(unsigned char board[8][8], char *colour, int *i1, int *i2, int *i3) {
    //function that will read a board from standard in and convert it into data the program can use
    char line[256];//space to read the current line of the file in
    
    //Get the the input from the first line of the program 
    //if it is formatted correctly then we will read & store the data on it otherwise we will exit(1)
    if (!fgets(line, sizeof(line), stdin) || sscanf(line, "%c %d %d %d", colour, i1, i2, i3) != 4){
        fprintf(stderr, "Invalid first line format\n");
        exit(EXIT_FAILURE);
    }

    /* Keep searching threw the file until we find the { marking the start of the board
    in theory this should always be on the next line but this makes reading the file a bit more
    reliable and this functon less prone to errors if the file is slightly off*/  
    while (fgets(line, sizeof(line), stdin)){
        if (strchr(line, '{')) break;
    }

    //Read threw the section of the file containing the piece locations
    while (fgets(line, sizeof(line), stdin)){
        if (strchr(line, '}')) break;//once we reach the } break out of this loop

        //strchr will return the string (including the :) and after 
        char *colon = strchr(line, ':');
        if (!colon){
            fprintf(stderr, "Error in file format\nNo : was found on a line of the piece section of the file, this may cause errors with your board not displaying correctly");
            continue;//skip this entry as it is no longer vlaid if there is a formating ishue
        }

        //extract the piece location from the line and skip the entry if the format is not as expected
        char pos[3];
        if (sscanf(line, " %2[^:]", pos) != 1) continue;
        if (strlen(pos) != 2) continue;

        //Now we need to convert the square ie a1 to the postion on the board ie (0, 0)
        int x = tolower(pos[0]) - 'a';//makesure the char is in lowercase and then subtract the ASCI value of a to convert to dec
        int y = pos[1] - '1';//convert the string that is a number into a int we can use

        //if the postion that was given is on the board 
        if (x < 0 || x >= 8 || y < 0 || y >= 8){
            fprintf(stderr, "The square %c%c is not on a standard 8x8 board\nThis piece will not be included on the board\n",pos[0], pos[1]);
            continue;
        } 

        //Now we need to extract the piece type from the line
        char *apos = strchr(line, '\'');//search for the opening '
        if (!apos) continue;//if there is no opening quote skip this line of the file
        char *bpos = strchr(apos+1, '\'');//search for the closing '
        if (!bpos) continue;//if there is no closing quote skip this line of the file

        //grab the 2 letters that makeup the piece out from in betwen the quotes
        char piece[3] = {0};
        strncpy(piece, apos+1, bpos-apos-1);
        
        //Convert from the given 2 chars into a single char so that it will take up 1 
        //less byte of storage per entrie and 64 vs 128 bytes for the whole board which
        //will save us some memory when we have to store these
        board[x][y] = pieceToDec(piece);
    }

    //Skip the last 3 lines of the file which aren't being used at this point in time
    for (int i = 0; i < 3; i++) fgets(line, sizeof(line), stdin);
}

//Same as the function above but it will read from the given file insted of stdin
void readBoardFromFile(unsigned char board[8][8], char *colour, int *i1, int *i2, int *i3, FILE *inputFile){
    char line[256];
    
    //Get the the input from the first line of the program 
    //if it is formatted correctly then we will read & store the data on it otherwise we will exit(1)
    if (!fgets(line, sizeof(line), inputFile) || sscanf(line, "%c %d %d %d", colour, i1, i2, i3) != 4){
        fprintf(stderr, "Invalid first line format\n");
        exit(EXIT_FAILURE);
    }

    /* Keep searching threw the file until we find the { marking the start of the board
    in theory this should always be on the next line but this makes reading the file a bit more
    reliable and this functon less prone to errors if the file is slightly off*/ 
    while (fgets(line, sizeof(line), inputFile)){
        if (strchr(line, '{')) break;
    }

    //Read threw the section of the file containing the piece locations
    while (fgets(line, sizeof(line), inputFile)){
        if (strchr(line, '}')) break;

        //strchr will return the string (including the :) and after 
        char *colon = strchr(line, ':');
        if (!colon){
            fprintf(stderr, "Error in file format\nNo : was found on a line of the piece section of the file, this may cause errors with your board not displaying correctly");
            continue;//skip this entry as it is no longer vlaid if there is a formating ishue
        }

        //extract the piece location from the line and skip the entry if the format is not as expected
        char pos[3];
        if (sscanf(line, " %2[^:]", pos) != 1) continue;
        if (strlen(pos) != 2) continue;

        //Now we need to convert the square ie a1 to the postion on the board ie (0, 0)
        int x = tolower(pos[0]) - 'a';//makesure the char is in lowercase and then subtract the ASCI value of a to convert to dec
        int y = pos[1] - '1';//convert the string that is a number into a int we can use

        //if the postion that was given is on the board 
        if (x < 0 || x >= 8 || y < 0 || y >= 8){
            fprintf(stderr, "The square %c%c is not on a standard 8x8 board\nThis piece will not be included on the board\n",pos[0], pos[1]);
            continue;
        } 

        //Now we need to extract the piece type from the line
        char *apos = strchr(line, '\'');//search for the opening '
        if (!apos) continue;//if there is no opening quote skip this line of the file
        char *bpos = strchr(apos+1, '\'');//search for the closing '
        if (!bpos) continue;//if there is no closing quote skip this line of the file

        //grab the 2 letters that makeup the piece out from in betwen the quotes
        char piece[3] = {0};
        strncpy(piece, apos+1, bpos-apos-1);
        
        //Convert from the given 2 chars into a single char so that it will take up 1 
        //less byte of storage per entrie and 64 vs 128 bytes for the whole board which
        //will save us some memory when we have to store these
        board[x][y] = pieceToDec(piece);
    }

    //Skip the last 3 lines of the file which aren't being used at this point in time
    for (int i = 0; i < 3; i++) fgets(line, sizeof(line), inputFile);
}

void outPutBoard(unsigned char board[8][8], char colour) {
    //function to take the c resprsentaion of a board and turn it into a output file

    int i1 = 1 , i2 = 60000, i3 = 3;

    FILE *ftpr;

    ftpr = stdout;

    //write out the first line of the file
    fprintf(ftpr, "%c %d %d %d\n", colour, i1, i2, i3);

    //start writing the js like thing that will decribe the pos of all the peices
    fprintf(ftpr, "{\n");

    //count how many peices are on the board
    char numPeices = 0;
    for (register char y = 7; y >= 0; y--) {
        for (register char x = 0; x < 8; x++) {
            if (board[x][y] != blankSquare){//if we find a peice
                numPeices++;//add it into the count
            }
        }
    }

    char currentPeice = 0; //to keep track of the peice number we are currently looking at

    //write all the peices pos in the correct file format
    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 8; x++) {
            if (board[x][y] != blankSquare){
                currentPeice++;//update the peice number we are looking at
                const char *pos = (char[]){x + 'a', y + '1', '\0'};
                fprintf(ftpr,"  "); //for the 2 space indent
                fprintf(ftpr, "%s: '%s'", pos, code_to_piece(board[x][y]));

                if (currentPeice !=numPeices) fprintf(ftpr,",\n"); //if it is NOT the last line put a ,
                else fprintf(ftpr,"\n"); //if its the last line between the {} just but a newline and no ,

            }
        }
    }

    fprintf(ftpr, "}\n");//close the }
    //fprintf(ftpr, "0\n0\n0\n");//put these in as placeholders at the bottom of the file
    //fclose(ftpr);
}

void printBoard(unsigned char board[8][8]) {
    //function to print a display board to give us some sence of what is going on in a given postion 
    printf("\n  +-----------------+\n");
    for (int y = 7; y >= 0; y--) {
        printf("%d |", y+1);
        for (int x = 0; x < 8; x++) {
            printf(" %c", get_piece_symbol(board[x][y]));
        }
        printf(" |\n");
    }
    printf("  +-----------------+\n");
    printf("    a b c d e f g h\n");
}

unsigned char pieceToDec(const char *piece) {
    //converts a piece in the <color><type> (ie wP for white pawn)
    //into its defN in the headerfile so that it can be placed on the board
    if (strlen(piece) < 2) return blankSquare;
    
    switch (piece[0]) {
        case 'w'://if it is a white piece
            switch (piece[1]) {
                case 'K': return whiteKing;
                case 'Q': return whiteQueen;
                case 'R': return whiteRook;
                case 'B': return whiteBishop;
                case 'N': return whiteKnight;
                case 'P': return whitePawn;
                case 'C': return whiteCannon;
                case 'F': return whiteFlinger;
                case 'Z': return whiteZombie;
            }
            break;
        case 'b'://if it is a black piece
            switch (piece[1]) {
                case 'K': return blackKing;
                case 'Q': return blackQueen;
                case 'R': return blackRook;
                case 'B': return blackBishop;
                case 'N': return blackKnight;
                case 'P': return blackPawn;
                case 'C': return blackCannon;
                case 'F': return blackFlinger;
                case 'Z': return blackZombie;
            }
            break;
    }
    return blankSquare;
}

const char *code_to_piece(unsigned char code) {
    //to convert my C resprsentation of each piece into the correct format for file output
    static const char *pieces[] = {
        [whiteKing] = "wK", 
        [whiteQueen] = "wQ", 
        [whiteRook] = "wR",
        [whiteBishop] = "wB", 
        [whiteKnight] = "wN", 
        [whitePawn] = "wP",
        [whiteCannon] = "wC", 
        [whiteFlinger] = "wF", 
        [whiteZombie] = "wZ",
        [blackKing] = "bK", 
        [blackQueen] = "bQ", 
        [blackRook] = "bR",
        [blackBishop] = "bB", 
        [blackKnight] = "bN", 
        [blackPawn] = "bP",
        [blackCannon] = "bC", 
        [blackFlinger] = "bF", 
        [blackZombie] = "bZ"
    };

    //compute where in the list of pieces the input maps to and return that string
    return (code < sizeof(pieces)/sizeof(pieces[0])) ? pieces[code] : "";
}

char get_piece_symbol(unsigned char code) {
    //helper function that will convert from the way the pieces are encoded in C
    //into the way we need to optput them into the file
    switch (code) {
        case whiteKing:    return 'K';
        case whiteQueen:   return 'Q';
        case whiteRook:    return 'R';
        case whiteBishop:  return 'B';
        case whiteKnight:  return 'N';
        case whitePawn:    return 'P';
        case whiteCannon:  return 'C';
        case whiteFlinger: return 'F';
        case whiteZombie:  return 'Z';
        case blackKing:    return 'k';
        case blackQueen:   return 'q';
        case blackRook:    return 'r';
        case blackBishop:  return 'b';
        case blackKnight:  return 'n';
        case blackPawn:    return 'p';
        case blackCannon:  return 'c';
        case blackFlinger: return 'f';
        case blackZombie:  return 'z';
        default:           return ' ';
    }
}

char* generate_next_board_filename() {
    //helper function to generate the next file name for the boards baised on what is currently avable

    DIR *dir;
    struct dirent *entry;
    int max_num = -1;
    char *filename = NULL;

    //this is a placeholder to keep things clean while im working
    dir = opendir(".");
    if (!dir){//if directory doesn't exist, start with 000
        if (errno == ENOENT){
            filename = malloc(MAX_PATH);
            snprintf(filename, MAX_PATH, "board.000");
            return filename;
        }
        perror("Error opening directory");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL){//go threw the existing files
        // skip . and .. entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // check filename pattern
        if (strncmp(entry->d_name, "board.", 6) != 0)
            continue;

        // Validate length and numeric suffix
        if (strlen(entry->d_name) != 9) continue;
        
        char suffix[4];
        strncpy(suffix, entry->d_name + 6, 3);
        suffix[3] = '\0';

        // Validate all digits
        int valid = 1;
        for (int i = 0; i < 3; i++) {
            if (!isdigit(suffix[i])) {
                valid = 0;
                break;
            }
        }
        if (!valid) continue;

        // Track maximum number
        int num = atoi(suffix);
        if (num > max_num) max_num = num;
    }

    closedir(dir);

    //determine the next number in the sequence
    int next_num = (max_num == -1) ? 0 : max_num + 1;

    //allocate and format filename
    filename = malloc(MAX_PATH);
    if (!filename) return NULL;
    snprintf(filename, MAX_PATH, "board.%03d", next_num);

    return filename;
}

void printControlBoard(char squaresControled[COLS][ROWS][2]) {
    printf("\nBoard Control Map (W: White, B: Black)\n");
    for (char row = 7; row >=0; row--) {
        for (char col = 0; col < COLS; col++) {
            printf("[%d,%d] ", squaresControled[col][row][white], squaresControled[col][row][black]);
        }
        printf("\n");
    }
}