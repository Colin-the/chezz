#include "chezz.h"

//root will be the start state for the program and will hold the initial board and all the information associated with it
Node root;
//unsigned long numExplored;
int moveNumber;
// pthread_mutex_t best_mutex = PTHREAD_MUTEX_INITIALIZER;
// AlphaBetaResult global_best;
SharedPrune shared_prune;
GlobalBest global_best;

char flipScore;
// unsigned long nextNodeIndex = 0;

// unsigned long allocateNode() {
//     return nextNodeIndex++; 
// }

//function to determine search depth of the engine based on remaining time
char getSearchDepth(int remainingTime, int eval){

    //if the heurstic tells us our advantage is more then being up a queen
    if (eval > whiteQueenWeight){
        //start conserving time as we are winning by so much and can afford to play slightly
        //worse moves if it means playing a bit faster (we don't want to lose on time)
        if (remainingTime < 40000) return 3;
        else return 5;
    }
    //if the board is closer in eval use default settings for search depth
    if (remainingTime < 1000) return 1;
    else if (remainingTime <= 30000) return 3;
    else return 5; 
}

//function to calculate remaining time in milliseconds
int getRemainingTime(int totalTime, int usedTime){
    return totalTime - usedTime;
}

// int main(int argc, char **argv){
//     //generateOpeningBook(4);
//     unsigned char board[8][8] = {0};
//     char colour;
//     int i1,i2,i3;
//     readBoard(board, &colour, &i1, &i2, &i3);
//     printBoard(board);
//     contagion(board, 'w');
//     printBoard(board);

//     //importGameToDatabase("RRBCZB");

    
// 	return 0;
//  }

int main(int argc, char **argv){
    /*
    Use a file to determine if we are still reading moves out of the opening dictionaryonce we find a move that is no longer in the dictionary write to a separate file to indicate 
    that we are out-of book and that way we don't have to keep searching threw it, next if we output a board with checkmate we can edit this file so that the next time the program runs
    it will start looking threw the dictornary AND if the move number is less then 2 (ie 0 or 1 depending on weather we are playing white or black in the next game) we can go
    BACK INTO this file and set the var to be true so we will be looking at our book for moves again 
    */

    //use a wall clock timer to keep track of how long the program takes to execute
    //  struct timespec start, end;
    //  clock_gettime(CLOCK_MONOTONIC, &start);
    //vars to load everything in the input file into 
    unsigned char board[8][8] = {0};
    char colour;
    //i1: used time, i2: total time, i3: move number
    int i1, i2, i3;

    //call helper function to read a board from standard in
    readBoard(board, &colour, &i1, &i2, &i3);

    // fprintf(stderr,"Searching at a depth of %d with %d time left\n",searchDepth, remainingTime);
    // fprintf(stderr,"i1:%d i2:%d\n",i1, i2);

    //i tried to impliment a opening dictonary but it proved to be more diffcult then I thought so I gave up on it

    //use the opening book to try and find a move
    Node *bestMove = NULL;
    
    //check the opening book only for the first 8 moves of the game 
    if (i3 < 8) bestMove = findOpeningMove(board, colour);
    
    char moveInBook = bestMove == NULL ? 0 : 1;

    //char moveInBook = 0;
    //Node *bestMove = NULL;

    //if the opening book contained the current board state
    if (moveInBook){
        //fprintf(stderr,"Found move in book!\n");
        //printBoard(bestMove->board);
        //fprintf(stderr,"Old eval: %d\n",root.eval);
        char opponentColor = colour == 'w' ? 'b' : 'w';

        outPutBoard(bestMove->board, opponentColor);
        //fprintf(stderr, "eval: %d\n", bestMove->eval);  
        //fprintf(stderr, "Best next move for opp: %d\n", bestMove->children[0]->eval);
    }else{
        //if there was no opening book move found search for the best move

        //update what move number it is for the heurstic
        moveNumber = i3;

        //to set the pov of the computer according to who it is playing as
        if (colour == 'w'){
            flipScore = 1;
        }else{
            flipScore = -1;
        }

        //fprintf(stderr,"doing our own search %d\n", moveInBook);
        memcpy(root.board, board, sizeof(char) * ROWS * COLS);
        root.depth = 0;
        root.childCount = 0;
        root.eval = heuristic(board, colour);
        //numExplored = 0;

        //calculate the time remaining and determine the depth we will be searching at depth
        int remainingTime = getRemainingTime(i2, i1);
        char searchDepth = getSearchDepth(remainingTime, root.eval);


        if (searchDepth > 1) bestMove = findBestMove(&root, searchDepth, colour);
        else bestMove = STfindBestMove(&root, searchDepth, colour);

        char opponentColor = colour == 'w' ? 'b' : 'w';

        //printBoard(bestMove->board);
        outPutBoard(bestMove->board, opponentColor);

        //Temporary thing for the program to keep track of every move it has made
        //as a responce to a postion it was run with :)
        //addToOpeningBook(board, colour, bestMove->board);
    }

    

     //if (bestMove == NULL){
     //   fprintf(stderr, "NULL best move....\n");
   // }

    // clock_gettime(CLOCK_MONOTONIC, &end);
    
    // double elapsed = (end.tv_sec - start.tv_sec) * 1000.0; 
    // elapsed += (end.tv_nsec - start.tv_nsec) / 1e6; 

    // fprintf(stderr, "Time: %.2f\n", elapsed);
    // fprintf(stderr,"Number of boards explored: %ld\n",numExplored);
    
    // fprintf(stderr,"root eval: %d\n",root.eval);
    // fprintf(stderr,"best eval: %d\n",bestMove->eval);
    // fprintf(stderr,"best eval child child: %d\n",bestMove->children[0]->eval);

    return 0;
}
