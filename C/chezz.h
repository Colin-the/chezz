/*
We can Represent the state of each board using a 8x8 unaigned char array where each peice will have a diffrent nurmeic resprsentaion 
depending on its type and color. Since we are unsigned using char's we have 8 bits for each square and thus we can use values 0 - 255
to resprsent our pieces. We can assign a arbitrary value to each piece in this range to keep track of its color and type. The way 
I did this was first be seting 0 to be a blank square as this makes the most sence, then I made up values for the white pieces
and set the black pieces to be the whitePeice value + 10 to keep everying more clear and orgnised  

Using bit boards to store the board pos vs 8*8 char array:array

//Using BB's: 18 64 bit ints or 64*18 bits =  1,152 bits
//8*8 chars = 64 chars or 64 * 8 bits = 512 bits

Therefore storing the board using a bit boards would take 2.25 times as much storage And it doesn't offer much benefit for doing this
*/

//standard libaries that we will be using 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
//#include <math.h>

#define MAX_PATH 256

//the number of cores the CPU this program will be running on has
#define NUM_CORES 16

//board size
#define ROWS 8
#define COLS 8

//Set the two colors that the peices have
#define white 0
#define black 1

//Define what the elements that make up a board are:
#define blankSquare 0

//white standard chess pieces:
#define whiteKing   9
#define whiteQueen  8
#define whiteRook   5
#define whiteBishop 4
#define whiteKnight 3
#define whitePawn   1

//white special chezz pieces
#define whiteCannon  7
#define whiteFlinger 6
#define whiteZombie  2

//black standard chess pieces:
#define blackKing    19
#define blackQueen   18
#define blackRook    15
#define blackBishop  14
#define blackKnight  13
#define blackPawn    11

//black special chezz pieces
#define blackCannon  17
#define blackFlinger 16
#define blackZombie  12

//Define how much each piece is "worth" when we are calculating our heuristic
//The higher the value of the piece the more the program will value having it on the board
#define tacticalMoveThreshold 450
#define kingTaken             20000
#define gameWeight            10000 //What the total game is worth
#define controllWeight        2 //how many "points" controlling each square is worth
#define controlOpponentWeight 3 //how many "points" controlling squares on the opponent side are worth
#define checkWeight           15 //how many "points" puting the enemy king in check is worth
#define closeToKingWeight1    10 //how many "points" controlling squares that are within one square of the opponets king are worth
#define closeToKingWeight2    3 //how many "points" controlling squares that are within two squares of the opponets king are worth
#define DOUBLED_PAWN_PENALTY 15
#define ISOLATED_PAWN_PENALTY 10
#define PAWN_ADVANCE_BONUS 7
#define HANGING_PIECE_PENALTY 150
#define CENTER_CONTROL_BONUS 5
#define OPENING_DEVELOPMENT_BONUS 45 //as devloupment is importand in the early stages of the game
#define MIDGAME_DEVELOPMENT_BONUS 20 // it's a little less importand in the mid game
#define ENDGAME_DEVELOPMENT_BONUS 5 // and hardly importand in the late game
#define QUEEN_SAFETY_BONUS 30      // Extra bonus for keeping queen safe
#define PIECE_MOBILITY_BONUS 1     // Bonus for each square a piece can move to
#define PIECE_DEFENSE_BONUS 10     // Bonus for each piece defending another
#define ATTACK_ON_QUEEN_PENALTY 50 // Penalty when queen is under attack
#define PAWN_SHIELD_BONUS 10       // Bonus for pawns protecting king
#define SPECIAL_PIECE_ACTIVITY 5  // Bonus for special pieces in good positions

#define whiteKingWeight       gameWeight // as the king dying is the end of the game it is worth the most
#define whiteQueenWeight      900   // Base piece values - these should not be scaled down as much
#define whiteRookWeight       500   // as they represent the fundamental material worth
#define whiteBishopWeight     325
#define whiteKnightWeight     300
#define whitePawnWeight       100
#define whiteCannonWeight     600
#define whiteFlingerWeight    750
#define whiteZombieWeight     350

//the balack peices are the same as the white but * -1
#define blackKingWeight    -whiteKingWeight    
#define blackQueenWeight   -whiteQueenWeight   
#define blackRookWeight    -whiteRookWeight    
#define blackBishopWeight  -whiteBishopWeight  
#define blackKnightWeight  -whiteKnightWeight  
#define blackPawnWeight    -whitePawnWeight    
#define blackCannonWeight  -whiteCannonWeight  
#define blackFlingerWeight -whiteFlingerWeight 
#define blackZombieWeight  -whiteZombieWeight  


static const int pieceWeights[20] = {
    0,                       
    whitePawnWeight,
    whiteZombieWeight,       
    whiteKnightWeight,     
    whiteBishopWeight,     
    whiteRookWeight,
    whiteFlingerWeight,
    whiteCannonWeight,       
    whiteQueenWeight,      
    whiteKingWeight, 
    0,
    blackPawnWeight,
    blackZombieWeight,       
    blackKnightWeight,     
    blackBishopWeight,     
    blackRookWeight,
    blackFlingerWeight,
    blackCannonWeight,       
    blackQueenWeight,      
    blackKingWeight, 
};

//The maxium number of "new move" boards one board can have
//If a board has more than 75 next possible moves we can throw away some of the really bad moves and not explore them any further
#define MAX_CHILDREN 75

//forward declaration so that we can use 'Node *' inside the struct.
typedef struct Node Node;

//to keep track of a given pos & its evulation so we can pefrom a search
typedef struct Node {
    unsigned char board[8][8];//board
    unsigned char depth;//depth we are currrently at
    int eval; // The value given to this board by the heuristic
    unsigned char childCount; //the number of children this board has (0-MAX_CHILDREN)
    Node * parent;//a pointer to this parent
    Node * children[MAX_CHILDREN];//a list of all the children this board has
} Node;

//will be used in searching threw the graph
typedef struct {
    int value;
    Node *best_node;
} AlphaBetaResult;

//this will be used for transfering info threw and seting up threads
typedef struct {
    Node *node;
    int depthLimit;
    int alpha;
    int beta;
    char maximizingColor;
    AlphaBetaResult result;
} ThreadArgs;

typedef struct {
    Node *root_child;   // Direct child of the root node to evaluate
    int depthLimit;     // Remaining depth for this subtree
    char maximizingColor;  // Original maximizing player color
} ThreadTask;

typedef struct {
    int alpha;
    int beta;
    pthread_mutex_t mutex;
} SharedPrune;

// Thread context structure
typedef struct {
    Node *node;             // Grandchild node to evaluate
    int remaining_depth;
    char maximizing_color;  // Original maximizing player color
    SharedPrune *shared;
    int best_value;
} ThreadContext;


// Global best tracking
typedef struct {
    Node *best_node;
    int best_value;
    pthread_mutex_t mutex;
} GlobalBest;

extern SharedPrune shared_prune;
extern GlobalBest global_best;


//the max #of nodes we can have in the search space
//This is temporary so I can get the other parts of the porgram working
// #define MAX_NODES 1000000
// extern Node nodes[MAX_NODES];
// extern unsigned long nextNodeIndex;

// extern pthread_mutex_t best_mutex;
// extern AlphaBetaResult global_best;

//for counting and program monitoring reasons
//extern unsigned long numExplored;
extern int moveNumber;

extern char flipScore;

//Function prototypes
char getSearchDepth(int remainingTime, int eval);
int getRemainingTime(int totalTime, int usedTime);
void readBoard(unsigned char board[8][8], char *colour, int *i1, int *i2, int *i3);
void readBoardFromFile(unsigned char board[8][8], char *colour, int *i1, int *i2, int *i3, FILE *inputFile);
void outPutBoard(unsigned char board[8][8], char colour);
void printBoard(unsigned char board[8][8]);
unsigned char pieceToDec(const char *piece);
const char *code_to_piece(unsigned char code);
char get_piece_symbol(unsigned char code);
char* generate_next_board_filename();
void printControlBoard(char squaresControled[COLS][ROWS][2]);

//chezzMoves functions:
char isColor (unsigned char peice, char color);
char isSquareOccupiedByColor (unsigned char board[8][8], char x, char y, char colorToCheck);
char isOnBoard (char x, char y);
void UpDownLeftRight(Node * instance, char colorToMove, char x, char y, char onlyOneSquare);
void diagonals(Node * instance, char colorToMove, char x, char y, char onlyOneSquare);
void LShape(Node * instance, char colorToMove, char x, char y);
void FIRE(Node * instance, char colorToMove, char x, char y);
void FLING(Node * instance, char colorToMove, char x, char y);
void contagion(unsigned char board[8][8], char contagionColor);
void peonMotion(unsigned char board[8][8]);
unsigned char caculatePossibleMoves(Node * instance, char colorToMove);
void saveToList(Node instance, char opponentColor);
void addNodeToGraph(Node * parent, unsigned char board[8][8], char colorMoved);
char isValidNextMove(unsigned char board[8][8], int src, int dest, char colorToMove);

//unsigned long allocateNode();

//Function prototypes for the engine
int heuristic(unsigned char board[8][8], char colorToMove);
void caculateSquaresControlled(char squaresControled[COLS][ROWS][2], unsigned char board[8][8], char peice, char x, char y);
int caculateKingSecurity(char squaresControled[COLS][ROWS][2], char kingX, char kingY, char color, char colorToMove);
int compareDesc(const void *a, const void *b);
int compareAsc(const void *a, const void *b);
AlphaBetaResult alphaBeta(Node *node, int depthLimit, int alpha, int beta, char current_color, char maximizingColor);
Node *findBestMove(Node *root, int depthLimit, char maximizingColor);
Node *STfindBestMove(Node *root, int depthLimit, char maximizingColor);
//void* alphaBetaThread(void *arg);
void* thread_func(void* arg);


//defs and prototypes for the opening dictonary 

//structure to store opening book entries
#pragma pack(push, 1)
typedef struct {
    unsigned char board[8][8];
    char color;
    unsigned char next_move[8][8];
} OpeningBookEntry;
#pragma pack(pop)

#define MAX_OPENING_ENTRIES 100000
#define OPENING_BOOK_FILE "openingBook.dat"

Node* findOpeningMove(unsigned char board1[8][8], char color);
char compareBoards(unsigned char board1[8][8], unsigned char board2[8][8]);
void addToOpeningBook(unsigned char currentBoard[8][8], char color, unsigned char nextBoard[8][8]);
void importGameToDatabase(char* folderPath);


//void generateOpeningBook(int max_moves);
//void initializeStartBoard(unsigned char board[8][8]);
//void explorePosition(Node* node, char color, int move_number, int max_depth, FILE* book);
//void processNode(Node* node, char color, int move_number, FILE* book);
