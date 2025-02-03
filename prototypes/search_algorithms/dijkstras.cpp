#include "dijkstra.h"

using namespace std;

int main() 
{
    
    //given a heighmap, start, and end cordinates, find a path wiht dijkstras algo
    vector<vector<double>> heightmap0 = {  {0,0,0,0,0}, 
                                        {0,0,0,0,0}, 
                                        {0,0,0,0,0}, 
                                        {0,0,0,0,0}, 
                                        {0,0,0,0,0}};

    vector<vector<double>> heightmap1 = {  {0,1,2,3,4}, 
                                        {5,6,7,8,9}, 
                                        {10,11,12,13,14}, 
                                        {15,16,17,18,19}, 
                                        {20,21,22,23,24},
                                        {25, 26, 27, 28, 29}};
    
    vector<vector<double>> heightmap2 = {  {0,0,45,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}};

    vector<vector<double>> heightmap3 = {  {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}, 
                                        {0,0,46,0,0}};
                                    
    vector<vector<double>> heightmap4 = {
        {29.88, 58.45, 85.44, 39.52, 13.17, 76.88, 72.87, 35.43, 4.13, 2.28},
        {88.60, 2.53, 61.67, 63.92, 47.15, 87.80, 37.40, 77.93, 66.13, 94.16},
        {12.29, 58.13, 60.67, 75.29, 13.88, 51.75, 98.61, 41.15, 74.55, 73.52},
        {65.25, 83.14, 3.58, 94.01, 33.84, 5.39, 97.50, 95.89, 75.48, 4.45},
        {23.82, 42.12, 84.21, 54.86, 58.99, 11.71, 5.30, 17.88, 80.06, 65.96},
        {41.52, 61.60, 35.88, 82.56, 80.39, 18.18, 8.29, 47.61, 88.11, 84.85},
        {56.58, 12.50, 4.84, 78.87, 70.72, 6.37, 97.30, 94.68, 37.45, 10.42},
        {70.94, 97.25, 15.16, 15.39, 57.27, 25.72, 81.94, 83.33, 85.91, 92.12},
        {47.62, 93.10, 29.13, 15.45, 6.88, 28.32, 35.61, 95.39, 79.68, 16.85},
        {13.99, 6.91, 93.83, 92.77, 89.40, 81.76, 42.83, 54.16, 88.39, 28.85}
    };

    int startPoint[2] = {0,0};
    int endPoint[2] = {4,4};
    int endPoint1[2] = {9,9};

    // dijkstras(heightmap1, startPoint, endPoint, 45.0, 1.0);
    // cout << endl;
    // dijkstras(heightmap2, startPoint, endPoint, 45.0, 1.0);
    // cout << endl;
    // dijkstras(heightmap3, startPoint, endPoint, 45.0, 1.0);
    // cout << endl;
    vector<pair<int, int>> heightmap4Path = dijkstras(heightmap4, startPoint, endPoint1, 45.0, 2.0);

    vector<pair<int, int>> pathStorage = {};
    for(int i = 0; i < heightmap4Path.size(); i++)
    {
        pair<int, int> cordinate = dijkstrasStepByStep(heightmap4, startPoint, endPoint1, 45.0, 2.0, &pathStorage);
        cout << cordinate.first << "," << cordinate.second << endl;
    }

    

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

double calculateDistanceBetweenNeighbors(Node* node1, Node* node2, double rise, double pixelSize)
{
    if(node1->x == node2->x || node1->y == node2->y)
    {
        return (sqrt(pow(rise,2) + pow(pixelSize,2)));
    }
    else
    {
        double diagonalHorizontalDistance = (sqrt(pow(pixelSize,2) + pow(pixelSize,2)));
        return(sqrt(pow(rise,2) + pow(diagonalHorizontalDistance,2)));
    }
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
    if (row > 0 && col > 0) 
    {
        out.push_back(calcFlatIndex(cols, row-1, col-1)); // top-left
    }
    if (row > 0 && col < cols - 1) 
    {
        out.push_back(calcFlatIndex(cols, row-1, col+1)); // top-right
    }
    if (row < rows - 1 && col > 0) 
    {
        out.push_back(calcFlatIndex(cols, row+1, col-1)); // bottom-left
    }
    if (row < rows - 1 && col < cols - 1) 
    {
        out.push_back(calcFlatIndex(cols, row+1, col+1)); // bottom-right
    }

    return out;
}

//searches though a heightmap to find the shortest route from startPoint to endPoint.  Returns a vector of cordinate pairs representing the path.
vector<pair<int, int>> dijkstras(vector<vector<double> > &heightmap, int startPoint[2], int endPoint[2], double maxSlope, double pixelSize)
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
            //pathTrace(graph[calcFlatIndex(cols, endPoint[1], endPoint[0])]);
            return pathToList(graph[calcFlatIndex(cols, endPoint[1], endPoint[0])]);
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
            //This is going to need to be changed to incorperated
            double distanceBetweenNodeAndNeighbor = calculateDistanceBetweenNeighbors(currentNode, currentNeighbor, rise, pixelSize);
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

    cout << "No route found " << endl;
    return {};
}

//Does the same as dijkstras above but only returns the cordinates one by one.  Requires an external vecctor to store the entire path.  
pair<int, int> dijkstrasStepByStep(vector<vector<double> > &heightmap, int startPoint[2], int endPoint[2], double maxSlope, double pixelSize, vector<pair<int, int>> *pathStorage)
{
    pair<int, int> out;

    if(pathStorage->empty())
    {
        vector<pair<int, int>> realPath = dijkstras(heightmap, startPoint, endPoint, maxSlope, pixelSize);

        for(int i = 0; i < realPath.size(); i++)
        {
            pathStorage->push_back(realPath.at(i));
        }

        out = pathStorage->back();
        pathStorage->pop_back();

        return out;
    }
    else
    {
        out = pathStorage->back();
        pathStorage->pop_back();

        return out;
    }
}

//allows you to print the path inisde of the dijkstras algoritm.  
void pathTrace(Node finalNode)
{
    Node *workingNode = &finalNode;

    while(workingNode->distFromPrevious != 0)
    {
        cout << "(" << workingNode->x << "," << workingNode->y << ") , height: " << workingNode->height << endl;
        workingNode = workingNode->previous;
    }
    cout << "(" << workingNode->x << "," << workingNode->y << ") , height: " << workingNode->height << " is the start Node" << endl;
}

//takes the linked list style Nodes from iside of dijkstras and returns the path as a list of cordinates.  
vector<pair<int, int>> pathToList(Node finalNode)
{
    Node *workingNode = &finalNode;
    vector<pair<int, int>> out;

    while(workingNode->distFromPrevious != 0)
    {
        out.push_back({workingNode->x, workingNode->y});
        workingNode = workingNode->previous;
    }
    out.push_back({workingNode->x, workingNode->y});

    //reverse(out.begin(), out.end());
    return out;
}
