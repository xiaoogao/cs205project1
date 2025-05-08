#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cmath>

using namespace std;

// Node class to represent each state in the search
class Node {
public:
    vector<int> state;  // the 9 man in a trench state
    Node* parent;       // pointer to parent node,  used to reconstruct the solution path once the goal is reached
    string action;      // the move that led to this node 
    int path_cost;      // the cost of reaching this node (g(n)), each move has a uniform cost of 1
    double heuristic_cost; // Heuristic cost of A-star, h(n)

    // constructor
    Node(vector<int> state, Node* parent = NULL, string action = "", int path_cost = 0, double heuristic_cost = 0)
        : state(state), parent(parent), action(action), path_cost(path_cost), heuristic_cost(heuristic_cost) {}

    // comparison operator for the priority queue based on total cost, f(n)
    bool operator<(const Node& other) const {
        return (path_cost + heuristic_cost) > (other.path_cost + other.heuristic_cost); 
        // reverse for priority queue (min-heap)
        // the node with the smallest cost is given priority
    }
};

// Problem class to represent the puzzle and its goal state
// the possible moves from a state, 
// and the operator to apply a move
class Problem {
public:
    vector<int> initial_state;
    vector<int> goal_state;

    // constructor
    Problem(const vector<int>& initial, const vector<int>& goal)
        : initial_state(initial), goal_state(goal) {}

    // check if current state is the goal
    bool is_goal(const vector<int>& state) {
        return state == goal_state;
    }

    // find the blank positions in the current state
    vector<int> findBlankPositions(const vector<int>& state) {
        vector<int> positions;

        for (size_t i = 0; i < state.size(); i++)
        {
            if (state[i]==0)
            {
                positions.push_back(i);
            }
            
        }
        return positions;
    }

    // get possible moves from the current state
    vector< vector<string> > get_possible_moves(const vector<int>& state) {
        
        vector<int> blank_index = findBlankPositions(state); //searches for the blank tile && convert the iterator into an index
        
        vector< vector<string> > moves(4); // store the valid move directions

        // check valid moves for each blank position
        for (size_t i = 0; i < blank_index.size(); i++)
        {
            if (blank_index[i] > 0 && blank_index[i] < 10) moves[i].push_back("left");
            if (blank_index[i] > -1 && blank_index[i] < 9) moves[i].push_back("right");
            if (blank_index[i] == 3) moves[i].push_back("blank1 up");
            if (blank_index[i] == 5) moves[i].push_back("blank2 up");
            if (blank_index[i] == 7) moves[i].push_back("blank3 up");
            if (blank_index[i] == 10) moves[i].push_back("blank1 down");
            if (blank_index[i] == 11) moves[i].push_back("blank2 down");
            if (blank_index[i] == 12) moves[i].push_back("blank3 down");
        }

             
        return moves;
    }

    // takes the current state & apply the operator to move ith blank
    // and return the new state
    vector<int> apply_operator(const vector<int>& state, const int& index, const string& move) {
        vector<int> new_state = state;

        vector<int> blank_index = findBlankPositions(state);
        int blank_to_move = blank_index[index]; // select the blank to move

        // change the blank index from 3 to 10 to empty the first trench
        if (move == "blank1 up") swap(new_state[blank_to_move], new_state[blank_to_move + 7]); 
        // change the blank index from 5 to 11
        else if (move == "blank2 up") swap(new_state[blank_to_move], new_state[blank_to_move + 6]); 
        // change the blank index from 7 to 12
        else if (move == "blank3 up") swap(new_state[blank_to_move], new_state[blank_to_move + 5]);
        // change the blank index from 3 to 10 to fill the first trench
        else if (move == "blank1 down") swap(new_state[blank_to_move], new_state[blank_to_move - 7]);
        else if (move == "blank2 down") swap(new_state[blank_to_move], new_state[blank_to_move - 6]);
        else if (move == "blank3 down") swap(new_state[blank_to_move], new_state[blank_to_move - 5]);
        else if (move == "left") swap(new_state[blank_to_move], new_state[blank_to_move - 1]);
        else if (move == "right") swap(new_state[blank_to_move], new_state[blank_to_move + 1]);

        return new_state;
    }
};

// helper function to print the puzzle state in trench format
void print_state(const vector<int>& state) {
    std::cout << "      " << state[10] << "   " << state[11] << "   " << state[12] << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << state[i] << " ";
    }
    std::cout << std::endl;
}


// Uniform Cost Search algorithm with h(n) hardcoded to 0
void uniform_cost_search(Problem& problem) {
    priority_queue<Node> frontier;  // min-heap priority queue for UCS
    set<vector<int> > explored;      // keep track of explored states

    frontier.push(Node(problem.initial_state));  // start with initial state
    int nodes_expanded = 0;  // counter for expanded nodes
    int max_queue_size = 1;   // track the max size of queue


    while (!frontier.empty()) { // still nodes to be expanded
        Node node = frontier.top(); // removes the node with  lowest path cost from the priority queue
        frontier.pop(); // removes the node from the frontier

        // print the current state being expanded
        cout << "Expanding state:" << endl;
        print_state(node.state);

        // Goal test
        if (problem.is_goal(node.state)) { // checks if the current node's state matches the goal state
            cout << "Goal!!!" << endl;
            cout << "Solution found at depth: " << node.path_cost << endl;
            cout << "Nodes expanded: " << nodes_expanded << endl;
            cout << "Maximum queue size: " << max_queue_size << endl;
            return; // end
        }else
        {nodes_expanded++;} // increments the counter
        
        explored.insert(node.state); // adds current node state to the explored set

        // get possible moves and expand nodes
        vector <vector<string>> possible_moves = problem.get_possible_moves(node.state); // store all valid moves from current state

        for (size_t i = 0; i < possible_moves.size(); ++i) { // iterate through each blank position      
            for (size_t j = 0; j < possible_moves[i].size(); j++) // iterate through each possible move for ith blank position
            {
                const string& move = possible_moves[i][j]; // get one move for the selected blank
                vector<int> new_state = problem.apply_operator(node.state, i, move); // aply the move to the selected blank and returns a new state
            
                // check if the new state has already been explored
                if (explored.find(new_state) == explored.end()) { // if not then create a new node for it, if in the explored set then skip
                    int new_path_cost = node.path_cost + 1; // increment g(n), path cost
                    Node child(new_state, &node, move, new_path_cost); // h(n) = 0 for UCS
                    frontier.push(child); //adds the child node to the frontier
                }
            }
        }

        // update maximum queue size
        max_queue_size = max(max_queue_size, (int)frontier.size());
        cout << "Queue size: " << max_queue_size << endl;
        // Display the contents of the frontier (queue)
        // priority_queue<Node> temp_queue = frontier;  // Create a temporary copy of the frontier
        // cout << "Contents of the queue:";
        // while (!temp_queue.empty()) {
        //     Node temp_node = temp_queue.top();
        //     temp_queue.pop();
        //     print_state(temp_node.state);  // Print each state's configuration
        //     cout << "Path cost: " << temp_node.path_cost << endl;
        // }
        // cout << "End of queue contents." << endl << endl;
        
    }

     // explored all possible states and can't to find the goal state
    cout << "No solution found." << endl;
    cout << "Nodes expanded: " << nodes_expanded << endl;
    cout << "Maximum queue size: " << max_queue_size << endl;
}

// Calculate the heuristic cost
double Calheuristic(const vector<int>& state, const vector<int>& goal){

    // //double euclidean = 0;
    // int manhattan = 0;

    // auto it = find(state.begin(), state.end(), 1); 
    // int index = distance(state.begin(), it); // get the index of 1

    // int row = 0;
    // int column = 0; // initial the row number and column number of 1
    
    // if(index <= 9){ // 1 is in the bottom row
    //     column = index % 10;
    // }else if (index == 10){ // case when 1 is in the first block on the top row
    //     row = 1;
    //     column = 3;
    // }else if (index == 11){ // case when 1 is in the second block on the top row
    //     row = 1;
    //     column = 5;
    // }else if (index == 12){ // case when 1 is in the thrid block on the top row
    //     row = 1;
    //     column = 7;
    // } 

    // int dx = column - 0;
    // int dy = row - 0;
    // manhattan = abs(dx) + abs(dy);

    //double euclidean = 0;

    int manhattan = 0;
    int dis = 0;
    
    for (size_t i=0; i < state.size(); ++i){ // iterate through the state vector
        if(state[i] != 0 && state[i] != goal[i]){
            auto it = find(goal.begin(), goal.end(), state[i]); 
            int index = distance(goal.begin(), it); // get the index in the goal
            
            int row1 = 0;
            int row2 = 0;
            int column1 = 0;
            int column2 = 0;

            if(index <= 9){ // find the row and column in the goal state
                row1 = 0;
                column1 = index % 10;
            }else if (index == 10){ // case when sergeant is in the first block on the top row
                row1 = 1;
                column1 = 3;
            }else if (index == 11){ // case when sergeant is in the second block on the top row
                row1 = 1;
                column1 = 5;
            }else if (index == 12){ // case when sergeant is in the thrid block on the top row
                row1 = 1;
                column1 = 7;
            }

            if(i <= 9){ // find the row and column in the current state
                row2 = 0;
                column2 = i % 10;
            }else if (i == 10){ // case when sergeant is in the first block on the top row
                row2 = 1;
                column2 = 3;
            }else if (i == 11){ // case when sergeant is in the second block on the top row
                row2 = 1;
                column2 = 5;
            }else if (i == 12){ // case when sergeant is in the thrid block on the top row
                row2 = 1;
                column2 = 7;
            }

            int dx = column1 - column2;
            int dy = row1 - row2;
            dis = abs(dx) + abs(dy); // find the manhattan distance for each misplaced sergeant
            
            manhattan = manhattan + dis; // find the sum of manhattan distan for all misplaced sergeant
        }

    }

    return manhattan;
}

// Heuristic search use manhattan distance cost
void manhattan_heuristic_search(Problem& problem){
    priority_queue<Node> frontier;
    set<vector<int> > explored;

    double initial_heuristic = Calheuristic(problem.initial_state, problem.goal_state);
    frontier.push(Node(problem.initial_state, nullptr, "", 0, initial_heuristic));
    int nodes_expanded = 0;
    int max_queue_size = 1;

    while (!frontier.empty()) {
        Node node = frontier.top();
        frontier.pop();

        cout << "Expanding state: \n";

        if (nodes_expanded > 0)
        {
            cout << "The best state to expand with g(n) = " << node.path_cost << " and h(n) = " << node.heuristic_cost << " is..." << endl;
        }
        print_state(node.state);

        if (problem.is_goal(node.state)) {
            cout << "Goal!!!" << endl;
            cout << "Solution found at depth: " << node.path_cost << endl;
            cout << "Nodes expanded: " << nodes_expanded << endl;
            cout << "Maximum queue size: " << max_queue_size << endl;
            return;
        }else
        {
            nodes_expanded++;
        }
        explored.insert(node.state);

        // std::cout << "Explored Nodes:" <<std::endl;
        // for(size_t i = 0; i < explored.size(); ++i){     
        //     std::set< vector<int> >::iterator it = explored.begin();
        //     std::advance(it, i);
        //     print_state(*it);
        // }
        
        // get possible moves and expand nodes
        vector <vector<string>> possible_moves = problem.get_possible_moves(node.state); // store all valid moves from current state
        for (size_t i = 0; i < possible_moves.size(); ++i) { // iterate through each blank position      
            for (size_t j = 0; j < possible_moves[i].size(); j++) // iterate through each possible move for ith blank position
            {
                const string& move = possible_moves[i][j]; // get one move for the selected blank
                vector<int> new_state = problem.apply_operator(node.state, i, move); // aply the move to the selected blank and returns a new state
            
                // check if the new state has already been explored
                if (explored.find(new_state) == explored.end()) { // if not then create a new node for it, if in the explored set then skip
                    
                    double new_heuristic = Calheuristic(new_state, problem.goal_state);
                    int new_path_cost = node.path_cost + 1; // increment g(n), path cost
                    Node child(new_state, &node, move, new_path_cost, new_heuristic); // h(n) = 0 for UCS
                    frontier.push(child); //adds the child node to the frontier
                }
            }
        }
        max_queue_size = max(max_queue_size, (int)frontier.size());
    }
    cout << "No solution found." << endl;
    cout << "Nodes expanded: " << nodes_expanded << endl;
    cout << "Maximum queue size: " << max_queue_size << endl;
}

int main() {
    vector<int> initial_state(13);

    int defaultpuzzle;
    cout << "This is Xiao's puzzle solver for nine man in a trench problem." << endl;
    cout << "Type \"1\" to use a default puzzle, or \"2\" to enter your own puzzle." << endl;
    cin >> defaultpuzzle;
    if (defaultpuzzle == 1){
        // Set the default initial puzzle state
        initial_state[0] = 0;
        initial_state[1] = 2;
        initial_state[2] = 3;
        initial_state[3] = 4;
        initial_state[4] = 5;
        initial_state[5] = 6;
        initial_state[6] = 7;
        initial_state[7] = 8;
        initial_state[8] = 9;
        initial_state[9] = 1;
        initial_state[10] = 0;
        initial_state[11] = 0;
        initial_state[12] = 0;
    }else if (defaultpuzzle == 2){
        std::cout << "Enter your puzzle, use a zero to represent the blank space." << std::endl;
        std::cout << "Enter the bottom row (ten numbers, use space to seperate numbers): " << std::endl;
        cin >> initial_state[0] >> initial_state[1] >> initial_state[2] >> initial_state[3] >> 
        initial_state[4] >> initial_state[5] >> initial_state[6] >> initial_state[7] >> 
        initial_state[8] >> initial_state[9];
        std::cout << "Enter the recess values: " << std::endl;
        cin >> initial_state[10] >>initial_state[11] >>initial_state[12];

    }
 
    // The goal state for nine man in a trench is:
    vector<int> goal_state;
    goal_state.push_back(1);
    goal_state.push_back(2);
    goal_state.push_back(3);
    goal_state.push_back(4);
    goal_state.push_back(5);
    goal_state.push_back(6);
    goal_state.push_back(7);
    goal_state.push_back(8);
    goal_state.push_back(9);
    goal_state.push_back(0);
    goal_state.push_back(0);
    goal_state.push_back(0);
    goal_state.push_back(0);

    int methodChoice;
    cout << "Enter your choice of algorithm" << endl;
    cout << "1) Uniform Cost Search" << endl;
    cout << "2) A* with the Manhattan Distance heuristic." << endl;
    cin >> methodChoice;

    // Create a problem instance
    Problem puzzle_problem(initial_state, goal_state);

    if (methodChoice == 1)
    {
        // Perform Uniform Cost Search
        uniform_cost_search(puzzle_problem);
    }else if (methodChoice == 2)
    {
        manhattan_heuristic_search(puzzle_problem);
    }

    return 0;
}

