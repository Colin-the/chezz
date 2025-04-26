#include "chezz.h"

//function to compare two boards to tell if they are the same
char compareBoards(unsigned char board1[8][8], unsigned char board2[8][8]){
    for (int i = 0; i < ROWS; i++){
        for (int j = 0; j < COLS; j++){
            if (board1[i][j] != board2[i][j]){
                return 0;
            }
        }
    }
    return 1;
}

//function to find a move in the opening book
Node* findOpeningMove(unsigned char board[8][8], char color){
    FILE* book = fopen(OPENING_BOOK_FILE, "rb");
    if (!book) {
        //fprintf(stderr,"No opening book!\n");
        return NULL;
    }

    OpeningBookEntry entry;
    //keep reading the opening book untill we find a match or run out of entires
    while (fread(&entry, sizeof(OpeningBookEntry), 1, book) == 1){
        if (entry.color == color) {
            if (compareBoards(board, entry.board)) {
                //found a matching position
                //fprintf(stderr,"Move found in book\n");

                Node* move = malloc(sizeof(Node));
                if (!move) {
                    //fprintf(stderr,"Malloc error\n");
                    fclose(book);
                    return NULL;
                }

                memcpy(move->board, entry.next_move, sizeof(char) * ROWS * COLS);
                //This is not nesssary in this application as we just want to output this board

                // move->depth = root->depth + 1;
                // move->childCount = 0;
                // move->eval = heuristic(move->board, color);

                fclose(book);
                return move;
            }
        }
    }
    //fprintf(stderr,"Move not found in book\n");
    fclose(book);
    return NULL;
}

//function to add a move to the opening book
void addToOpeningBook(unsigned char currentBoard[8][8], char color, unsigned char nextBoard[8][8]){
    FILE* book = fopen(OPENING_BOOK_FILE, "ab");
    if (!book) {
        return;
    }

    OpeningBookEntry entry;
    memcpy(entry.board, currentBoard, sizeof(char) * ROWS * COLS);
    entry.color = color;
    memcpy(entry.next_move, nextBoard, sizeof(char) * ROWS * COLS);

    fwrite(&entry, sizeof(OpeningBookEntry), 1, book);
    fclose(book);
    //fprintf(stderr,"Move added to the book!\n");
} 

//function that will read in a bunch of game files and insert them into our database of moves
void importGameToDatabase(char* folderPath){
    //Look at every other board file (as my program is making every other move in this sequence
    //and I don't want to play the same moves my opponent did if I were in their position)
    for (int i = 0; i <= 44; i += 2) {
        char currentPath[256], nextPath[256];
        
        //make the filenames
        if (i == 0){
            snprintf(currentPath, sizeof(currentPath), "%s/b", folderPath);
        } else{
            snprintf(currentPath, sizeof(currentPath), "%s/b%d", folderPath, i);
        }
        snprintf(nextPath, sizeof(nextPath), "%s/b%d", folderPath, i + 1);

        //read boards from files
        unsigned char currentBoard[8][8], nextBoard[8][8];
        FILE* currentFile = fopen(currentPath, "r");
        FILE* nextFile = fopen(nextPath, "r");
        
        if (!currentFile || !nextFile) {
            //fprintf(stderr, "Failed to open files for index %d\n", i);
            continue;
        }

        unsigned char initialBoard[8][8] = {0}, finalBoard[8][8] = {0};

        //we don't care about these values in this application only the boards
        char colour;
        int i1, i2, i3;

        //call helper function to read a board from standard in
        readBoardFromFile(initialBoard, &colour, &i1, &i2, &i3, currentFile);
        readBoardFromFile(finalBoard, &colour, &i1, &i2, &i3, nextFile);

        fclose(currentFile);
        fclose(nextFile);

        //add to the book
        addToOpeningBook(initialBoard, 'w', finalBoard);
    }
}