/* Do not make any changes in the skeleton. Your submission will be invalidated if the skeleton is changed */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>

#define NUM_THREADS_MAX 4

// can be made local if required
int N;
pthread_mutex_t lock;
int found = 0;

// Helper struct, can utilize any other custom structs if needed 
typedef struct{
	int x,y;
} pair;

pair possibleMoves[8] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};

// Not necessary to use this function as long as same printing pattern is followed 
void print_path(pair path[], int n){
	for(int i=0;i<n;i++){
		printf("%d,%d|",path[i].x,path[i].y);
	}
}

//helper fns
int checkWithinBoard(int x, int y) {        //checks if x, y is within bounds
    if(x >= 0 && x < N && y >= 0 && y < N)
        return 1;
    
    return 0;
}

int calcCellNo(int x, int y) {      // calcs cell number or square no for a particular coord
    return N*y + x;
}

int notVisited(int arr[], int x, int y) {          // checks if (x, y) has not been visited before
    if(checkWithinBoard(x, y) == 1 && arr[calcCellNo(x, y)] < 0)
        return 1;
    
    return 0;
}

int calcDeg(int arr[], int x, int y) {          // calculates degree of (x, y) as per warnsdorff's heuristic
    int ctr = 0;
    
    for(int i = 0; i < 8; i++)
        if(notVisited(arr, x + possibleMoves[i].x, y + possibleMoves[i].y) == 1)
            ctr++;

    return ctr;
}

int makeMove(int arr[], int *x, int *y) {
    int idx_minDeg = -1;
    int currMinDeg = INT_MAX;
    int md;
    int newX, newY;

    int selectOne = rand() % 8;

    for(int i = 0; i < 8; i++) {
        
        int dirIdx = (selectOne + i) % 8;
        newX = *x + possibleMoves[dirIdx].x;
        newY = *y + possibleMoves[dirIdx].y;
        md = calcDeg(arr, newX, newY);

        if(notVisited(arr, newX, newY) == 1 && md < currMinDeg) {
            currMinDeg = md;
            idx_minDeg = dirIdx;
        }
    }

    if(idx_minDeg == -1)
        return 0;
    
    newX = *x + possibleMoves[idx_minDeg].x;
    newY = *y + possibleMoves[idx_minDeg].y;
    
    arr[calcCellNo(newX, newY)] = arr[calcCellNo(*x, *y)] + 1;

    *x = newX;
    *y = newY;

    return 1;
}

void printPath(int arr[]) {
    
    pair path[N * N];

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            path[arr[calcCellNo(i, j)] - 1].x = i;
            path[arr[calcCellNo(i, j)] - 1].y = j;
        }
    }

    for(int i = 0; i < N * N; i++)
        printf("%d,%d|", path[i].x, path[i].y);
}

int possiblePath(int initX, int initY) {

    int moveArr[N * N];

    memset(moveArr, -1, N * N * sizeof(moveArr[0]));
    
    int x = initX;
    int y = initY;

    moveArr[calcCellNo(x, y)] = 1;

    for(int i = 0; i < N * N - 1; i++)
        if(makeMove(moveArr, &x, &y) == 0)
            return 0;
    
    found = 1;

    pthread_mutex_lock(&lock);
    // entering critical section to print path once any thread has found soln and then exiting the program to kill other threads searching for paths
    printPath(moveArr);
    exit(0);
    // unreachable code beyond this pt, but including for the sake of completeness of the mechanism
    pthread_mutex_unlock(&lock);
    
    return 1;
}

void *callerFunc(void *args) {
    int ctr = 0;
    pair *p = (pair *) args;
    while(!(possiblePath(p -> x, p -> y)) && ctr < 120)    // decides num of iters
        ctr++;

    return NULL;
}

int main(int argc, char *argv[]) {
	
	if (argc != 4) {
		printf("Usage: ./Knight.out grid_size StartX StartY");
		exit(-1);
	}
	
	N = atoi(argv[1]);
	int StartX=atoi(argv[2]);
	int StartY=atoi(argv[3]);

	/* Do your thing here */
    pthread_mutex_init(&lock, NULL);
    pthread_t tID[NUM_THREADS_MAX];

    pair arg;
    arg.x = StartX;
    arg.y = StartY;

    for(int i = 0; i < NUM_THREADS_MAX; i++)
        pthread_create(&tID[i], NULL, callerFunc, (void *) &arg);

    for(int i = 0; i < NUM_THREADS_MAX; i++)
        pthread_join(tID[i], NULL);
    
    if(found == 0)
        printf("No Possible Tour");

	return 0;
}