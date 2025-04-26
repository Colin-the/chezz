#include "chezz.h"

/*
This file will contain all of the implimentation for the diffrent 
heurstics that will be used in my program
*/


int heuristic(unsigned char board[8][8], char colorToMove){
    //Given a board and whose move it currently is this function will 
    //return an estimate of the quality of the postion.
    //A high postive score is good for white where as a low negative score is good for black

    //to keep track of the evulation of the board
    int eval = 0;

    //to store what peices "control" what squares 
    char squaresControled[COLS][ROWS][2] = {0};
    
    //to get the pos of the kings (we will need these later for some more math)
    char whiteKingX = '\0', whiteKingY, blackKingX = '\0', blackKingY;
    char whiteQueenX = '\0', whiteQueenY, blackQueenX = '\0', blackQueenY;

    //to track pawn columns for structure analysis
    char whitePawnColumns[8] = {0};
    char blackPawnColumns[8] = {0};

    //to rack piece development (num pieces moved from starting position)
    int whiteDevelopment = 0;
    int blackDevelopment = 0;
    int whitePiecesSafe = 0;
    int blackPiecesSafe = 0;

    //Itterate over the whole board and count the difference in material value
    for (char y = 0; y < ROWS; y++) {
        for (char x = 0; x < COLS; x++) {
            if (board[x][y] != blankSquare){
                //count the material value
                eval = eval + pieceWeights[board[x][y]];

                //if the current peice is a peon we will consider It for farther analysis based 
                //on other peons (doubled = bad) and how advanced it is at the board (close to permotion = good)
                if (board[x][y] == whitePawn){
                    whitePawnColumns[x]++;

                    // add a bonus for advanced pawns
                    eval += (y - 1) * PAWN_ADVANCE_BONUS * (y / 2);
                } else if (board[x][y] == blackPawn){
                    blackPawnColumns[x]++;

                    //add a bonus for advanced pawns (from black's pov)
                    eval -= (6 - y) * PAWN_ADVANCE_BONUS * ((7-y) / 2);
                }

                //add in a slight development bonus for pieces moved from starting position
                //as all of the peices start in the back two ranks we can give points to any peice
                //that is NOT in this zone to Encourage the algorithm to move pieces out onto the board
                if ((pieceWeights[board[x][y]] > whitePawnWeight) && y != 0 && (board[x][y] != whiteKing)) whiteDevelopment++;
                if ((pieceWeights[board[x][y]] == whitePawnWeight) && y != 1) whiteDevelopment++;
                if ((pieceWeights[board[x][y]] < blackPawnWeight) && y != 7 && (board[x][y] != blackKing)) blackDevelopment++;
                if ((pieceWeights[board[x][y]] == blackPawnWeight) && y != 6) blackDevelopment++;

                // if (y != 0 && y != 1 && isColor(board[x][y], 'w')){
                //     whiteDevelopment++;
                // }
                // if (y != 6 && y != 7 && isColor(board[x][y], 'b')){
                //     blackDevelopment++;
                // }

                //collect a bunch of data on what squares this peice controls as this will be useful later
                caculateSquaresControlled(squaresControled, board, board[x][y], x, y);
                
                //if we come accross one of the kings on the board save its postion so we can refrence it later
                if (board[x][y] == whiteKing){
                    whiteKingX = x;
                    whiteKingY = y;
                }else if (board[x][y] == blackKing){
                    blackKingX = x;
                    blackKingY = y;
                }

                //track the pos of both queens on the board
                if (board[x][y] == whiteQueen){
                    whiteQueenX = x;
                    whiteQueenY = y;
                }else if (board[x][y] == blackQueen){
                    blackQueenX = x;
                    blackQueenY = y;
                }


                //development and piece safety evaluation
                if (isColor(board[x][y], 'w')){
                    //add a development bonus for pieces moved from starting position
                    //if (y != 0 && y != 1) whiteDevelopment++;
                    
                    //factor in a safety bonus if piece is defended
                    if (squaresControled[x][y][white] > 0){
                        whitePiecesSafe++;
                        eval += PIECE_DEFENSE_BONUS;
                        
                        //add a extra bonus for defended queen (so the program is less likely to hang it)
                        if (board[x][y] == whiteQueen){
                            eval += QUEEN_SAFETY_BONUS;
                        }
                    }
                } else if (isColor(board[x][y], 'b')){
                    //add a development bonus for pieces moved from starting position
                    //if (y != 6 && y != 7) blackDevelopment++;
                    
                    //factor in a safety bonus if piece is defended
                    if (squaresControled[x][y][black] > 0){
                        blackPiecesSafe++;
                        eval -= PIECE_DEFENSE_BONUS;
                        
                        //add a extra bonus for defended queen (so the program is less likely to hang it)
                        if (board[x][y] == blackQueen){
                            eval -= QUEEN_SAFETY_BONUS;
                        }
                    }
                }

            }
        }
    }
    //if the given board didn't have a king on it return the score associated with checkmate

    //make sure that both sides have a king on the board and if either doesn't then we know that 
    //this move resulted in the capture of a king and thus we should assign the highest possible
    //score to this move so it will always get played if it has the oppernuity to
    if (whiteKingX == '\0'){
    //    fprintf(stderr,"\tNo WK on board\n");
        return -kingTaken * flipScore;
    }else if (blackKingX == '\0'){
    //    fprintf(stderr,"\tNo BK on board\n");
        return kingTaken * flipScore;
    }

    //Now we are going to do a special evaluation for the saftey of the queen
    if (whiteQueenX != '\0'){//if there is a white queen on the board
        if (squaresControled[whiteQueenX][whiteQueenY][black] > 0){//if the queen is under attack
            if (squaresControled[whiteQueenX][whiteQueenY][white] == 0){//if the queen is hanging!
                eval -= HANGING_PIECE_PENALTY * 3;  //double penalty for hanging queen as this is normally quite bad
            }
            eval -= ATTACK_ON_QUEEN_PENALTY;
        }
    }
    if (blackQueenX != '\0') {//if there is a black queen on the board
        if (squaresControled[blackQueenX][blackQueenY][white] > 0){//if the queen is under attack
            if (squaresControled[blackQueenX][blackQueenY][black] == 0){//if the queen is hanging!
                eval += HANGING_PIECE_PENALTY * 3;//double penalty for hanging queen as this is normally quite bad
            }
            eval += ATTACK_ON_QUEEN_PENALTY;
        }
    }

    //Now we are going to look at the pawn structure of both sides
    //we are going to assign a penlity for badly Structured pawns to slightly 
    //discourage the program from making moves that will damage its pawn structure
    for (char x = 0; x < 8; x++){

        //factor in a penality for doubled pawns
        if (whitePawnColumns[x] > 1){
            eval -= (whitePawnColumns[x] - 1) * DOUBLED_PAWN_PENALTY;
        }

        if (blackPawnColumns[x] > 1){
            eval += (blackPawnColumns[x] - 1) * DOUBLED_PAWN_PENALTY;
        }
        
        //also add in a penality for inolated pawns
        if (whitePawnColumns[x] > 0){
            char isolated = 1;
            if (x > 0 && whitePawnColumns[x-1] > 0) isolated = 0;
            if (x < 7 && whitePawnColumns[x+1] > 0) isolated = 0;
            if (isolated) eval -= ISOLATED_PAWN_PENALTY;
        }

        if (blackPawnColumns[x] > 0){
            char isolated = 1;
            if (x > 0 && blackPawnColumns[x-1] > 0) isolated = 0;
            if (x < 7 && blackPawnColumns[x+1] > 0) isolated = 0;
            if (isolated) eval += ISOLATED_PAWN_PENALTY;
        }
    }

    //check the board for any peices that are undefended (hanging)
    for (char y = 0; y < ROWS; y++){
        for (char x = 0; x < COLS; x++){
            if (board[x][y] != blankSquare){

                //if we find a queen move on as we already handled this case
                if (board[x][y] == whiteQueen || board[x][y] == blackQueen) {
                    continue; 
                }

                // First check if piece is currently hanging
                if ((isColor(board[x][y], 'w') && squaresControled[x][y][black] > 0) || (isColor(board[x][y], 'b') && squaresControled[x][y][white] > 0)){
                    //and the current peice is not defended by friendly piece
                    if ((isColor(board[x][y], 'w') && squaresControled[x][y][white] == 0) || (isColor(board[x][y], 'b') && squaresControled[x][y][black] == 0)) {
                        // Calculate penalty based on piece value
                        int pieceValue = abs(pieceWeights[board[x][y]]);
                        int penalty = HANGING_PIECE_PENALTY * (pieceValue / 100) * -1; // Scale penalty by piece value
                        
                        //add the penalty to the evaluation
                        eval += isColor(board[x][y], 'w') ? penalty : -penalty;
                    }
                }
            }
        }
    }

    //Now just looking at the 4 squares in the middle of the board
    //count how many times white and black are controling these squares

    int whiteCenterControl = 0, blackCenterControl = 0;
    for (char y = 2; y <= 5; y++) {
        for (char x = 2; x <= 5; x++) {
            whiteCenterControl += squaresControled[x][y][white];
            blackCenterControl += squaresControled[x][y][black];
        }
    }
    //fprintf(stderr,"Eval before ctrl and devloupmetn: %d\n",eval);
    //add the diffrence * a bounds for controling the center of the board
    eval += (whiteCenterControl - blackCenterControl) * CENTER_CONTROL_BONUS;

    //Factor in the devloupment bonus we caculated earlier in the game depending on the stage in the game that we are at
    if (moveNumber < 25) eval += (whiteDevelopment - blackDevelopment) * OPENING_DEVELOPMENT_BONUS;
    else if (moveNumber < 75) eval += (whiteDevelopment - blackDevelopment) * MIDGAME_DEVELOPMENT_BONUS;
    else eval += (whiteDevelopment - blackDevelopment) * ENDGAME_DEVELOPMENT_BONUS;

    //printControlBoard(squaresControled);

    //as we were looping we collected a bunch of data about what squares every single piece on the 
    //board controls. We can now use that data to gain more insight into the quality of the postion

    // 1. Have a look at the number of squares both sides control
    //      If one player controlls more squares then the other then their peices are more active
    //      then their opponents which translates into an advantageas their pieces should be 
    //      better than the counterparts of the opposite colour.

    // 2. Look at the number of squares each side controlls on the other one's half of the board
    //      If one side is really pressing into the other and end is controlling a lot of squares 
    //      on their half of the board then chances are that they are the being aggressive and 
    //      are starting or are on a bit of an attack which translates into some form of an advantage

    // 3. Number of squares around the king opponets king that both players are attacking
    //      If One side has a lot of pieces that are pointing at the king then this likely is an 
    //      advantage as we can turn it into some kind of attack on the king and possibly 
    //      capture him winning us the game

    //We know that the max possible value for the # of squares controled by
    //either side is 118 (from the math in the caculateSquaresControlled function)
    //so we can safely store these numbers as unsigned char's
    unsigned char whiteCtrl = 0, blackCtrl = 0, whiteCtrlBlackSide = 0, blackCtrlWhiteSide = 0;; 

    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            whiteCtrl = whiteCtrl + squaresControled[x][y][white];
            blackCtrl = blackCtrl + squaresControled[x][y][black];

            //this means that we are on whites side of the board
            if (y < 4){
                //keep track of the number of squares black controlls on whites side
                blackCtrlWhiteSide = blackCtrlWhiteSide + squaresControled[x][y][black];
            }else{
                //keep track of the number of squares white controlls on black's side
                whiteCtrlBlackSide = whiteCtrlBlackSide + squaresControled[x][y][white];
            }
        }
    }

    //compote the diffrence in them and adjust it by a scaling factor
    //if this value is negative that means black controls more then white
    //printf("W:%d b:%d eval: %d\n",whiteCtrl, blackCtrl, eval);
    int numSquaresDiffrence = (whiteCtrl - blackCtrl) * controllWeight;
    int sideControllDiffrence = (whiteCtrlBlackSide - blackCtrlWhiteSide) * controlOpponentWeight;

    int whiteKingSecurity = caculateKingSecurity(squaresControled, whiteKingX, whiteKingY, white, colorToMove);

    int blackKingSecurity = caculateKingSecurity(squaresControled, blackKingX, blackKingY, black, colorToMove);

     //fprintf(stderr,"whiteCtrl: %d, blackCtrl: %d color %c\n", whiteCtrl, blackCtrl, colorToMove);
     //fprintf(stderr,"whiteCtrlBlackSide: %d, blackCtrlWhiteSide: %d\n", whiteCtrlBlackSide, blackCtrlWhiteSide);
     //fprintf(stderr,"whiteKingSecurity: %d, blackKingSecurity: %d\n", whiteKingSecurity, blackKingSecurity);
     //fprintf(stderr,"Eval: %d\n",eval);
    // printControlBoard(squaresControled);


    //if either king can be taken (ie checkmate) then we disregard everything else and return a score that reflects this
    if (whiteKingSecurity == gameWeight) {
        return -gameWeight * flipScore;
        //return (colorToMove == 'w') ? -gameWeight : gameWeight;
    }
        
    
    if (blackKingSecurity == gameWeight){
        return gameWeight * flipScore;
        //return (colorToMove == 'b') ? gameWeight : -gameWeight;
    }
        
    //int perspective = (colorToMove == 'w') ? 1 : -1;

    // if (whiteKingSecurity == gameWeight) return gameWeight * -1;
    // if (blackKingSecurity == gameWeight) return gameWeight;

    //if there is no checkmate on the board then takeinto accout how safe each king is
    int kingSecurity = (blackKingSecurity - whiteKingSecurity);

    return (eval + numSquaresDiffrence + sideControllDiffrence + kingSecurity) * flipScore;
    //return eval + numSquaresDiffrence + sideControllDiffrence + kingSecurity;
}

int caculateKingSecurity(char squaresControled[COLS][ROWS][2], char kingX, char kingY, char color, char colorToMove){

    //This function will return a int that resprsents how "secure" the king is
    //a score of 0 is ldeal as it would mean the opponent can't attack the king or any squares in close proximity to him
    //where as a score of +-gameWeight would mean that the king CAN be captured on the given board. This function will just
    //return a score baised on king saftey and it is the responcibility of the caller to enterprit this

    //First look to see if whoever's turn it is can take a king (ie checkmate the opponent)

    //if it is whites turn and we have the pos of the black king
    
    if (colorToMove == 'w' && color == black){
        //fprintf(stderr,"whites's move w/ BK pos\n");
        //if the king CAN be captured by some peice then return the max as this is checkmate for white
        if (squaresControled[kingX][kingY][white] >= 1){
        //    fprintf(stderr,"\tWhite can take black king\n");
            return gameWeight;//return the highest possible score if white can take blacks king
        }

        //otherwise if it is black's turn and we have the pos of the white king
    }else if (colorToMove == 'b' && color == white){
        //fprintf(stderr,"Blacks's move w/ WK pos\n");
        //if the king CAN be captured by some peice then return the min as this is checkmate for black
        if (squaresControled[kingX][kingY][black] >= 1){
        //    fprintf(stderr,"\tBlack can take white king\n");
            return gameWeight;//return the highest possible score if white can take blacks king
        }
    }

    //if the king can't be taken look at the squares in a area around him 

    //if the king can't be taken we can look at the squares in a 8-neighborhood around him and see what the peace activity
    //for the enemy looks like in this area. If there's a lot of activity it may be that the king can be easily attacked
    char squaresToCheck[25][2] = {{0,0}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1},\
                                 {2, 2}, {2, 1}, {2, 0}, {2, -1}, {2, -2},\
                                 {-2, 2}, {-2, 1}, {-2, 0}, {-2, -1}, {-2, -2},\
                                 {-1, 2}, {0, 2}, {1, 2},\
                                 {-1, -2}, {0, -2}, {1, -2}};
    
    int kingSecurity = 0;

    for (char i = 0; i < 25; i++){
        //caculate one of the squares around the king
        char xToCheck = kingX + squaresToCheck[i][0];
        char yToCheck = kingY + squaresToCheck[i][1];

        //check if this new square is on the board
        if (isOnBoard(xToCheck, yToCheck)){

            if (i == 0){
                kingSecurity = kingSecurity + squaresControled[xToCheck][yToCheck][!color] * checkWeight;

            //if we are looking at one of the squares within ONE square of the king
            }else if (i < 9){
                kingSecurity = kingSecurity + squaresControled[xToCheck][yToCheck][!color] * closeToKingWeight1;

            //we are looking at one of the spaces that is 2 squares away
            }else{
                kingSecurity = kingSecurity + squaresControled[xToCheck][yToCheck][!color] * closeToKingWeight2;
            }
        }
    }

    return kingSecurity;
}

void caculateSquaresControlled(char squaresControled[COLS][ROWS][2], unsigned char board[8][8], char peice, char x, char y){
    //This function will calculate the number of pieces that can capture onto each square of the board
    //This will tell us who has more control over certan set of squares and we can use this to infer 
    //who has more control over key areas of the board wich is useful in evaluating quality of positions

    //Max control numbers:
    // Peon:        2 * 7 * 2 (extra *2 to account for them all getting turned into zombies and having 4 possible moves)
    // Zombie       4 
    //bish/cannon   7 * 2 
    //rook          14
    //queen/flinger 21 * 2
    //knight/King   8 * 2
    //total         118
    //Therefore we are safe no matter the postion to store this resulting sum in a char

    if(peice == whitePawn){
        if(isOnBoard(x - 1, y + 1)) squaresControled[x - 1][y + 1][white]++;
        if(isOnBoard(x + 1, y + 1)) squaresControled[x + 1][y + 1][white]++;
    }else if (peice == blackPawn){
        if(isOnBoard(x - 1, y - 1)) squaresControled[x - 1][y - 1][black]++;
        if(isOnBoard(x + 1, y - 1)) squaresControled[x + 1][y - 1][black]++;
    }else if (peice == whiteZombie){
        if(isOnBoard(x, y + 1)) squaresControled[x][y + 1][white]++;
        if(isOnBoard(x, y - 1)) squaresControled[x][y - 1][white]++;
        if(isOnBoard(x + 1, y)) squaresControled[x + 1][y][white]++;
        if(isOnBoard(x - 1, y)) squaresControled[x - 1][y][white]++;
    }else if (peice == blackZombie){
        if(isOnBoard(x, y + 1)) squaresControled[x][y + 1][black]++;
        if(isOnBoard(x, y - 1)) squaresControled[x][y - 1][black]++;
        if(isOnBoard(x + 1, y)) squaresControled[x + 1][y][black]++;
        if(isOnBoard(x - 1, y)) squaresControled[x - 1][y][black]++;
    }else if (peice == whiteKing){
        if(isOnBoard(x, y + 1)) squaresControled[x][y + 1][white]++;
        if(isOnBoard(x, y - 1)) squaresControled[x][y - 1][white]++;
        if(isOnBoard(x + 1, y)) squaresControled[x + 1][y][white]++;
        if(isOnBoard(x - 1, y)) squaresControled[x - 1][y][white]++;

        if(isOnBoard(x + 1, y + 1)) squaresControled[x + 1][y + 1][white]++;
        if(isOnBoard(x - 1, y - 1)) squaresControled[x - 1][y - 1][white]++;
        if(isOnBoard(x + 1, y - 1)) squaresControled[x + 1][y - 1][white]++;
        if(isOnBoard(x - 1, y + 1)) squaresControled[x - 1][y + 1][white]++;
    }else if (peice == blackKing){
        if(isOnBoard(x, y + 1)) squaresControled[x][y + 1][black]++;
        if(isOnBoard(x, y - 1)) squaresControled[x][y - 1][black]++;
        if(isOnBoard(x + 1, y)) squaresControled[x + 1][y][black]++;
        if(isOnBoard(x - 1, y)) squaresControled[x - 1][y][black]++;

        if(isOnBoard(x + 1, y + 1)) squaresControled[x + 1][y + 1][black]++;
        if(isOnBoard(x - 1, y - 1)) squaresControled[x - 1][y - 1][black]++;
        if(isOnBoard(x + 1, y - 1)) squaresControled[x + 1][y - 1][black]++;
        if(isOnBoard(x - 1, y + 1)) squaresControled[x - 1][y + 1][black]++;
    }else if (peice == whiteKnight){
        //All the possible ways a night can move from a given square
        char directions[8][2] = {{1, 2}, {-1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, 1}, {-2, -1}};

        for (char i = 0; i < 8; i++) {
            //Use the directions as an offset from the current square to calculate where the knight can land
            char newX = x + directions[i][0];
            char newY = y + directions[i][1];

            //If this new square is on the board then we know the Knight can go there as a legal move
            if(isOnBoard(newX, newY)) squaresControled[newX][newY][white]++;
        }
    }else if (peice == blackKnight){
        //All the possible ways a night can move from a given square
        char directions[8][2] = {{1, 2}, {-1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, 1}, {-2, -1}};

        for (char i = 0; i < 8; i++) {
            //Use the directions as an offset from the current square to calculate where the knight can land
            char newX = x + directions[i][0];
            char newY = y + directions[i][1];
            
            //If this new square is on the board then we know the Knight can go there as a legal move
            if(isOnBoard(newX, newY)) squaresControled[newX][newY][black]++;
        }

    }else if (peice == whiteBishop){
        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestions[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the bishop
            while (validDirestions[i]){//while the particular line is uninterrupted
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][white]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestions[i] = false; 
                    else{
                        if (i == 0){ directions[0][0]++; directions[0][1]++; }
                        else if (i == 1){ directions[1][0]++; directions[1][1]--; }
                        else if (i == 2){ directions[2][0]--; directions[2][1]++; }
                        else { directions[3][0]--; directions[3][1]--; }
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestions[i] = false;    
                }
            }    
        }

    }else if (peice == blackBishop){
        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestions[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the bishop
            while (validDirestions[i]){//while the particular line is uninterrupted
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][black]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestions[i] = false; 
                    else{
                        if (i == 0){ directions[0][0]++; directions[0][1]++; }
                        else if (i == 1){ directions[1][0]++; directions[1][1]--; }
                        else if (i == 2){ directions[2][0]--; directions[2][1]++; }
                        else { directions[3][0]--; directions[3][1]--; }
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestions[i] = false;    
                }
            }    
        }

    }else if (peice == whiteRook){
        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestions[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the rook
            while (validDirestions[i]){//while the particular line is uninterrupted
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][white]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestions[i] = false; 
                    else{
                        if (i == 0){directions[0][0]++;}
                        else if (i == 1){directions[1][1]--;}
                        else if (i == 2){directions[2][0]--;}
                        else {directions[3][1]++;}
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestions[i] = false;    
                }
            }    
        }

    }else if (peice == blackRook){
        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestions[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the rook
            while (validDirestions[i]){//while the particular line is uninterrupted
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][black]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestions[i] = false; 
                    else{
                        if (i == 0){directions[0][0]++;}
                        else if (i == 1){directions[1][1]--;}
                        else if (i == 2){directions[2][0]--;}
                        else {directions[3][1]++;}
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestions[i] = false;    
                }
            }    
        }

    }else if (peice == whiteQueen){
        //consider all the possible directions we can move from a given square
        char directionsD[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestionsD[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the bishop
            while (validDirestionsD[i]){//while the particular line is uninterrupted
                char newX = x + directionsD[i][0];
                char newY = y + directionsD[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][white]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestionsD[i] = false; 
                    else{
                        if (i == 0){ directionsD[0][0]++; directionsD[0][1]++; }
                        else if (i == 1){ directionsD[1][0]++; directionsD[1][1]--; }
                        else if (i == 2){ directionsD[2][0]--; directionsD[2][1]++; }
                        else { directionsD[3][0]--; directionsD[3][1]--; }
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestionsD[i] = false;    
                }
            }    
        }

        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestions[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the rook
            while (validDirestions[i]){//while the particular line is uninterrupted
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][white]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestions[i] = false; 
                    else{
                        if (i == 0){directions[0][0]++;}
                        else if (i == 1){directions[1][1]--;}
                        else if (i == 2){directions[2][0]--;}
                        else {directions[3][1]++;}
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestions[i] = false;    
                }
            }    
        }

    }else if (peice == blackQueen){
        //consider all the possible directions we can move from a given square
        char directionsD[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestionsD[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the bishop
            while (validDirestionsD[i]){//while the particular line is uninterrupted
                char newX = x + directionsD[i][0];
                char newY = y + directionsD[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][black]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestionsD[i] = false; 
                    else{
                        if (i == 0){ directionsD[0][0]++; directionsD[0][1]++; }
                        else if (i == 1){ directionsD[1][0]++; directionsD[1][1]--; }
                        else if (i == 2){ directionsD[2][0]--; directionsD[2][1]++; }
                        else { directionsD[3][0]--; directionsD[3][1]--; }
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestionsD[i] = false;    
                }
            }    
        }

        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
        
        //this will be used to keep track of when we encounter an obsticle (wall, friendly peice) and can stop exploring the file
        bool validDirestions[4] = {true, true, true, true};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the rook
            while (validDirestions[i]){//while the particular line is uninterrupted
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];
                
                if(isOnBoard(newX, newY)){

                    //then the peice can go here thus it is a square controled by it
                    squaresControled[newX][newY][black]++;

                    //if there is some peice on this line then we stpo exploring it
                    if (board[newX][newY] != blankSquare) validDirestions[i] = false; 
                    else{
                        if (i == 0){directions[0][0]++;}
                        else if (i == 1){directions[1][1]--;}
                        else if (i == 2){directions[2][0]--;}
                        else {directions[3][1]++;}
                    }
                    
                }else{
                    //as we have encountered a square on this line where we can no longer move to we can stop exploring this line
                    validDirestions[i] = false;    
                }
            }    
        }

    }else if (peice == whiteCannon){
        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the bishop
                //find the first square of the new pos
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];  
                
                //while the line is still on the board
                while(isOnBoard(newX, newY)){

                    //then the cannon can fire a cannon ball here thus it *controls* the square 
                    squaresControled[newX][newY][white]++;

                    //Increment our vector to point to the next square
                    if (i == 0){ directions[0][0]++; directions[0][1]++; }
                    else if (i == 1){ directions[1][0]++; directions[1][1]--; }
                    else if (i == 2){ directions[2][0]--; directions[2][1]++; }
                    else { directions[3][0]--; directions[3][1]--; }

                    //adjust our pos using the vector
                    newX = x + directions[i][0];
                    newY = y + directions[i][1]; 
                }     
        }

    }else if (peice == blackCannon){
        //consider all the possible directions we can move from a given square
        char directions[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

        for (char i = 0; i < 4; i++) {//loop over all possible vectors coming out of the bishop
                //find the first square of the new pos
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];  
                
                //while the line is still on the board
                while(isOnBoard(newX, newY)){

                    //then the cannon can fire a cannon ball here thus it *controls* the square 
                    squaresControled[newX][newY][black]++;

                    //Increment our vector to point to the next square
                    if (i == 0){ directions[0][0]++; directions[0][1]++; }
                    else if (i == 1){ directions[1][0]++; directions[1][1]--; }
                    else if (i == 2){ directions[2][0]--; directions[2][1]++; }
                    else { directions[3][0]--; directions[3][1]--; }

                    //adjust our pos using the vector
                    newX = x + directions[i][0];
                    newY = y + directions[i][1]; 
                }     
        }

    }else if (peice == whiteFlinger){
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
            if(isSquareOccupiedByColor(board, potentialPeiceX, potentialPeiceY, 'w') && isOnBoard(potentialPeiceX, potentialPeiceY)){
                while (validDirestions[i]){  //to consider all of the squares in line with the flinger and this peice  
                    //caclate the new postion of the flung peice
                    char newX = x + directions[i][0];
                    char newY = y + directions[i][1];

                    //fprintf(stderr,"\t\n from %d %d \n", newX, newY);
                    if(isOnBoard(newX, newY)){//if the peice is on the board
                        //AND if the square we are going to be landing on isn't the enemy king
                        if (board[newX][newY] != blackKing){
                            //as the flinger can fling here we say it has *control* over the square
                            squaresControled[newX][newY][white]++;
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
    }else if (peice == blackFlinger){
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
        if(isSquareOccupiedByColor(board, potentialPeiceX, potentialPeiceY, 'b') && isOnBoard(potentialPeiceX, potentialPeiceY)){
            while (validDirestions[i]){  //to consider all of the squares in line with the flinger and this peice  
                //caclate the new postion of the flung peice
                char newX = x + directions[i][0];
                char newY = y + directions[i][1];

                //fprintf(stderr,"\t\n from %d %d \n", newX, newY);
                if(isOnBoard(newX, newY)){//if the peice is on the board
                    //AND if the square we are going to be landing on isn't the enemy king
                    if (board[newX][newY] != whiteKing){
                        //as the flinger can fling here we say it has *control* over the square
                        squaresControled[newX][newY][black]++;
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
    }else{
        //printf("Peace type : %d not recognized\n",peice);
    }
        

}
