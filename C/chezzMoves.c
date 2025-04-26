/*This file has all the nessary functions to generate next possible moves from a given postion*/

#include "chezz.h"

char isColor (unsigned char peice, char color){
    //helper function to determine weather the peice is a given color (ie white or black)
    
    //if it is whites turn
    if (color == 'w'){
        //return true if it is a white peice and false otherwise
        if ((peice >= whitePawn) && (peice <= whiteKing)) return 1;
        else return 0;
    }else if (color == 'b'){
        if ((peice >= blackPawn) && (peice <= blackKing)) return 1;
        else return 0;
    }else{
        fprintf(stderr,"Invalid peice color\n");
        return -1;
    }
}

char isSquareOccupiedByColor (unsigned char board[8][8], char x, char y, char colorToCheck){
    //helper function to depermine if there is a peice of a given color on a square
    //if colorToCheck is set to 0 then this function will run a check for both colors
    //fprintf(stderr,"is occ %d %d by %c\n",x,y,colorToCheck);
    if (x < 0 || y < 0 || x > ROWS || y > COLS) return 1;

    unsigned char peice = board[x][y];
    //fprintf(stderr,"occ by: %u\n",peice);

    //if theres nothing on the square then we are done
    if (peice == blankSquare) return 0;
    else{
        //we know that there is some peice on the given square

        //if we don't care about the color of the peice on this square then we are done
        if (colorToCheck == 0) return 1;

        //if we do care about the color of the particular piece then we can use the helper function to test it
        return isColor(peice, colorToCheck);
    } 
}

char isOnBoard (char x, char y){
    //helper function that will determine if the given square is on the board ie (0,0) to (7,7)
    //fprintf(stderr, "xy: %d %d\n",x,y);
    if (x < 0 || y < 0 || x >= ROWS || y >= COLS) return 0;
    else return 1;
}

void UpDownLeftRight(Node * instance, char colorToMove, char x, char y, char onlyOneSquare){

    char realColorToMove;

    //if the peice that is moving isn't allowed to take other peices and thus colorToMove was given as a upper case letter
    if (colorToMove < 'a'){
        realColorToMove = colorToMove + 32;
        colorToMove  = 0;
    }else{
        realColorToMove = colorToMove; 
    }

    //will caculate all the diffrent square a rook can end up in within a file (also useful for the queen)
    char opponentColor = (realColorToMove == 'w') == 0 ?  'w':  'b';
    //consider all the possible directions we can move from a given square
    char directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    
    //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
    bool validDirestions[4] = {true, true, true, true};


    for (char i = 0; i < 4; i++) {
        while (validDirestions[i]){
            char newX = x + directions[i][0];
            char newY = y + directions[i][1];
            
            if(!isSquareOccupiedByColor(instance->board, newX, newY, colorToMove) && isOnBoard(newX, newY)){
                //fprintf(stderr,"\tvalid move from %d %d to %d %d\n", x, y, newX, newY);
                //now we can safely make the move on a copy of the board

                unsigned char newBoard[8][8];
                memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new
                newBoard[newX][newY] = newBoard[x][y];//move the peice one square
                newBoard[x][y] = blankSquare;//set where they were prevously to be blank
                
                addNodeToGraph(instance, newBoard, realColorToMove);

                //if we ended up capturing a peice then this we can stop exploring this line 
                if (instance->board[newX][newY] != blankSquare) validDirestions[i] = false;
                else{
                    if (i == 0) directions[i][1] = directions[i][1] + 1;
                    else if (i == 1) directions[i][1] = directions[i][1] - 1;
                    else if (i == 2) directions[i][0] = directions[i][0] + 1;
                    else directions[i][0] = directions[i][0] - 1;

                    if (onlyOneSquare) validDirestions[i] = false;
                } 
                
                
            }else{
                //as we have encountered a square on this line where we can no longer move to we can 
                //stop exploring this line
                validDirestions[i] = false;    
            }
        }    
    }   
}

void diagonals(Node * instance, char colorToMove, char x, char y, char onlyOneSquare){

    char realColorToMove;

    //if the peice that is moving isn't allowed to take other peices and thus colorToMove was given as a upper case letter
    if (colorToMove < 'a'){
        realColorToMove = colorToMove + 32;
        colorToMove  = 0;
    }else{
        realColorToMove = colorToMove; 
    }

    //will caculate all the diffrent square a bishop can end up in within a file (also useful for the queen)
    //char opponentColor = (colorToMove == 'w') == 0 ?  'w':  'b';
    //consider all the possible directions we can move from a given square
    char directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    
    //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
    bool validDirestions[4] = {true, true, true, true};

    for (char i = 0; i < 4; i++) {
        while (validDirestions[i]){
            char newX = x + directions[i][0];
            char newY = y + directions[i][1];
            
            if(!isSquareOccupiedByColor(instance->board, newX, newY, colorToMove) && isOnBoard(newX, newY)){
                //fprintf(stderr,"\tvalid move from %d %d to %d %d\n", x, y, newX, newY);
                //now we can safely make the move on a copy of the board

                unsigned char newBoard[8][8];
                memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new
                newBoard[newX][newY] = newBoard[x][y];//move the peice one square
                newBoard[x][y] = blankSquare;//set where they were prevously to be blank
                
                addNodeToGraph(instance, newBoard, realColorToMove);

                //if we ended up capturing a peice then this we can stop exploring this line 
                if (instance->board[newX][newY] != blankSquare) validDirestions[i] = false; 
                else{
                    if (i == 0){ directions[0][0]++; directions[0][1]++; }
                    else if (i == 1){ directions[1][0]++; directions[1][1]--; }
                    else if (i == 2){ directions[2][0]--; directions[2][1]++; }
                    else { directions[3][0]--; directions[3][1]--; }

                    if (onlyOneSquare) validDirestions[i] = false;
                }
                
            }else{
                //as we have encountered a square on this line where we can no longer move to we can 
                //stop exploring this line
                validDirestions[i] = false;    
            }
        }    
    }   
}

void LShape(Node * instance, char colorToMove, char x, char y){
    //will caculate all the diffrent squares a knight can move to from it's current postion 
    //char opponentColor = (colorToMove == 'w') == 0 ?  'w':  'b';
    //consider all the possible directions we can move from a given square
    char directions[8][2] = {{1, 2}, {-1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, 1}, {-2, -1}};

    for (char i = 0; i < 8; i++) {

        char newX = x + directions[i][0];
        char newY = y + directions[i][1];
        
        if(!isSquareOccupiedByColor(instance->board, newX, newY, colorToMove) && isOnBoard(newX, newY)){
            //fprintf(stderr,"\tvalid move from %d %d to %d %d\n", x, y, newX, newY);
            //now we can safely make the move on a copy of the board

            unsigned char newBoard[8][8];
            memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new
            newBoard[newX][newY] = newBoard[x][y];//move the peice one square
            newBoard[x][y] = blankSquare;//set where they were prevously to be blank
            
            addNodeToGraph(instance, newBoard, colorToMove);
            
        }   
    } 
}

void FIRE(Node * instance, char colorToMove, char x, char y){
    //this function will handle when the cannon fires the cannon ball in each of the possible directions
    //to make this happen we can reuse the same logic from the diagonals fuction and just clear all the squares along the path
    char directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}, notNullMove = 0;

    
    //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
    bool validDirestions[4] = {true, true, true, true};

    for (char i = 0; i < 4; i++) {
        //make a copy of the board to sitmulate the cannon firing on
        unsigned char newBoard[8][8];
        memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new
        

        while (validDirestions[i]){
            //update our postion to the next square in the sequence
            char newX = x + directions[i][0];
            char newY = y + directions[i][1];
            
            if(isOnBoard(newX, newY)){//keep going along the line for as long as it is on the board
                //fprintf(stderr,"is on board %d %d\n",newX, newY);
                //set the square to be empty to stimulate the cannon ball hitting and clearing the square
                newBoard[newX][newY] = blankSquare;

                //move to the next square in the squence
                if (i == 0){ directions[0][0]++; directions[0][1]++; }
                else if (i == 1){ directions[1][0]++; directions[1][1]--; }
                else if (i == 2){ directions[2][0]--; directions[2][1]++; }
                else { directions[3][0]--; directions[3][1]--; }

                if (notNullMove || isSquareOccupiedByColor(instance->board, newX, newY, 0)){
                    //as we have hit somthing with the cannon we know that firing the cannon in this direction is a valid move and can set the flag 
                    notNullMove++;
                    //fprintf(stderr,"IN FINRE checking %d\n",notNullMove);
                }
                

                
            }else{//once we find a square that is't on the board
                validDirestions[i] = false;   //we know weve reached the end of this line and can stop

                if (notNullMove != 0){//if we hit at least 1 peice with the cannon ball then this counts as a valid move and we can log it
                    addNodeToGraph(instance, newBoard, colorToMove); 
                    notNullMove = 0;//reset the flag for the next itteration of the loop
                }
                
            }
        }    
    }

}

void FLING(Node * instance, char colorToMove, char x, char y){
     //This function will caculate all the diffrent possible ways the flinger can throw any peices that are adjacent to it

     char opponentColor = (colorToMove == 'w') == 0 ?  'w':  'b';

     //consider all the possible directions we can fling a peice
     char directions[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
     
     //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
     bool validDirestions[8] = {true, true, true, true, true, true, true, true};
 
     for (char i = 0; i < 8; i++){//look threw all the possible directions 

        //firstly we need to see if there is a peice that we can fling in the current direction:
        char potentialPeiceX = x + directions[i][0] * -1;
        char potentialPeiceY = y + directions[i][1] * -1;

        //fprintf(stderr, "\tpotental peice %d : %d %d\n",i,potentialPeiceX,potentialPeiceY);
        //fprintf(stderr, "\ttype %d result %d\n",board[potentialPeiceX][potentialPeiceY],isSquareOccupiedByColor(board, potentialPeiceX, potentialPeiceY, colorToMove));
        //if this new square we've found adjacent to the flinger HAS a frie3ndly pecie on it
        if(isSquareOccupiedByColor(instance->board, potentialPeiceX, potentialPeiceY, colorToMove) && isOnBoard(potentialPeiceX, potentialPeiceY)){

            while (validDirestions[i]){  //to consider all of the squares in line with the flinger and this peice
            
                //caclate the new postion of the flung peice
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];

                //fprintf(stderr,"\t\n from %d %d \n", newX, newY);
                if(isOnBoard(newX, newY)){//if the peice is on the board
                    //AND if the square we are going to be landing on does't have a friendly peice or a king
                    if (!isSquareOccupiedByColor(instance->board, newX, newY, colorToMove) && instance->board[newX][newY] != blackKing && instance->board[newX][newY] != whiteKing){
                        //fprintf(stderr,"\tvalid move from %d %d to %d %d\n", x, y, newX, newY);

                        //create a copy of the board
                        unsigned char newBoard[8][8];
                        memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new

                        //now we know that we can fling a peice to this square and that it doesn't contain a friendly piece or a king
                        //however we still need to consider if the square has a enemy peice on it or not and if it does then we need to distroy both peices

                        //if the square we are going to be landing on has a enemy peice
                        if (isSquareOccupiedByColor(instance->board, newX, newY, opponentColor)){
                            //distroy both of the peices as the friendly one will have landed on the enemy one killing them both
                            newBoard[newX][newY] = blankSquare;
                            newBoard[potentialPeiceX][potentialPeiceY] = blankSquare;
                        }else{
                            //pick up the peice from its old location and move it to the new square, clearing where it was prevously 
                            newBoard[newX][newY] = newBoard[potentialPeiceX][potentialPeiceY];
                            newBoard[potentialPeiceX][potentialPeiceY] = blankSquare;
                        }
                        
                        //save a updated version of the board 
                        addNodeToGraph(instance, newBoard, colorToMove); 

                    }

                    //adjust to the next postion in the line as long as the last pos was on the board
                    if (i == 0){ directions[0][1]++; }
                    else if (i == 1){ directions[1][1]--; }
                    else if (i == 2){ directions[2][0]++; }
                    else if (i==3) { directions[3][0]--; }
                    else if (i==4) { directions[4][0]++; directions[4][1]++; }
                    else if (i==5) { directions[5][0]--; directions[5][1]--; }
                    else if (i==6) { directions[6][0]++; directions[6][1]--; }
                    else { directions[7][0]--; directions[7][1]++; }   
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestions[i] = false;    
                
                }   

            }
        }
             
              
     } 
}

void contagion(unsigned char board[8][8], char contagionColor){
    //this function should be called after every move an it will appily the zombie rule to the board
    //set the zombies that we are going to be looking for
    //printf("Called with %c",contagionColor);
    char zombieColor = (contagionColor == 'w') == 0 ? blackZombie : whiteZombie;
    char opponentColor = (contagionColor == 'w') == 0 ?  'w':  'b';
    
    char directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    bool tempBoard[8][8] = {{false}};//to keep track of what squares are going to be infected

    //look threw the whole board
    for (char y = 0; y < ROWS; y++) {
        for (char x = 0; x < COLS; x++) {        
            if (board[x][y] == zombieColor){//if we find a zombie of a given color
                for (int i = 0; i < 4; i++) {//look at the the squares around the zombie
                    int newX = x + directions[i][0];
                    int newY = y + directions[i][1];
                    
                    if (isOnBoard(newX,newY)) {
                        // Mark for conversion if not already a zombie
                        if (board[newX][newY] != zombieColor) {
                            tempBoard[newX][newY] = true;
                        }
                    }
                }
            }
        }
    }
    
    //now we are going to look threw the board again and appliy the zombification we just calculated
    for (char y = 0; y < ROWS; y++) {
        for (char x = 0; x < COLS; x++) {  
            //if the curent square is marked for contagion AND isn't a king or another zombie then we can convert it
            if (tempBoard[x][y] && isSquareOccupiedByColor(board, x, y, opponentColor) && \
            board[x][y] != whiteKing && board[x][y] != whiteZombie && board[x][y] != blackKing && board[x][y] != blackZombie){
                //fprintf(stderr,"\t\tchanging %d %d\n",x,y );
                board[x][y] = zombieColor;
            }
        }
    }
}

void peonMotion(unsigned char board[8][8]){
    //will turn any peon's on the last rank of the board into zombies
    
    //check the last rank of the board for white pawn's and if we find one convert it into a white zombie
    for (register char x = 0; x < COLS; x++) {
        if (board[x][7] == whitePawn){
            board[x][7] = whiteZombie;
            //printf("White pawn found at %d 7\n",x);
        }   
    }

    //check the first rank of the board for black pawn's and if we find one convert it into a black zombie
    for (register char x = 0; x < COLS; x++) {
        if (board[x][0] == blackPawn){
            //printf("black pawn found at %d 7\n",x);
            board[x][0] = blackZombie; 
        }  
    }
}

void addNodeToGraph(Node * parent, unsigned char board[8][8], char colorMoved){
    //given a board this function will 1. apply the zombie contagion rules to the given board
    // 2. see if there are any peons that need to be proboted to zombies 3. add the given board as a child of the parent
    char opponentColor = (colorMoved == 'w') == 0 ?  'w':  'b';

    //apllily the zombie rules to the board
    contagion(board, colorMoved);

    //see if any peons reached there respective ends of the board and need to be permoted
    peonMotion(board);

    //printBoard(board);

    if (parent->childCount < MAX_CHILDREN){
        parent->children[parent->childCount] = malloc(sizeof(Node));//allocate space for this child node 
        memcpy(parent->children[parent->childCount]->board, board, sizeof(char) * ROWS * COLS);//copy the new board into the node
        parent->children[parent->childCount]->depth = parent->depth + 1;//set the depth of the node to be one more then the parent
        parent->children[parent->childCount]->eval = heuristic(board, opponentColor);//caculate the heurstic of the board
        parent->children[parent->childCount]->parent = parent;//save a refrence to the parent this new node came from
        parent->children[parent->childCount]->childCount = 0;//as this is a new node and doesn't have any children now

        //ensure that all the pointers to potential children of this note are set to null
        for (char i = 0; i < MAX_CHILDREN; i++) {
            parent->children[parent->childCount]->children[i] = NULL;
        }

        //printBoard(parent->children[parent->childCount]->board);
        parent->childCount++;//increase the number of children

        //numExplored++;
    }else{
        //TO DO 
        //Figure out what to do in the even where there are more then 50 moves from a given postion 
        //fprintf(stderr,"discarding possible moves due to lack of pointer space\n");
    }
    
    
    //saveToList(parent, opponentColor);
    // //find the corrsponding file name
    // char *fileName = generate_next_board_filename();
    // //output the board with the zombie rules applied to it
    // if (fileName){
    //     outPutBoarsToFile( fileName, board, opponentColor);
    //     free(fileName);
    // }

}

void saveToList(Node instance, char opponentColor){
    // unsigned long pos = allocateNode();
    // memcpy(nodes[pos].board, board, sizeof(char) * ROWS * COLS);
    // nodes[pos].depth = 0;
    // nodes[pos].eval = heuristic(board, opponentColor);
    //printBoard(board);
}

unsigned char caculatePossibleMoves(Node * instance, char colorToMove){
    unsigned long numPossibleMoves = 0;
    //this function will use the helper fucntions to caculate all next possible moves from a given postion 
    //fprintf(stderr,"starts");
    //get the color of the opponent, this will be nessary later for captruing peices
    char opponentColor = (colorToMove == 'w') == 0 ?  'w':  'b';
    //fprintf(stderr,"start loop:\n");
    //loop threw the whole board looking at each square
    for (char y = 0; y < ROWS; y++) {
        for (char x = 0; x < COLS; x++) {
            //if the current square we are looking at has a peice of the color who's move it is
            if ((instance->board[x][y] != blankSquare) && isColor(instance->board[x][y], colorToMove)) {
                //fprintf(stderr,"Found non-black square of: %u at pos %d %d\n",board[x][y],x,y);
                //we now need to consider the types of peices that could be on the square
                //as each peice will have diffrent rules for how it can move

                if (instance->board[x][y] == whitePawn || instance->board[x][y] == blackPawn){
                    //we know that the board at the current postion has a pawn now we have to consider how pawns can move:
                    //It can 1. Move foward 2. capture A piece one square diagonaly away
                    
                    //first we will Deal with moving the "Peon's" one square forward

                    //find the direction we will be moving 1 for up the board -1 for down
                    char moveDirection = (colorToMove == 'w') == 0 ? -1 : 1;
                    char newY = y + moveDirection;//find its new Y pos (the x won't change)
                    //fprintf(stderr,"offsett: %d %d %c\n",moveDirection, newY, colorToMove);
                    
                    

                    //find out if this square has anything on it adn if it does then its not a valid move 
                    if (!(isSquareOccupiedByColor(instance->board, x, newY, 0)) && isOnBoard(x, newY)){
                        //fprintf(stderr,"Valid pawn move! to %d %d\n",x,newY);
                        numPossibleMoves++;

                        //now we can safely make the move on a copy of the board
                        unsigned char newBoard[8][8];
                        memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new
                        newBoard[x][newY] = newBoard[x][y];//move the peice one square
                        newBoard[x][y] = blankSquare;//set where they were prevously to be blank
                        
                        addNodeToGraph(instance, newBoard, colorToMove);
                    }else{
                        //fprintf(stderr,"Not a valid pawn move!!to %d %d\n",x,newY);
                    }

                    //now we can check if the pawn can capture anything
                    if (isSquareOccupiedByColor(instance->board, x-1, newY, opponentColor) && isOnBoard(x-1, newY)){
                        //fprintf(stderr,"Valid pawn capture left!to %d %d\n", x-1,newY);
                        numPossibleMoves++;

                        //now we can safely make the move on a copy of the board
                        unsigned char newBoard[8][8];
                        memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new
                        newBoard[x-1][newY] = newBoard[x][y];//move the peice one square
                        newBoard[x][y] = blankSquare;//set where they were prevously to be blank
                        
                        addNodeToGraph(instance, newBoard, colorToMove);
                    }else{
                        //fprintf(stderr,"Not a valid pawn capture left!!to %d %d\n", x-1,newY);
                    }
                    
                    // char occupied = isSquareOccupiedByColor(board, x+1, newY, opponentColor);
                    // char onBoard = isOnBoard(x+1, newY);
                    // fprintf(stderr, "\tisSquareOccupiedByColor: %d, isOnBoard: %d oppColor: %c\n", occupied, onBoard, opponentColor);

                    if (isSquareOccupiedByColor(instance->board, x+1, newY, opponentColor) && isOnBoard(x+1, newY)){
                        //fprintf(stderr,"Valid pawn capture right!to %d %d\n",x+1,newY);
                        numPossibleMoves++;

                        //now we can safely make the move on a copy of the board
                        unsigned char newBoard[8][8];
                        memcpy(newBoard, instance->board, sizeof(char) * ROWS *COLS);//cpy the old board over into the new
                        newBoard[x+1][newY] = newBoard[x][y];//move the peice one square
                        newBoard[x][y] = blankSquare;//set where they were prevously to be blank
                        
                        addNodeToGraph(instance, newBoard, colorToMove);
                    }else{
                        //fprintf(stderr,"Not a valid pawn capture right!!to %d %d\n",x+1,newY);
                    }

                    
                    
                    

                }else if (instance->board[x][y] == whiteZombie || instance->board[x][y] == blackZombie){
                    //the zombies can move 1 square up down left or right and we need to consider all of these
                    //the zombies can capure any enemy peices on the squares they land on
                    UpDownLeftRight(instance, colorToMove, x, y, 1);   

                }else if (instance->board[x][y] == whiteKnight || instance->board[x][y] == blackKnight){
                    LShape(instance, colorToMove, x, y);

                }else if (instance->board[x][y] == whiteBishop || instance->board[x][y] == blackBishop){

                    diagonals(instance, colorToMove, x, y, 0);
            
                }else if (instance->board[x][y] == whiteRook  || instance->board[x][y] == blackRook){

                    UpDownLeftRight(instance, colorToMove, x, y, 0);
            
                }else if (instance->board[x][y] == whiteFlinger  || instance->board[x][y] == blackFlinger){
                    //a flinger can move like a king by never capture, we can sitimulate this as follows:

                    //convert color to move into the UPPERCASE version of it's self so the function can tell The behavior we want
                    UpDownLeftRight(instance, colorToMove - 32, x, y, 1);
                    diagonals(instance, colorToMove - 32, x, y, 1);

                    //next the flinger can fling a friendly peice
                    FLING(instance, colorToMove, x, y);
            
                }else if (instance->board[x][y] == whiteCannon  || instance->board[x][y] == blackCannon){
                    //the cannon can move 1 square UDLR like 1 step rook and we can stimulate this using the same function we
                    //used for the rook and just seting the flag to only let it move 1 square
                    //by seting colorToMove to be 0 in this function call we make it so the cannon can not move onto a occupied square

                    //convert color to move into the UPPERCASE version of it's self so the function can tell The behavior we want
                    UpDownLeftRight(instance, colorToMove - 32, x, y, 1);

                    //That takes care of the movement now we have to work on the cannonball
                    FIRE(instance, colorToMove, x, y);
            
                }else if (instance->board[x][y] == whiteQueen  || instance->board[x][y] == blackQueen){
                    //as a queen can move to the same square as a rook and bishop combined we can justy call the functions we used for both of these peices
                    UpDownLeftRight(instance, colorToMove, x, y, 0);
                    diagonals(instance, colorToMove, x, y, 0);
            
                }else if (instance->board[x][y] == whiteKing  || instance->board[x][y] == blackKing){
                    //this will will deal with all the moves just along the x or y axis
                    UpDownLeftRight(instance, colorToMove, x, y, 1);
                    diagonals(instance, colorToMove, x, y, 1);
                    
                    //now we just need to handle the dignal moves
                }
            }
        }
    }
    //fprintf(stderr,"Total number of moves found: %ld\n\n",numPossibleMoves);
    return 0;
}
