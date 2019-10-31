#include <iostream>
#include <string>
#include <limits>
using namespace std;

int puzzles[9]={8,7,6,5,4,3,2,1,0};
// int MAX = 1000000000;
bool isVisited[1000000000] = { 0 };

int getHashValue() {
    string puzzleStr = "";
    for (int i = 0; i <= 8; i++) {
        puzzleStr += to_string(puzzles[i]);
        cout << "The " << i << "th" << puzzleStr << endl;
    }
    int id = stoi(puzzleStr);
    return id;
}

int main(int argc, char *argv[])
{

    int result = getHashValue();
    cout << isVisited[result] << endl;
    isVisited[result] = true;
    cout << result  << endl;
    cout << isVisited[result] << endl;

}