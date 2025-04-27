import sys
import time
import numpy as np
import chezz

def list_to_unsigned_char_array(data_list):
    try:
        np_array = np.array(data_list, dtype=np.uint8)
        return np_array
    except ValueError:
        print("Error: Input list could not be converted to a NumPy array.")
        return None
  
exampleBoard = [
    [0, 0, 0, 0, 0, 0, 0, 0],
    [11, 11, 11, 11, 11, 11, 11, 11],
    [0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0],
    [1, 1, 1, 1, 1, 1, 1, 1],
    [0, 0, 0, 0, 0, 0, 0, 0],
]

board = list_to_unsigned_char_array(exampleBoard)
src = 0
dest = 9
color_to_move = ord('w')

if __name__ == "__main__":
    print("live")
    eval = chezz.getSearchDepth(int(30000),int(950))

    print("From C we have ", ord(eval))

    is_valid = chezz.isValidNextMove(exampleBoard, src, dest, color_to_move)

    if is_valid:
        print("The move is valid!")
    else:
        print("The move is invalid!")

    if len(sys.argv) > 2 and sys.argv[2] == "-f":#file option
        #print("Using file")
        algorthymToUse = sys.argv[1] #0 for BFS, 1 for H1 and 2 for H2
        input_file = sys.argv[3]
        array = create_2d_array(input_file)
        #print_2d_array(array)
        flat_board = flatten_board(array)
        timeStart = time.time()
        chezz.solve_bfs(flat_board, int(algorthymToUse))
        endTime = time.time()
        print("Elapsed time:",endTime-timeStart)

    elif len(sys.argv) > 2 and sys.argv[2] == "-s":#option for running all boards to get stats
        algorthymToUse = int(sys.argv[1])  # 0 for BFS, 1 for H1, 2 for H2
        masterTime = time.time()
        for i in range(1,41):
            input_file = f'../testBoards/{i}'
            array = create_2d_array(input_file)
            #print_2d_array(array)
            flat_board = flatten_board(array)
            timeStart = time.time()
            chezz.solve_bfs(flat_board, algorthymToUse)
            endTime = time.time()
            print("Elapsed time Solving board",i,": ",endTime-timeStart)

        print("Elapsed for all trials: ",time.time() - masterTime)
    
