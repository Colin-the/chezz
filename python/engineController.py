import sys
import time
import chezz


if __name__ == "__main__":
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
    
