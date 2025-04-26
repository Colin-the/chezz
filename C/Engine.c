#include "chezz.h"

/*
This file will contain the key logic for searching for the best board 
*/

//functions for sorting children
int compareDesc(const void *a, const void *b){
    Node *nodeA = *(Node **)a;
    Node *nodeB = *(Node **)b;
    return (nodeB->eval - nodeA->eval);//return in descending order
}

int compareAsc(const void *a, const void *b){
    Node *nodeA = *(Node **)a;
    Node *nodeB = *(Node **)b;
    return (nodeA->eval - nodeB->eval);//return in ascending order
}

// Function that the threads will run for their task
void* thread_func(void* arg){
    ThreadTask* task = (ThreadTask*)arg;
    
    // Get local copy of pruning bounds
    pthread_mutex_lock(&shared_prune.mutex);
    int alpha = shared_prune.alpha;
    int beta = shared_prune.beta;
    pthread_mutex_unlock(&shared_prune.mutex);
    
    
    // Run alpha-beta on this child
    char opponentColor = task->maximizingColor == 'w' ? 'b' : 'w';
    AlphaBetaResult result = alphaBeta(task->root_child, task->depthLimit, alpha, beta, opponentColor, task->maximizingColor);
    
    //Update the best score according to who we are trying to find the best move for
    //fprintf(stderr,"T fin with value: %d\n", result.value);
    //printBoard(task->root_child->board);
    // if ((result.value > global_best.best_value && flipScore == 1) || (result.value < global_best.best_value && flipScore==-1)){
    //     pthread_mutex_lock(&global_best.mutex);
    //     global_best.best_value = result.value;
    //     global_best.best_node = task->root_child;
    //     pthread_mutex_unlock(&global_best.mutex);
    // }

    //we have the board with the highest possible H-value from all the sub boards here

    if (result.value > global_best.best_value){
        pthread_mutex_lock(&global_best.mutex);
        global_best.best_value = result.value;
        global_best.best_node = task->root_child;
        //fprintf(stderr,"\tupdated best to have eval: %d\n", result.value);
        pthread_mutex_unlock(&global_best.mutex);
        //printBoard(result.best_node->board);
    }
    
    
    // Update shared pruning bounds
    pthread_mutex_lock(&shared_prune.mutex);

    if (result.value > shared_prune.alpha){
        shared_prune.alpha = result.value;
    }
    else if (result.value < shared_prune.beta){
        shared_prune.beta = result.value;
    }

    pthread_mutex_unlock(&shared_prune.mutex);
    
    return NULL;
}

AlphaBetaResult alphaBeta(Node *node, int depthLimit, int alpha, int beta, char currentColor, char maximizingColor){
    //printf("\tFuntcion statr with Number of boards explored: %ld\n",numExplored);
    //printf("\tCurrent depth: %d\n\n",node->depth);
    AlphaBetaResult result;
    result.value = node->eval;
    
    result.best_node = NULL;

    //base case
    if (node->depth >= depthLimit){
        //when we reach the depth limit return the current node as the best move (as we are doing no further exploration)
        //result.best_node = node;
        return result;
    }

    //generate children for CURRENT player's moves if they haven't been already 
    if (node->childCount == 0){
        caculatePossibleMoves(node, currentColor);

        if (node -> childCount == 0){//if the node stil doesn't have any children then just return
            // if (abs(node->eval) >= gameWeight - 150){
            //     // Propagate the current node as the best move.
            //     result.best_node = node;
            // }
            return result;
        }   
    }

    //initialize best_node to first child as a fallback
    //result.best_node = node->children[0];

    //boolean value that is true if the current color is the same as the one we are trying to find the best move for
    char isMaximizing = (currentColor == maximizingColor);

    //set the current value to be below the lowest possible score if we are maximizing
    //and set it to be the highest possible score if we are minimizing so it will 
    //get replaced by the first board with a score that we encountre  
    result.value = isMaximizing ? INT_MIN : INT_MAX;

    Node *tacticalMoves[MAX_CHILDREN];
    Node *normalMoves[MAX_CHILDREN];
    char tacticalCount = 0;
    char normalCount = 0;

    for (char i = 0; i < node->childCount; i++){
        if (abs(node->children[i]->eval - node->eval) >= tacticalMoveThreshold){
            tacticalMoves[tacticalCount++] = node->children[i];
        }else{
            normalMoves[normalCount++] = node->children[i];
        }
    }

    //Now that we have catagoried the moves we will sort them in order to find the best one according to the heurstic
    if (tacticalCount > 0){
        qsort(tacticalMoves, tacticalCount, sizeof(Node *), isMaximizing ? compareDesc : compareAsc);
    }if (normalCount > 0){
        qsort(normalMoves, normalCount, sizeof(Node *), isMaximizing ? compareDesc : compareAsc);
    }


    //explore all of the tactial moves that we found first to ensure we don't miss anything
    for (int i = 0; i < tacticalCount; i++){
        Node *child = tacticalMoves[i];
        char nextColor = (currentColor == 'w') ? 'b' : 'w';
        
        AlphaBetaResult childResult = alphaBeta(child, depthLimit, alpha, beta, nextColor, maximizingColor);
        
        if (isMaximizing){
            if (childResult.value > result.value){
                result.value = childResult.value;
                result.best_node = child;
                alpha = (alpha > result.value) ? alpha : result.value;
            }
        } else{
            if (childResult.value < result.value){
                result.value = childResult.value;
                result.best_node = child;
                beta = (beta < result.value) ? beta : result.value;
            }
        }
        //only prune branches after evaluating all tactical moves
        //for sacrificial moves (negative eval change), we want to check normal moves too
        // if (i == tacticalCount - 1 && alpha >= beta){
        //     // If the best tactical move we found is significantly worse than our current position,
        //     // continue to normal moves
        //     if ((isMaximizing && result.value < node->eval - tacticalMoveThreshold) || (!isMaximizing && result.value > node->eval + tacticalMoveThreshold)){
        //         // Don't prune - continue to normal moves
        //         continue;
        //     }
        //     return result;
        // }
    }

    // if (node->depth < 2){
    //     fprintf(stderr,"Looking at normal moves at depth %d\n",node->depth);
    // }
    
    //After looking at the tactial moves next look at the normal moves
    for (int i = 0; i < normalCount; i++){
        Node *child = normalMoves[i];
        char nextColor = (currentColor == 'w') ? 'b' : 'w';
        
        AlphaBetaResult childResult = alphaBeta(child, depthLimit, alpha, beta, nextColor, maximizingColor);
        
        if (isMaximizing){
            if (childResult.value > result.value){
                result.value = childResult.value;
                result.best_node = child;
                alpha = (alpha > result.value) ? alpha : result.value;
            }
        } else{
            if (childResult.value < result.value){
                result.value = childResult.value;
                result.best_node = child;
                beta = (beta < result.value) ? beta : result.value;
            }
        }

        if (alpha >= beta){
            break;
        }
    }

    //sort the children 
    /*qsort(node->children, node->childCount, sizeof(Node *), isMaximizing ? compareDesc : compareAsc);

    for (int i = 0; i < node->childCount; i++){
        Node *child = node->children[i];
        char nextColor = (currentColor == 'w') ? 'b' : 'w';
        
        AlphaBetaResult childResult = alphaBeta(child, depthLimit, alpha, beta, nextColor, maximizingColor);
        
        if (isMaximizing){
            if (childResult.value > result.value){
                result.value = childResult.value;
                result.best_node = child;
                alpha = (alpha > result.value) ? alpha : result.value;
            }
        }else{
            if (childResult.value < result.value){
                result.value = childResult.value;
                result.best_node = child;
                beta = (beta < result.value) ? beta : result.value;
            }
        }

        if (alpha >= beta){
            break;
        }
    }*/

    return result;
}

Node* findBestMove(Node *root, int depthLimit, char maximizingColor){
//    fprintf(stderr,"Start MT find best move\n");
    // Generate moves at root if not already done
    if (root->childCount == 0){
        caculatePossibleMoves(root, maximizingColor);
    }
    
    // If no moves available, return NULL
    if (root->childCount == 0){
        return NULL;
    }

    //keep track of the number of boards where we lost the king
    char numKingLost = 0;

    //do a check for all of the children and see if we can take the king OR our king can be taken 
    for (char i = 0; i < root->childCount; i++){
        //first if we have a move that takes the enemy king we will always play it
        //if(root->children[i]->eval > gameWeight - 150) return root->children[i];
        
        //if this move results in a postion that our king could be taken in 
        if(root->children[i]->eval <= -gameWeight + 150){
            numKingLost++;//add it to our counter

            //if every single board that we have results in us losing the game 
            //this effectivly means that we got checkmated and have lost no matter what move we make
            //therefore we just play the last move in the sequence
            if (root->childCount == numKingLost) return root->children[i];

        }
    }

    
    // Sort moves to improve alpha-beta pruning efficiency
    //qsort(root->children, root->childCount, sizeof(Node *), maximizingColor == 'w' ? compareDesc : compareAsc);
    qsort(root->children, root->childCount, sizeof(Node *), compareDesc);

    //if we had at least one move that resulted in hanging our king
    if (numKingLost){
        //if there is only one board that results in us not losing immediately we can play it
        if (root->childCount == numKingLost + 1) return root->children[0];
        //fprintf(stderr,"after main logicc\n");

        //if we have at least 2 moves to explore where we don't lose our king
        //loop over all of our moves and remove the bad ones so we don't explore/play them
        for (char i = root->childCount - 1; i > root->childCount - numKingLost; i--){
            //as we are NOT going to be playing this move as it is terriable we can throw it out
            //and then it won't get played and we won't waste time looking at it's children 
            free(root->children[i]);
            //fprintf(stderr,"removed board %d that had eval: %d\n",i,root->children[i]->eval);
        }
        //update the child count to reflect the number we just removed
        root->childCount = root->childCount - numKingLost;
    }
    

    //printBoard(root->children[0]->board);

    // Initialize shared pruning bounds
    shared_prune.alpha = INT_MIN;
    shared_prune.beta = INT_MAX;
    pthread_mutex_init(&shared_prune.mutex, NULL);

    // Initialize global best tracking
    //global_best.best_value = INT_MAX;
    global_best.best_value = INT_MIN;
    //global_best.best_value = flipScore == -1 ? INT_MAX : INT_MIN;
    //global_best.best_value = maximizingColor == 'w' ? INT_MIN : INT_MAX;
    global_best.best_node = NULL;
    pthread_mutex_init(&global_best.mutex, NULL);

    // Determine number of threads to use (one per root move, up to NUM_CORES)
    int num_threads = root->childCount < NUM_CORES ? root->childCount : NUM_CORES;
    pthread_t threads[num_threads];
    ThreadTask tasks[root->childCount];
    
    // Create and initialize tasks
    for (int i = 0; i < root->childCount; i++){
        tasks[i].root_child = root->children[i];
        tasks[i].depthLimit = depthLimit - 1;  // Subtract 1 since we're at first level
        tasks[i].maximizingColor = maximizingColor;
    }

    // Launch threads to process moves
    for (int i = 0; i < root->childCount; i++){
        // Determine which thread will handle this task
        int thread_idx = i % num_threads;

        pthread_create(&threads[thread_idx], NULL, thread_func, &tasks[i]);
        
        // If we've filled all threads or this is the last move, wait for threads to finish
        if ((i + 1) % num_threads == 0 || i == root->childCount - 1){
            for (int j = 0; j < (i % num_threads) + 1; j++){
                pthread_join(threads[j], NULL);
            }
        }
    }

    //fprintf(stderr,"After threads done best eval: %d\n", global_best.best_value);
    //printBoard(global_best.best_node->board);

    // Cleanup
    pthread_mutex_destroy(&shared_prune.mutex);
    pthread_mutex_destroy(&global_best.mutex);

    //if we for some reason failed to find a move return the first legal move
    if (!global_best.best_node) {
        return root->children[0];
    }

    return global_best.best_node;
}

//old non multi threaded function
//single threaded iplimentation as it is faster for small versions
 Node * STfindBestMove(Node *root, int depthLimit, char maximizingColor){
     if (root->childCount == 0){
         caculatePossibleMoves(root, maximizingColor);
     }
  //   fprintf(stderr,"start ST find best\n");
    //if we are not going to be searching and are simpily going to be realying on the
    //board's eval for the best move
    if (depthLimit == 1){
	//sort the moves to find the one with the highest eval and return it
	qsort(root->children, root->childCount, sizeof(Node *), compareDesc);
	//fprintf(stderr,"Move eval: %d 2nd best: %d\n",root->children[0]->eval,root->children[1]->eval);
	return root->children[0];
    }

    //otherwse perform a single threaded alphaBeta search to find the best move
     AlphaBetaResult final_result = alphaBeta(root, depthLimit, INT_MIN, INT_MAX, maximizingColor, maximizingColor);

     return final_result.best_node;
 }
