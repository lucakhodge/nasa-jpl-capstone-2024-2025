#include "dijkstra.h"

using namespace std;

int main() 
{
    //given a heighmap, start, and end cordinates, find a path wiht dijkstras algo
    vector<vector<int>> heightmap0 = {  {0,0,0,0,0}, 
                                        {0,0,0,0,0}, 
                                        {0,0,0,0,0}, 
                                        {0,0,0,0,0}, 
                                        {0,0,0,0,0}};

    vector<vector<int>> heightmap1 = {  {0,1,2,3,4}, 
                                        {5,6,7,8,9}, 
                                        {10,11,12,13,14}, 
                                        {15,16,17,18,19}, 
                                        {20,21,22,23,24},
                                        {25, 26, 27, 28, 29}};
    
    vector<vector<int>> heightmap2 = {  {0,0,45,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}};

    vector<vector<int>> heightmap3 = {  {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}};                                    
    

    int startPoint[2] = {0,0};
    int endPoint[2] = {4,4};

    dijkstras(heightmap1, startPoint, endPoint, 45.0, 1.0);
    cout << endl;
    dijkstras(heightmap2, startPoint, endPoint, 45.0, 1.0);
    cout << endl;
    dijkstras(heightmap3, startPoint, endPoint, 45.0, 1.0);

}

class Node {
    public:
        int height;
        Node *previous;
        double distFromPrevious;
        double distFromNeighbor;
        bool visited;
        vector<int> neighborIndex;
        int selfIndex;
        int x;
        int y;
};

bool compareNodesByHeight(Node* node1, Node* node2) {
    return node1->height < node2->height;
}

bool compareNodesByDistFromNeighbor(Node* node1, Node* node2){
    return node1->distFromNeighbor < node2->distFromNeighbor;
}

int calcFlatIndex(int cols, int row, int col)
{
    return row * cols + col;
}

vector<int> getNeighborIndexs(int rows, int cols, int row, int col)
{
    vector<int> out;

    if (row > 0) //if not in top row add above
    {
        out.push_back(calcFlatIndex(cols, row-1, col));
    }
    if(row < rows-1) //if not in botom row add below
    {
        out.push_back(calcFlatIndex(cols, row+1, col));
    }
    if(col > 0) //if not far left col then add left
    {
        out.push_back(calcFlatIndex(cols, row, col-1));
    }
    if(col < cols-1) //if not far right col then add right
    {
        out.push_back(calcFlatIndex(cols, row, col+1));
    }

    return out;
}

int dijkstras(vector<vector<int> > &heightmap, int startPoint[2], int endPoint[2], double maxSlope, double pixelSize)
{
    //get height and length of heightmap
    int rows = heightmap.size();
    int cols = heightmap[0].size();

    int numVertices = rows * cols;

    Node graph[numVertices];

    queue<Node*> Q;

    for (int a = 0; a < rows; a++)
    {
        for (int b = 0; b < cols; b++)
        {
            int index = calcFlatIndex(cols, a, b);
            graph[index].height = heightmap[a][b];
            graph[index].visited = false;
            graph[index].neighborIndex = getNeighborIndexs(rows, cols, a, b);
            graph[index].selfIndex = index;
            graph[index].distFromPrevious = DBL_MAX;
            graph[index].x = b;
            graph[index].y = a;
        }
    }

    int startNodeIndex = calcFlatIndex(cols, startPoint[0], startPoint[1]);
    graph[startNodeIndex].distFromPrevious = 0;
    Q.push(&graph[startNodeIndex]);

    while(!Q.empty())
    {
        //pop node with min distance and set viseted to true
        Node *currentNode = Q.front();
        Q.pop();
        currentNode->visited = true;

        if(currentNode->x == endPoint[1] && currentNode->y == endPoint[0])
        {
            pathTrace(graph[calcFlatIndex(cols, endPoint[1], endPoint[0])]);
            return 1;
        }

        //add unvisited neighbors of that node "shortests/closeest/easiest to drive to" to Q and upate their distance from source and their previous nodes
        //for each neighbor of current_node alt ← dist[u] + Graph.Edges(u, v)

        vector<Node*> neighbors;
        for(int i = 0; i < currentNode->neighborIndex.size(); i++)
        {
            Node *currentNeighbor;
            currentNeighbor = &graph[currentNode->neighborIndex[i]];
            currentNeighbor->distFromNeighbor = DBL_MAX;

            double rise = abs(currentNode->height - currentNeighbor->height);
            double slope = rise/pixelSize;
            double distanceBetweenNodeAndNeighbor = (sqrt(pow(rise,2) + pow(pixelSize,2)));
            currentNeighbor->distFromNeighbor = distanceBetweenNodeAndNeighbor;
            

            if (slope <= maxSlope)
            {
                double alt = currentNode->distFromPrevious + distanceBetweenNodeAndNeighbor;
                neighbors.push_back(currentNeighbor);

                //cout << currentNeighbor->height << endl;

                if (alt < currentNeighbor->distFromPrevious)
                {
                    currentNeighbor->distFromPrevious = alt;
                    currentNeighbor->previous = currentNode;
                }
            }
        }
        
        //sort neighbors by desried value here so that the first in the list is the "closests" and the last is "farthest"
        sort(neighbors.begin(), neighbors.end(), compareNodesByDistFromNeighbor);

        for (Node* node : neighbors){
            if(node->visited != true)
            {
                Q.push(node);
            }
        }
    }

    for (int i = 0; i < numVertices; i++)
    {
        // cout << dist[i] << endl;
        // cout << prev[i][0] << " " << prev[i][1] << endl;
        /*cout << "graph[" << i << "] has height " << graph[i].height << " and neighbors w/ height ";
        for (int j = 0; j < graph[i].neighborIndex.size(); j++)
        {
            cout << graph[graph[i].neighborIndex[j]].height << " ";
        }
        cout << endl;*/
        //cout << graph[i].distFromPrevious << endl;
    }


    cout << "No route found " << endl;
    return -1;
}

void pathTrace(Node finalNode)
{
    Node *workingNode = &finalNode;

    while(workingNode->distFromPrevious != 0)
    {
        cout << "(" << workingNode->x << "," << workingNode->y << ") , height: " << workingNode->height << endl;
        workingNode = workingNode->previous;
    }
}
