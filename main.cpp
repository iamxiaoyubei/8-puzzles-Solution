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
    // Intial Start State and Running each algorithm with Recording times
    while (count--) {
        cout << "You are in the " << times-count << "th 8-puzzle Game..." << endl;
        State startState = State(shuffleTimes);
        Game game = Game(startState, goalState);
        clock_t startTimer = clock();
        game.IDS();
        clock_t IDSTimer = clock();
        game.GreedySearch();
        clock_t GreedyTimer = clock();
        game.AStarSearchByManhattanDistance();
        clock_t AStarTimerBy1 = clock();
        game.AStarSearchByMisplacedDistance();
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
    return 0;
}