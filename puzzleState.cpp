#include <iostream>
#include <ctime>
#include <queue>
#include <string>
using namespace std;
class State {
    public:
        State() {
            this->manhattanDistance = -1;
            this->misplacedDistance = -1;
            this->costG = 0; 
        }

        State(int initial) {
            if (initial == -1) {
                this->generateGoalState();
            } else {
                // initial means the times of shuffle
                this->generateStartState(initial);
            }
            this->manhattanDistance = -1;
            this->misplacedDistance = -1;
            this->costG = 0; 
        }

        int getUniqueId() {
            string puzzleStr = "";
            for (int i = 0; i <= 8; i++) {
                puzzleStr += to_string(this->puzzles[i]);
            }
            int id = stoi(puzzleStr);
            return id;
        }

        queue<int> getRoutes() {
            return this->routes;
        }

        void clearRoutes() {
            while (!this->routes.empty()) this->routes.pop();
        }

        void setRoutes(queue<int> beforeRoutes, int direction) {
            beforeRoutes.push(direction);
            this->routes = beforeRoutes;
        }

        int getManhattanDistance() const {
            return this->manhattanDistance;
        }

        /* Manhattan Distance Between this State and Goal State
            Goal State Value: 1 2 3 4 5 6 7 8 0
            Goal State Index: 0 1 2 3 4 5 6 7 8
            This State Value: 7 2 4 5 0 6 8 3 1 (for example)
            This State Index: 0 1 2 3 4 5 6 7 8
            This State Value 1 needs on Expected Goal State Value Index 0
            This Position -> row: this_state_index(8)/3 = 2 col: this_state_index(8)%3 = 2
            Expected Index(0) = this_state_value(1) - 1 = 0
            Expected Position -> row: expected_state_index(0)/3 = 0 col: expected_state_index(0)%3 = 0
            Manhattan Distance = |This Position - Expected Position| = 2+2 = 4
        */
        int computeManhattanDistance() {
            this->manhattanDistance = 0;
            for (int thisIndex = 0; thisIndex <= 8; thisIndex++) {
                if (this->puzzles[thisIndex] != 0) {
                    int thisPositionRow = thisIndex / 3;
                    int thisPositionCol = thisIndex % 3;
                    int expectedIndex = this->puzzles[thisIndex] - 1;
                    int expectedPositionRow = expectedIndex / 3;
                    int expectedPositionCol = expectedIndex % 3;
                    this->manhattanDistance += abs(expectedPositionRow - thisPositionRow) + abs(expectedPositionCol - thisPositionCol);
                }
            }
            return this->manhattanDistance;
        }

        int getCostG() const {
            return this->costG;
        }

        void setCostG(int cost) {
            this->costG = cost;
        }

        int getMisplacedDistance() const {
            return this->misplacedDistance;
        }

        int computeMisplacedDistance() {
            this->misplacedDistance = 0;
            for (int thisIndex = 0; thisIndex <= 8; thisIndex++) {
                if (this->puzzles[thisIndex] != 0 && this->puzzles[thisIndex] != (thisIndex+1)) {
                    this->misplacedDistance++;
                }
            }
            return this->misplacedDistance;
        }

        void show() {
            cout << "Puzzles:" << endl;
            for (int i = 0; i < 3; i++) {
                cout << this->puzzles[i] << " ";
            }
            cout << endl;
            for (int i = 3; i < 6; i++) {
                cout << this->puzzles[i] << " ";
            }
            cout << endl;
            for (int i = 6; i < 9; i++) {
                cout << this->puzzles[i] << " ";
            }
            cout << endl;
        }

        bool isGoalState() {
            for (int i = 0; i <= 8; i++) {
                if (this->puzzles[i] != (i + 1) % 9) {
                    return false;
                }
            }
            return true;
        }

        void changePuzzles(int anotherIndex) {
            this->puzzles[this->blankBlockIndex] = this->puzzles[anotherIndex];
            this->puzzles[anotherIndex] = 0;
            this->blankBlockIndex = anotherIndex;
        }

        // 0->up 1->left 2->down 3->right
        bool canGoNext(int direction) {
            if (direction == 0 && (this->blankBlockIndex == 0 || this->blankBlockIndex == 1 || this->blankBlockIndex == 2)) {
                return false;
            }
            if (direction == 1 && (this->blankBlockIndex == 0 || this->blankBlockIndex == 3 || this->blankBlockIndex == 6)) {
                return false;
            }
            if (direction == 2 && (this->blankBlockIndex == 6 || this->blankBlockIndex == 7 || this->blankBlockIndex == 8)) {
                return false;
            }
            if (direction == 3 && (this->blankBlockIndex == 2 || this->blankBlockIndex == 5 || this->blankBlockIndex == 8)) {
                return false;
            }
            return true;
        }

        // 0->up 1->left 2->down 3->right
        void goNext(int direction) {
            if (direction == 0) {
                this->changePuzzles(this->blankBlockIndex-3);
            } else if (direction == 1) {
                this->changePuzzles(this->blankBlockIndex-1);
            } else if (direction == 2) {
                this->changePuzzles(this->blankBlockIndex+3);
            } else if (direction == 3) {
                this->changePuzzles(this->blankBlockIndex+1);
            }
            return;
        } 
    private:
        int puzzles[9];
        int blankBlockIndex;
        int manhattanDistance;
        int misplacedDistance;
        queue<int> routes;
        int costG;
        /* generate Goal State
                1 2 3
                4 5 6
                7 8 0
        */
        void generateGoalState() {
            for (int i = 0; i <= 8; i++) {
                this->puzzles[i] = (i + 1) % 9;
            }
            this->blankBlockIndex = 8;
        }

        void generateStartState(int times) {
            this->generateGoalState();
            while (times--) {
                int random = rand() % 4;
                if (this->canGoNext(random)) {
                    this->goNext(random);
                }
            }
        }
};