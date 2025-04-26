#include "chezz.h"

// Function to initialize the starting board
void initializeStartBoard(unsigned char board[8][8]){
    // Clear the board
    memset(board, 0, sizeof(char) * ROWS * COLS);
    
    // Set up white pieces
    board[0][0] = whiteFlinger;
    board[1][0] = whiteKnight;
    board[2][0] = whiteCannon;
    board[3][0] = whiteQueen;
    board[4][0] = whiteKing;
    board[5][0] = whiteBishop;
    board[6][0] = whiteKnight;
    board[7][0] = whiteRook;
    for (int i = 0; i < 8; i++) {
        if(i == 4) board [4][1] = whiteZombie;
        else board[i][1] = whitePawn;
    }
    
    // Set up black pieces
    board[0][7] = blackFlinger;
    board[1][7] = blackKnight;
    board[2][7] = blackCannon;
    board[3][7] = blackQueen;
    board[4][7] = blackKing;
    board[5][7] = blackBishop;
    board[6][7] = blackKnight;
    board[7][7] = blackRook;
    for (int i = 0; i < 8; i++) {
        if(i == 4) board [4][6] = blackZombie;
        else board[i][6] = blackPawn;
    }
}

// Function to process a node and store its best move in the opening book
void processNode(Node* node, char color, int move_number, FILE* book){
    // Generate all possible moves for this position
    caculatePossibleMoves(node, color);
    
    if (node->childCount == 0) {
        return;  // No moves available
    }

    // Run alpha-beta search to find the best move
    Node * result = findBestMove(node, 5, color);
    //AlphaBetaResult result = alphaBeta(node, 5, INT_MIN, INT_MAX, color, color);
    
    if (result != NULL) {
        // Store the best move in the opening book
        OpeningBookEntry entry;
        memcpy(entry.board, node->board, sizeof(char) * ROWS * COLS);
        entry.color = color;
        entry.move_number = move_number;
        memcpy(entry.next_move, result->board, sizeof(char) * ROWS * COLS);
        fwrite(&entry, sizeof(OpeningBookEntry), 1, book);
        
        fprintf(stderr, "Stored move %d for %c with eval %d\n", move_number, color);
    }
}

//recursive function to explore positions up to a given depth
void explorePosition(Node* node, char color, int move_number, int max_depth, FILE* book){
    if (move_number >= max_depth) {
        return;  // Reached maximum depth
    }

    // Process current position
    processNode(node, color, move_number, book);

    // Generate moves for current position
    caculatePossibleMoves(node, color);

    // Explore each possible move
    for (int i = 0; i < node->childCount; i++) {
        Node* child = node->children[i];
        char next_color = (color == 'w') ? 'b' : 'w';
        explorePosition(child, next_color, move_number + 1, max_depth, book);
    }
}

//function to generate and store opening book
void generateOpeningBook(int max_moves) {
    FILE* book = fopen(OPENING_BOOK_FILE, "wb");
    if (!book) {
        fprintf(stderr, "Error creating opening book file\n");
        return;
    }

    // Initialize starting position
    unsigned char startBoard[8][8];
    initializeStartBoard(startBoard);

    // Create root node
    Node root;
    memcpy(root.board, startBoard, sizeof(char) * ROWS * COLS);
    root.depth = 0;
    root.childCount = 0;
    root.eval = heuristic(startBoard, 'w');

    fprintf(stderr, "Starting opening book generation for %d moves\n", max_moves);
    
    // Start exploration from the root position
    explorePosition(&root, 'w', 0, max_moves, book);

    fclose(book);
    fprintf(stderr, "Opening book generation complete\n");
}

// int main(int argc, char **argv) {
//     if (argc != 2) {
//         fprintf(stderr, "Usage: %s <number_of_moves>\n", argv[0]);
//         return 1;
//     }

//     int max_moves = atoi(argv[1]);
//     if (max_moves <= 0) {
//         fprintf(stderr, "Please provide a positive number of moves\n");
//         return 1;
//     }

//     generateOpeningBook(max_moves);
//     return 0;
// }