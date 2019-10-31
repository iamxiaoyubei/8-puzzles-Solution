#include <iostream>
#include "puzzleState.cpp"
#include <string>
#include <stack>
#include <climits>
#include <queue>
using namespace std;

bool isVisited[1000000000];

struct LessThanByManhattan
{
    bool operator()(const State& lhs, const State& rhs) const {
        return lhs.getManhattanDistance() > rhs.getManhattanDistance();
    }
};

struct LessThanByManhattanAndCostG
{
    bool operator()(const State& lhs, const State& rhs) const {
        return (lhs.getManhattanDistance()+lhs.getCostG()) > (rhs.getManhattanDistance()+rhs.getCostG());
    }
};

struct LessThanByMisplacedAndCostG
{
    bool operator()(const State& lhs, const State& rhs) const {
        return (lhs.getMisplacedDistance()+lhs.getCostG()) > (rhs.getMisplacedDistance()+rhs.getCostG());
    }
};

class Game {
    public:
        Game(State startState, State goalState) {
            this->startState = startState;
            this->goalState = goalState;
        }
        
        int IDS() {
            cout << "Using Iterative Deepening Search(IDS) to Solve..." << endl;
            this->clearRoutes();
            for (int depth = 0; depth <= INT_MAX; depth++) {
                // // debug
                // cout << "[Depth Limited Search] -- " << depth << endl;
                // // debug
                memset(isVisited, 0, sizeof(isVisited));
                int result = this->depthLimitedSearch(depth, this->startState);
                if (result == 1) {
                    cout << "IDS found the goal state in depth [" << depth << "]!!!" << endl;
                    int totalSteps = this->showRoutes();
                    this->showStateRoutes();
                    return totalSteps;
                }
            }
            return -1;
        }

        int GreedySearch() {
            cout << "Using Greedy Search to Solve..." << endl;
            this->clearRoutes();
            // intialize start state
            State currentState = State(this->startState);
            currentState.computeManhattanDistance();
            currentState.clearRoutes();
            // intialize queue
            priority_queue<State, vector<State>, LessThanByManhattan> priorStateQueue;
            priorStateQueue.push(currentState);
            memset(isVisited, 0, sizeof(isVisited));
            // start greedy search
            while (!priorStateQueue.empty()) {
                currentState = priorStateQueue.top();
                isVisited[currentState.getUniqueId()] = true;
                // // debug
                // cout << "Prior State dis:" << currentState.getManhattanDistance() << endl;
                // currentState.show();
                // // debug
                if (currentState.isGoalState()) {
                    cout << "Greedy Search found the goal state!!!" << endl;
                    int totalSteps = this->showRoutesAndStateRoutes(currentState.getRoutes());
                    return totalSteps;
                } else {
                    priorStateQueue.pop();
                    for (int i = 0; i < 4; i++) {
                        if (currentState.canGoNext(i)) {
                            State nextState = State(currentState);
                            nextState.goNext(i);
                            if (!isVisited[nextState.getUniqueId()]) {
                                nextState.computeManhattanDistance();
                                nextState.setRoutes(currentState.getRoutes(), i);
                                priorStateQueue.push(nextState);
                            }
                        }
                    }
                }
            }
            return -1;
        }

        int AStarSearchByManhattanDistance() {
            cout << "Using A* Search with Manhattan Distance to Solve..." << endl;
            this->clearRoutes();
            // intialize start state
            State currentState = State(this->startState);
            currentState.computeManhattanDistance();
            currentState.setCostG(0);
            currentState.clearRoutes();
            // intialize queue
            priority_queue<State, vector<State>, LessThanByManhattanAndCostG> priorStateQueue;
            priorStateQueue.push(currentState);
            memset(isVisited, 0, sizeof(isVisited));
            // start A* search
            while (!priorStateQueue.empty()) {
                currentState = priorStateQueue.top();
                isVisited[currentState.getUniqueId()] = true;
                if (currentState.isGoalState()) {
                    cout << "A* Search with Manhattan Distance found the goal state!!!" << endl;
                    int totalSteps = this->showRoutesAndStateRoutes(currentState.getRoutes());
                    return totalSteps;
                } else {
                    priorStateQueue.pop();
                    for (int i = 0; i < 4; i++) {
                        if (currentState.canGoNext(i)) {
                            State nextState = State(currentState);
                            nextState.goNext(i);
                            if (!isVisited[nextState.getUniqueId()]) {
                                nextState.computeManhattanDistance();
                                nextState.setCostG(currentState.getCostG()+1);
                                nextState.setRoutes(currentState.getRoutes(), i);
                                priorStateQueue.push(nextState);
                            }
                        }
                    }
                }
            }
            return -1;
        }

        int AStarSearchByMisplacedDistance() {
            cout << "Using A* Search with Misplaced Distance to Solve..." << endl;
            this->clearRoutes();
            // intialize start state
            State currentState = State(this->startState);
            currentState.computeMisplacedDistance();
            currentState.setCostG(0);
            currentState.clearRoutes();
            // intialize queue
            priority_queue<State, vector<State>, LessThanByMisplacedAndCostG> priorStateQueue;
            priorStateQueue.push(currentState);
            memset(isVisited, 0, sizeof(isVisited));
            // start A* search
            while (!priorStateQueue.empty()) {
                currentState = priorStateQueue.top();
                isVisited[currentState.getUniqueId()] = true;
                if (currentState.isGoalState()) {
                    cout << "A* Search with Misplaced Distance found the goal state!!!" << endl;
                    int totalSteps = this->showRoutesAndStateRoutes(currentState.getRoutes());
                    return totalSteps;
                } else {
                    priorStateQueue.pop();
                    for (int i = 0; i < 4; i++) {
                        if (currentState.canGoNext(i)) {
                            State nextState = State(currentState);
                            nextState.goNext(i);
                            if (!isVisited[nextState.getUniqueId()]) {
                                nextState.computeMisplacedDistance();
                                nextState.setCostG(currentState.getCostG()+1);
                                nextState.setRoutes(currentState.getRoutes(), i);
                                priorStateQueue.push(nextState);
                            }
                        }
                    }
                }
            }
            return -1;
        }
        
    private:
        State startState, goalState;
        stack<int> routes; // saves the actions routes from startState to goalState
        stack<State> stateRoutes; // saves the states routes from startState to goalState
        void clearRoutes() {
            while (!routes.empty()) routes.pop();
            while (!stateRoutes.empty()) stateRoutes.pop();
        }

        int showRoutesAndStateRoutes(queue<int> outputRoutes) {
            State currentState = this->startState;
            queue<State> outputStateRoutes;
            outputStateRoutes.push(currentState);
            int totalSteps = 0;
            // Last In First Out
            while (!outputRoutes.empty()) {
                currentState.goNext(outputRoutes.front());
                outputStateRoutes.push(currentState);
                if (outputRoutes.front() == 0) {
                    cout << "Up";
                } else if (outputRoutes.front() == 1) {
                    cout << "Left";
                } else if (outputRoutes.front() == 2) {
                    cout << "Down";
                } else if (outputRoutes.front() == 3) {
                    cout << "Right";
                }
                cout << " ";
                outputRoutes.pop();
                totalSteps++;
            }
            cout << endl;

            while (!outputStateRoutes.empty()) {
                outputStateRoutes.front().show();
                outputStateRoutes.pop();
            }
            return totalSteps;
        }

        int showRoutes() {
            // Last In First Out
            int totalSteps = 0;
            while (!routes.empty()) {
                if (routes.top() == 0) {
                    cout << "Up";
                } else if (routes.top() == 1) {
                    cout << "Left";
                } else if (routes.top() == 2) {
                    cout << "Down";
                } else if (routes.top() == 3) {
                    cout << "Right";
                }
                cout << " ";
                routes.pop();
                totalSteps++;
            }
            cout << endl;
            return totalSteps;
        }

        void showStateRoutes() {
            // Last In First Out
            while (!stateRoutes.empty()) {
                stateRoutes.top().show();
                stateRoutes.pop();
            }
        }

        int depthLimitedSearch(int depth, State currentState) {
            if (currentState.isGoalState()) {
                this->stateRoutes.push(currentState);
                return 1;
            } else if (0 == depth) {
                return 0;
            } else {
                isVisited[currentState.getUniqueId()] = true;
            }
            for (int i = 0; i < 4; i++) {
                if (currentState.canGoNext(i)) {
                    State nextState = State(currentState);
                    nextState.goNext(i);
                    if (!isVisited[nextState.getUniqueId()]) {
                        int result = depthLimitedSearch(depth-1, nextState);
                        if (result == 1) {
                            this->routes.push(i);
                            this->stateRoutes.push(currentState);
                            return 1;
                        }
                    }
                }
            }
            return 0;
        }
};