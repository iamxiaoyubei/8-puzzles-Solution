#include <iostream>
#include "puzzleGame.cpp"
#include <ctime>
using namespace std;
int main() {
    // Intial goal state
    State goalState = State(-1);
    // Input total times to play 8-puzzle game
    int times;
    cout << "Please input times for 8-puzzle Game(computing avg running times):" << endl;
    cin >> times;
    // Input shuffle times for Intial State
    int shuffleTimes;
    cout << "Please input shuffle times for initial state:" << endl;
    cin >> shuffleTimes;
    // Intial time of each algorithm
    clock_t timeIDS = 0;
    clock_t timeGreedySearch = 0;
    clock_t timeAStarSearchByManhattanDistance = 0;
    clock_t timeAStarSearchByMisplacedDistance = 0;
    int count = times;
    // Intialize steps of each algorithm
    int stepIDS = 0;
    int stepGreedySearch = 0;
    int stepAStarSearchByManhattanDistance = 0;
    int stepAStarSearchByMisplacedDistance = 0; 
    // Intial Start State and Running each algorithm with Recording times
    while (count--) {
        cout << "You are in the " << times-count << "th 8-puzzle Game..." << endl;
        State startState = State(shuffleTimes);
        Game game = Game(startState, goalState);
        clock_t startTimer = clock();
        stepIDS += game.IDS();
        clock_t IDSTimer = clock();
        stepGreedySearch += game.GreedySearch();
        clock_t GreedyTimer = clock();
        stepAStarSearchByManhattanDistance += game.AStarSearchByManhattanDistance();
        clock_t AStarTimerBy1 = clock();
        stepAStarSearchByMisplacedDistance += game.AStarSearchByMisplacedDistance();
        clock_t AStarTimerBy2 = clock();
        timeIDS += (IDSTimer - startTimer);
        timeGreedySearch += (GreedyTimer - IDSTimer);
        timeAStarSearchByManhattanDistance += (AStarTimerBy1 - GreedyTimer);
        timeAStarSearchByMisplacedDistance += (AStarTimerBy2 - AStarTimerBy1);
    }
    // Output Times of each algorithm
    cout << "Avg Time for IDS:" << timeIDS / times / (double) CLOCKS_PER_SEC << endl;
    cout << "Avg Time for GreedySearch:" << timeGreedySearch / times / (double) CLOCKS_PER_SEC << endl;
    cout << "Avg Time for AStarSearchByManhattanDistance:" << timeAStarSearchByManhattanDistance / times / (double) CLOCKS_PER_SEC << endl;
    cout << "Avg Time for AStarSearchByMisplacedDistance:" << timeAStarSearchByMisplacedDistance / times / (double) CLOCKS_PER_SEC << endl;
    // Output Avg Steps of each algorithm
    cout << "Avg Step for IDS:" << stepIDS / (times * 1.0) << endl;
    cout << "Avg Step for GreedySearch:" << stepGreedySearch / (times * 1.0) << endl;
    cout << "Avg Step for AStarSearchByManhattanDistance:" << stepAStarSearchByManhattanDistance / (times * 1.0) << endl;
    cout << "Avg Step for AStarSearchByMisplacedDistance:" << stepAStarSearchByMisplacedDistance / (times * 1.0) << endl;
    
    return 0;
}