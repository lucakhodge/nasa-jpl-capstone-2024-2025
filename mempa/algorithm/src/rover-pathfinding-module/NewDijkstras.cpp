#include "NewDijkstras.hpp"

 /**
  * @brief Sets up and runs NewDijkstras all in one step
  * 
  * @param heightmap contains the height values to be used for naviagtion, Usualy a chunk of a larger heightmap
  * @param chunkLocation 0,0 in the passed heightmap is this value in the whole larger heightmap (global context)
  * @param startPoint the start point for navigation in the whole larger heightmap (global context)
  * @param endPoint the end point for nagivation in the whole larger heightmap (global context)
  * @param maxSlope the maximum slope that is allowed to be navigated over
  * @param pixelSize the size (in meters) of the resolution of the heightmap
  * @return std::vector<std::pair<int,int>> retruns the route taken from the startPoint to the endPoint or if that is not in range the closest point in the heightmap chunk to the endPoint
  * 
  * @author Oscar Mikus <osmi3783@colorado.edu>
  */
std::vector<std::pair<int,int>> NewDijkstras::get_step(std::vector<std::vector<float>> &heightmap,
    std::pair<int, int> chunkLocation, std::pair<int, int> startPoint,
    std::pair<int, int> endPoint, float maxSlope, float pixelSize){
                this->setUpAlgo(heightmap, chunkLocation, startPoint, endPoint, maxSlope,
                    pixelSize);

        // std::cout << "AT: start newDijkstras" << std::endl;
        std::vector<std::pair<int, int>> realPath = newDijkstras();
        std::vector<std::pair<int, int>> globalPath;
        // std::cout << "AT: after newDijkstras" << std::endl;

        // for (size_t i = static_cast<size_t>(0); i < realPath.size(); i++) {
        for (int i = realPath.size() - 1; i >= 0; i--) {
        std::pair<int, int> localLocation = realPath.at(i);
        std::pair<int, int> globalLocation =
        std::make_pair(localLocation.first + chunkLocation.first,
                        localLocation.second + chunkLocation.second);
        globalPath.push_back(globalLocation);
        }

        // std::cout << "AT: after global loop" << std::endl;

        return globalPath;
}

/**
 * @brief Runs the search algoritm after being set up by SearchAlgorithm::set_up_algo
 * 
 * @return std::vector<std::pair<int, int>> retruns the route taken from the startPoint to the endPoint or if that is not in range the closest point in the heightmap chunk to the endPoint
 * 
 * @author Oscar Mikus <osmi3783@colorado.edu>
 */
std::vector<std::pair<int, int>> NewDijkstras::newDijkstras()
{
    if (_heightmap.empty() || _heightmap[0].empty()) {
        std::cout << "Error: Empty heightmap provided" << std::endl;
        return {};
    }
    
    int rows = _heightmap.size();
    int cols = _heightmap[0].size();

    std::vector<Node> graph(rows * cols);

    std::queue<Node*> Q;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {

            int index = calc_flat_index(cols, row, col);

            graph[index].height = _heightmap[row][col];
            graph[index].distFromPrevious = DBL_MAX;
            graph[index].visited = false;
            graph[index].neighborIndex = get_neighbor_indexs(rows, cols, row, col);
            graph[index].selfIndex = index;
            graph[index].x = col;
            graph[index].y = row;

        }
    }

    std::pair<int,int> localStart = std::make_pair(_startPoint.first - _chunkLocaiton.first, _startPoint.second - _chunkLocaiton.second);
    std::pair<int,int> localEnd = std::make_pair(std::min(rows - 1, std::max(0, _endPoint.first - _chunkLocaiton.first)), 
                                                    std::min(cols - 1, std::max(0, _endPoint.second - _chunkLocaiton.second)));

    std::cout << "localEnd: " << localEnd.first << "," << localEnd.second << std::endl;

    int startNodeIndex = calc_flat_index(cols, localStart.first, localStart.second);
    graph[startNodeIndex].distFromPrevious = 0;
    Q.push(&graph[startNodeIndex]);

    while (!Q.empty())
    {
        Node *currentNode = Q.front();
        Q.pop();
        if(currentNode->visited == true){
            continue;
        }
        currentNode->visited = true;

        // std::cout << "current Node: " << currentNode->x << "," << currentNode->y <<".currentNode height " << currentNode->height <<std::endl;

        if(currentNode->x == localEnd.second && currentNode->y == localEnd.first)
        {
            return path_to_list(graph[calc_flat_index(cols, localEnd.second, localEnd.first)]);
        }

        std::vector<Node*> neighbors;
        for(int i = 0; i < currentNode->neighborIndex.size(); i++)
        {
            Node *currentNeighbor;
            currentNeighbor = &graph[currentNode->neighborIndex[i]];
            currentNeighbor->distFromNeighbor = DBL_MAX;

            double rise = abs(currentNode->height - currentNeighbor->height);
            double run = 0.0;
            if(currentNode->x == currentNeighbor->x || currentNode->y == currentNeighbor->y){
                run = _pixelSize;
            }
            else{
                run = sqrt(pow(_pixelSize,1)+pow(_pixelSize,1));
            }

            double slope = rise/run;
            slope = std::atan(slope)*180/M_PI;
            double distanceBetweenNodeAndNeighbor = calculate_distance_between_nodes(currentNode, currentNeighbor, rise, _pixelSize);
            currentNeighbor->distFromNeighbor = distanceBetweenNodeAndNeighbor;
            

            if (slope <= _maxSlope)
            {
                double alt = currentNode->distFromPrevious + distanceBetweenNodeAndNeighbor;
                neighbors.push_back(currentNeighbor);


                if (alt < currentNeighbor->distFromPrevious)
                {
                    currentNeighbor->distFromPrevious = alt;
                    currentNeighbor->previous = currentNode;
                }

            }
        }

        //sort neighbors by desried value here so that the first in the list is the "closests" and the last is "farthest"
        std::sort(neighbors.begin(), neighbors.end(), compare_nodes_by_dist_from_neighbor);

        for (Node* node : neighbors){
            if(node->visited != true)
            {
                Q.push(node);
            }
        }

    }

    std::cout << "No route found " << std::endl;
    return {};

}

/**
 * @brief finds the index of a value in a 1d array context when you know where it is in a 2d array context
 * 
 * @param cols number of columns in your 2d array
 * @param row the row position of your desired element in a 2d array
 * @param col the column position of your desired element in a 2d array
 * @return int The index for your desired element in a 1d array
 * 
 * @author Oscar Mikus <osmi3783@colorado.edu>
 */
int NewDijkstras::calc_flat_index(int cols, int row, int col) {
    return row * cols + col;
}

/**
 * @brief gets the 1d index of the neighbors of an element in a 2d array context
 * 
 * @param rows number of rows in your 2d array
 * @param cols number of columns in your 2d array
 * @param row the row of your target element (The one you are trying to get the neighbor indexs of)
 * @param col the column of your target element (The one you are trying to get the neighbor indexs of)
 * @return std::vector<int> 
 * 
 * @author Oscar Mikus <osmi3783@colorado.edu>
 */
std::vector<int> NewDijkstras::get_neighbor_indexs(int rows, int cols, int row, int col)
{
    std::vector<int> out;

    if (row > 0) //if not in top row add above
    {
        out.push_back(calc_flat_index(cols, row-1, col));
    }
    if(row < rows-1) //if not in botom row add below
    {
        out.push_back(calc_flat_index(cols, row+1, col));
    }
    if(col > 0) //if not far left col then add left
    {
        out.push_back(calc_flat_index(cols, row, col-1));
    }
    if(col < cols-1) //if not far right col then add right
    {
        out.push_back(calc_flat_index(cols, row, col+1));
    }
    if (row > 0 && col > 0) 
    {
        out.push_back(calc_flat_index(cols, row-1, col-1)); // top-left
    }
    if (row > 0 && col < cols - 1) 
    {
        out.push_back(calc_flat_index(cols, row-1, col+1)); // top-right
    }
    if (row < rows - 1 && col > 0) 
    {
        out.push_back(calc_flat_index(cols, row+1, col-1)); // bottom-left
    }
    if (row < rows - 1 && col < cols - 1) 
    {
        out.push_back(calc_flat_index(cols, row+1, col+1)); // bottom-right
    }

    return out;
}

/**
 * @brief used to find the path from a node back to the start point by tracing from a final node along pointers to previous nodes until the start node is reached
 * 
 * @param finalNode The Node that coresponds to the final endPosition after the search algorithm is completed
 * @return std::vector<std::pair<int, int>> the path in x,y pairs that the search algorithm found.  
 * 
 * @author Oscar Mikus <osmi3783@colorado.edu>
 */
std::vector<std::pair<int, int>> NewDijkstras::path_to_list(Node finalNode)
{
    Node *workingNode = &finalNode;
    std::vector<std::pair<int, int>> out;

    while(workingNode->distFromPrevious != 0)
    {
        out.push_back({workingNode->x, workingNode->y});
        workingNode = workingNode->previous;
    }
    std::cout << workingNode->x << "," << workingNode->y <<std::endl;
    out.push_back({workingNode->x, workingNode->y});

    //reverse(out.begin(), out.end());
    return out;
}

/**
 * @brief used to compare two Nodes together
 * 
 * @param node1 First Node to compare
 * @param node2 Second Node to compare
 * @return true if the first Node has a smaller distance to its neighbor than the second Node
 * @return false if the first Node has a larger distance to its neighbor than the second Node
 * 
 * @author Oscar Mikus <osmi3783@colorado.edu>
 */
bool NewDijkstras::compare_nodes_by_dist_from_neighbor(Node* node1, Node* node2)
{
    return node1->distFromNeighbor < node2->distFromNeighbor;
}

/**
 * @brief calcualates the 3d distance between two nodes, takes into account diagonal/or not and the diffrernce in height between two Nodes/points
 * 
 * @param node1 First Node for distance calculation
 * @param node2 Second Node for distance calculation
 * @param rise the absolute differnece in height between node1 and node2
 * @param pixelSize the size (in meters) of the resolution of the heightmap
 * @return double the 3d distance between node1 and node2
 * 
 * @author Oscar Mikus <osmi3783@colorado.edu>
 */
double NewDijkstras::calculate_distance_between_nodes(Node* node1, Node* node2, double rise, double pixelSize)
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
