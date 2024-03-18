//-------------------------------------------------by Ye Liang at 14:46 on 9th Dec. 2014-----------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
#include <cmath>

#define NMAX 500
#define MMAX (NMAX+31)/32
#define SETWD(pos) ((pos)>>5)//define basic computation operation
#define SETBT(pos) ((pos)&037)//define basic computation operation
#define ADD_ELEMENT(setadd, pos) ((setadd)[SETWD(pos)] |= bit[SETBT(pos)])//define basic computation operation: add 1 to assigned position
#define DEL_ELEMENT(setadd, pos) ((setadd)[SETWD(pos)] &= ~bit[SETBT(pos)])//define basic computation operation: minus 1 to assigned position
#define IS_ELEMENT(setadd, pos) ((setadd)[SETWD(pos)] &  bit[SETBT(pos)])//define basic computation operation: examine if it is 1 in the assigned position.
#define POP_COUNT(x) (bytecount[(x) >> 24 & 0377] + bytecount[(x) >> 16 & 0377] + bytecount[(x) >> 8 & 0377] + bytecount[(x) & 0377])

extern "C" {
unsigned int bit[] =//define bit used for position calculation.
        {020000000000, 010000000000,
         04000000000, 02000000000, 01000000000,
         0400000000, 0200000000, 0100000000,
         040000000, 020000000, 010000000,
         04000000, 02000000, 01000000,
         0400000, 0200000, 0100000,
         040000, 020000, 010000,
         04000, 02000, 01000,
         0400, 0200, 0100,
         040, 020, 010,
         04, 02, 01
        };
int bytecount[] =
        {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};
int nCprEdg;//# of integers I use to save the neighbors.
int nVerNum;//# of vertices of the current graph,
//the two variables above can only be changed in the Read_Graph function, but their value must be used in many functions as the loop limit.

int BFS_Array(int BfsArray[NMAX], int G[NMAX][MMAX], int Red[MMAX],
              int Root) {//Using BFS search to generate a SearchOrder, and our search behavior will follow this order.
    int i, j, nParent, nPointer, FlagExist;
    //nParent:integer index for indicating the current parent position in the BFQ.
    //nPointer:integer index on the end of the BFQ
    //FlagExist:Flag to show if the vertex was been visited.
    nParent = nPointer = 0;
    BfsArray[nPointer++] = Root;//This is BFQ
    while (nParent < nPointer + 1) {
        for (i = 0; i < nVerNum; i++) {
            if (IS_ELEMENT(G[nParent], i) &&
                !IS_ELEMENT(Red, i))//I traverse the parents' neighbors, and add them in to the BFQ.
            {//However, if some of the neighbor is already in the Red set, that means the neighbor has been fixed in dominating set for some reason,
                //so I should ignore it when I are searching the min dom set.If it is not in our BFQ(will be the SearchOrder), the algorithm
                // will leave it alone.
                for (FlagExist = j = 0; j < nPointer; j++)
                    if (BfsArray[j] == i)//before adding the neighbor into the BFQ, check if it is already in.
                    {
                        FlagExist = 1;
                        break;
                    }
                if (!FlagExist)BfsArray[nPointer++] = i;//if it is not in the current BFQ, Add it in!
            }
        }
        nParent++;
    }
    return 0;
}

int Set_Size(int Set[])//to count the number of ones in the set.
{
    int i, m, d = 0;
    m = nCprEdg;
    for (i = 0; i < m; i++)
        d += POP_COUNT(Set[i]);
    return d;
}

int Read_Graph(int G[NMAX][MMAX])// to read the graph into the memory
{
    int i, j, u, d;
    nVerNum = nCprEdg = 0;
    //Reset the Vertex number and the integer number used for Edges(neighbor) in Compressed Adjacency Matrix.
    scanf("%d", &nVerNum);
    //Read the Vertex Number
    if (nVerNum == 0)return (0);
    //Nothing get, return 0
    nCprEdg = (nVerNum + 31) / 32;
    //in the compressed adjacency matrix data structure, # of integers will be used to memory the neighbors can be calculated by this fomula.
    for (i = 0; i < nVerNum; i++) {
        for (j = 0; j < nCprEdg; j++) {
            G[i][j] = 0;
            //Set the compressed adjacency matrix 0 in any position.
        }
    }
    for (i = 0; i < nVerNum; i++) {
        scanf("%d", &d);
        for (j = 0; j < d; j++) {
            scanf("%d", &u);
            ADD_ELEMENT(G[i], u);
            //Set the bit : 1 in the connected position
        }
        ADD_ELEMENT(G[i], i);
        //Set itself 1(bit).
    }
    return 1;//read successfully.
}

int
Can_Be_Blue(int G[NMAX][MMAX], int Blue[])//if the new added point can be blue(fixed it out of dominating set)return 1
{
    int i, j, d;
    for (i = 0; i < nVerNum; i++)
        // if any of the vertices can not be dominated by the time, the new added vertex must be removed from the blue set
    {
        for (d = j = 0; j < nCprEdg; j++)
            // if all the neighbors(including itself) are 0(no connection) except those are in the blue set d will be 0.
            if (G[i][j] & ~Blue[j])
                // if there is still some neighbor which is not in the blue set, or if itself is not in the blue set, d will be 1
            {
                d = 1;
                break;
            }
        if (!d)return 0;
        // if d is 0, that means some vertex can not be dominated already, return 0.
    }
    return 1;// else return 1.
}

void Refresh_DomtedSet(int G[NMAX][MMAX], int DomtedSet[],
                       int Red[])//After adding or deleting a vertex into(from) the dominating set,
//I will use this function to refresh the dominated set by now.
{
    int i, j;
    for (j = 0; j < nCprEdg; j++)DomtedSet[j] = 0;
    for (i = 0; i < nVerNum; i++) {
        if (IS_ELEMENT(Red, i))
            //if the vertex i is in the dominating set
        {
            for (j = 0; j < nCprEdg; j++)// add all of its neighbors(including itself) dominated!
                DomtedSet[j] = DomtedSet[j] | G[i][j];
        }
    }
}

int Check_Dominated(int DomtedSet[])//function used to check if the set can dominate the graph
{
    int nUndom;
    nUndom = nVerNum - Set_Size(DomtedSet);
    return nUndom;
}

int Refresh_Max_Extra(int G[NMAX][MMAX], int Red[], int Blue[],
                      int DomtedSet[])//Used to find the maximum number of undominated vertices that can be dominated
//by the current white vertices. this is pretty usefull to find a good bound.
{
    int i, j, nDeg, nMaxExtra;
    int FixedSet[MMAX];
    int CanDomSet[MMAX];
    nDeg = 0;
    nMaxExtra = 1;
    for (i = 0; i < nCprEdg; i++) {
        FixedSet[i] = Red[i] | Blue[i];
        //find out the white vertices
        CanDomSet[i] = 0;
    }
    for (i = 0; i < nVerNum; i++) {
        if (!IS_ELEMENT(FixedSet, i)) {
            for (j = 0; j < nCprEdg; j++)CanDomSet[j] = G[i][j] & ~DomtedSet[j];
            // for each white vertices, check how many undominated vertices they can dominate.
            nDeg = Set_Size(CanDomSet);
            nMaxExtra = (nMaxExtra > nDeg ? nMaxExtra : nDeg);
        }
    }
    return nMaxExtra;
    //return the maximum number
}

int Print_Set(int n, int Set[], int j)//just print the set
{
    int i;
    for (i = 0; i < n; i++)
        if (IS_ELEMENT(Set, i) && i != j)printf(" %d", i);
    printf("\n");
    return 0;
}

void Find_Dom_Set(int G[NMAX][MMAX], int Red[], int Blue[], int DomtingSet[], int DomtedSet[], int nDepthIndex,
                  int SearchOrder[NMAX], int *nMinBound) {
    // recursive function for finding the minimum dominating set
    int i, nMaxExtra, nUnDom, nDom;
    //i: index integer used in 'for' loops
    //nMaxExtra: integer used to represent the maximum number of undominated vertices that one more vertex can dominate at most by the time.
    //nUnDom: integer used to represent the number of undominated vertices so far.
    //nDom: integer used to represent the size of the dominating set.

    ADD_ELEMENT(Blue, SearchOrder[nDepthIndex]);//firstly, make the current vertex blue(out of dominating set).

    if (Can_Be_Blue(G, Blue))
        Find_Dom_Set(G, Red, Blue, DomtingSet, DomtedSet, nDepthIndex + 1, SearchOrder,
                     nMinBound);//if it can be blue, I
    //will continue to consider the next vertex according to the SearchOrder.

    DEL_ELEMENT(Blue, SearchOrder[nDepthIndex]);
    ADD_ELEMENT(Red, SearchOrder[nDepthIndex]);
    //After trying blue, I make it Red(part of dominating set).

    for (i = 0; i < nCprEdg; i++) DomtedSet[i] = DomtedSet[i] | G[SearchOrder[nDepthIndex]][i];
    //After making the vertex a part of dominating set, I want to update the dominated set by now.
    nUnDom = Check_Dominated(DomtedSet);
    //check if all the vertices are dominated.
    nDom = Set_Size(Red);//calculate the size of the current dominating set.
    if (nUnDom == 0)
        // if the number of undominated vertices is zero, I will get a dominating set.
    {
        printf("0 %d ", nDom);
        //I don't know if it is optimum by now...
        Print_Set(nVerNum, Red, -1);
        //print the dominating set.
        fflush(stdout);
        if (nDom < *nMinBound)
            //keep the smallest dominating set as the best ever.
        {
            for (i = 0; i < nCprEdg; i++)DomtingSet[i] = Red[i];
            //copy set.
            *nMinBound = nDom;
            //the nMinBound means the maximum bound for the size of the dominating set, I update it when I found a smaller one.
        }
        DEL_ELEMENT(Red, SearchOrder[nDepthIndex]);
        //remove the vertex from the dominating set and return(This branch is over).
        Refresh_DomtedSet(G, DomtedSet, Red);
        //always update the dominated set when the dominating set is changing.
        return;
    }
    //if the program runs here, it means the number of undominated vertices is NOT zero.
    //then we update the n_extra: the maximum number of undominated vertices that can be dominated in one step.
    nMaxExtra = Refresh_Max_Extra(G, Red, Blue, DomtedSet);
    // if it is impossible to find a smaller size in this branch, I cut it.
    if (ceilf(((float) nUnDom / (float) nMaxExtra) + nDom + 1) > *nMinBound) {
        DEL_ELEMENT(Red, SearchOrder[nDepthIndex]);//delete the current vertex from the dominating set and return
        Refresh_DomtedSet(G, DomtedSet, Red);
        return;
    } else// After making the current vertex red, and I will have chanced to find a smaller set in this branch ,so I go on to consider the next point.
    {
        Find_Dom_Set(G, Red, Blue, DomtingSet, DomtedSet, nDepthIndex + 1, SearchOrder, nMinBound);
        DEL_ELEMENT(Red, SearchOrder[nDepthIndex]);//Having tried the blue and red, I return this branch.
        Refresh_DomtedSet(G, DomtedSet, Red);
        return;
    }
}


//Initialization, two purpose: one is to set some vertices 'red' which must be part of the dominating set.
//another is to rank the other vertices according to their degree, and return the root(vertex has the maximum degree).
int Initialise(int G[NMAX][MMAX], int Red[MMAX]) {
    int i, j, BubbleFlag, SwampTemp, Order[2][NMAX];
    for (i = 0; i < nVerNum; i++) {
        Order[0][i] = i;
        Order[1][i] = Set_Size(G[i]);
        if (Order[1][i] == 2)
            //if the vertex just has degree 1(2 ones in compressed adjacency matrix), then its neighbor must be part of the dominating set.
        {
            for (j = 0; j < nVerNum; j++)if (IS_ELEMENT(G[i], j) && i != j) ADD_ELEMENT(Red, j);
        } else {
            if (Order[1][i] == 1) ADD_ELEMENT(Red, i);
            // if the vertex is separated, then it must be part of the dominating set.
        }
    }
    for (i = 0; i < nVerNum; i++)
        //rand the vertices according to the degree.
    {
        for (j = BubbleFlag = 0; j < nVerNum - 1; j++) {
            if (Order[1][j] < Order[1][j + 1])
                //bubble rank
            {
                SwampTemp = Order[1][j];
                Order[1][j] = Order[1][j + 1];
                Order[1][j + 1] = SwampTemp;
                SwampTemp = Order[0][j];
                Order[0][j] = Order[0][j + 1];
                Order[0][j + 1] = SwampTemp;
                BubbleFlag = 1;
            }
        }
        if (!BubbleFlag)break;
    }
    return Order[0][0];
    //return the vertes number of the vertex which has the largest degree as the root.
}

int main(void) {
    int nCount, Root, Bound, i;
    int G[NMAX][MMAX];
    int DomtingSet[MMAX];
    int DomtedSet[MMAX];
    int Red[MMAX];
    int Blue[MMAX];
    int SearchOrder[NMAX];
    nCount = 1;//count the number of the graphs
    while (Read_Graph(G)) {
        Bound = nVerNum;//Reset bound.
        for (i = 0; i < MMAX; i++)//Reset
        {
            Red[i] = 0;
            Blue[i] = 0;
            DomtedSet[i] = 0;
            DomtingSet[i] = 0;
        }
        printf("%d %d\n", nCount, nVerNum);//print
        Root = Initialise(G, Red);//initialization
        BFS_Array(SearchOrder, G, Red, Root);//use BFS to create a search order.
        Find_Dom_Set(G, Red, Blue, DomtingSet, DomtedSet, 0, SearchOrder, &Bound);//main algorithm
        printf("1 %d ", Set_Size(DomtingSet));//print the optimum set.
        Print_Set(nVerNum, DomtingSet, -1);
        fflush(stdout);
        nCount++;
    }
}
}
//-------------------------------------------------by Ye Liang at 14:46 on 9th Dec. 2014-------------------------------------------------------------