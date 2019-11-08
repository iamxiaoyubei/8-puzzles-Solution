# 8-puzzles Solution
By Yubei Xiao

## Problem

Given any randomly generated start state and a goal state shown below, implement the IDS, greedy search and A* search algorithms, respectively, to find a sequence of actions that will transform the state from the start state to the goal state.

<img src="/Users/xiaoyubei/Desktop/屏幕快照 2019-11-08 上午10.39.33.png" style="zoom:40%;" />

## Requirements

- When implementing the A* search algorithm, you need to use at least two different heuristic functions;
- Compare the running time of these different searching algorithms, and do some analyses on your obtained results. Note that the reported running time should be averaged on many randomly generate different start states;



## Solutions

为了解决这个问题，我首先使用了面向对象的编程思想构造了两个类：Game和State。State用于存储和改变puzzle棋盘的状态，而Game代表了整个puzzle game，存储了start state, goal state和state routes(路径)，使用了不同的算法去求解从start state到goal state的路径并且将路径展示出来。

在算法上，我使用了Iterative Deepening Search(IDS), Greedy Search With Manhattan Distance, Greedy Search with Misplaced Distance, A* Search with Manhattan Distance以及A* Search with Misplaced Distance共五种算法去求解这个问题。在实现了这些算法之后，我发现在某些start state情况下，Greedy Search出现了死循环的现象，于是我维护了一个isVisited数组进行剪枝，让每个状态在一个path下只能够出现一次，若下一次访问到该state发现已经访问过则不再访问。同时为了公平起见，在各个算法上我都加入了该剪枝。

最后，我调节参数用不同的初始状态实验了很多次，将平均结果和分析展示在了下面Experiments章节中。并且在每一次不同算法中，我都将a sequence of actions that transform the state from the start state to the goal state以及state在路径中的变化都展示了出来。

在下面的小节，我将详细地描述各种算法和一些必要代码。

### Intialization

因为随机生成的start state可能会存在无解的情况，所以为了确保生成的start state有解，我使用了从goal state随机游走的方式生成start state。

用户可以输入*shuffleTimes*用于控制随机游走的次数，之后State的构造函数便接收*shuffleTimes*并调用随机生成start state的private函数*generateStartState*进行初始化并返回*startState*实例。

```c++
void generateStartState(int times) {
    this->generateGoalState();
    while (times--) {
        int random = rand() % 4;
        if (this->canGoNext(random)) {
            this->goNext(random);
        }
    }
}
```

如上面的代码所示，*generateStartState*首先调用private函数*generateGoalState*生成goal state的puzzle棋盘，然后随机进行*times*次游走，每一次都从四个方向中选择一个方向，如果该方向可以走则*goNext*改变当前state的puzzle棋盘。



### Iterative Deepening Search(IDS)

IDS算法的伪代码和思路已在课件中给出：

<img src="/Users/xiaoyubei/Desktop/屏幕快照 2019-11-08 下午3.01.33.png" style="zoom:100%;" />

它实际上可以认为是深搜(depth-first search)和宽搜(breadth-first search)的结合版本。在外层循环$depth=[0,+\infty)$从而给定了每次搜索的深度限制，在内部使用类似于深搜的方法*depthLimitedSearch*进行限制深度的搜索。

因此它结合了深搜和宽搜两者的优势并且摒弃了两者的劣势，比如它通过每次搜索的深度限制去除了深搜在无限深度情况下陷入死循环的问题(fail in inﬁnite-depth spaces)，同时又通过每次*depthLimitedSearch*使用深搜的策略从而无需将上面每一层的node保存在memory中，因此相对于宽搜节省了十分多的内存空间(memory space)等等。下图便展示了Iterative Deepening Search相对于深搜宽搜的优势：

<img src="/Users/xiaoyubei/Desktop/屏幕快照 2019-11-08 下午4.02.07.png" style="zoom:100%;" />

![](/Users/xiaoyubei/Desktop/屏幕快照 2019-11-08 下午4.04.30.png)

因此，按照IDS的思路，代码实现如下：

```c++
int IDS(clock_t &runningTime) {
    cout << "Using Iterative Deepening Search(IDS) to Solve..." << endl;
    this->clearRoutes();
    clock_t startTimer = clock();
    for (int depth = 0; depth <= INT_MAX; depth++) {
        memset(isVisited, 0, sizeof(isVisited));
        int result = this->depthLimitedSearch(depth, this->startState);
        if (result == 1) {
            clock_t endTimer = clock();
            runningTime += (endTimer - startTimer);
            cout << "IDS found the goal state in depth [" << depth << "]!!!" << endl;
            int totalSteps = this->showRoutes();
            this->showStateRoutes();
            return totalSteps;
        }
    }
    return -1;
}
```

*main*函数可以传入*runningTime*变量的引用调用*IDS*函数，*IDS*函数内部进行Iterative Deepening Search算法搜索从*startState*到*goalState*的路径，在搜索到结果后，将算法的运行时间传给*runningTime*引用，同时使用*showRoutes*和*showStateRoutes*展示结果的action sequences以及每一步state的变化，最后将该结果的*totalSteps*传回。同时IDS内部调用的*depthLimitedSearch*函数代码如下：

```c++
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
```

*depthLimitedSearch*使用了递归的方式实现了限制深度的类似深搜的搜索。同时使用了*isVisited*数组记录state是否已经访问过，从而避免在搜索中重复访问相同的state(avoid repeated states along path)。



### Heuristic functions

上面的IDS算法是uninformed search: uninformed strategies use only the information available in the problem definition。而下面的两个算法greedy search和A* search是informed search: use problem-speciﬁc knowledge to decide the order of node selection。因此对于每一个node，我们需要提前定义一个heuristic function, which is the estimate of “desirability”，也就是这个函数估计了一个到goal state的期望，函数值越高我们认为离终点的可能性越高。

下面采用Misplaced distance和Manhattan distance两种距离作为heuristic function：

<img src="./pic/heuristic_function.png" style="zoom:50%;" />

如上图所示给出了Misplaced distance和Manhattan distance的计算，Misplaced distance即start state相对于goal state的puzzle棋盘数字放的位置不同的个数(除了空白块以外)，而Manhattan distance即start state中每一个数字放置的位置相对于goal state相应数字放置位置的曼哈顿距离之和，即：
$$
\sum_{i=1}^{8} || startStatePosition_i - goalStatePosition_i ||
$$
其中$startStatePosition_i$指的是数字$i$在start state puzzle上的位置，是一个矢量$(x, y)$，$goalStatePosition_i$同理。

则计算Misplaced distance的代码如下：

```c++
int computeMisplacedDistance() {
    this->misplacedDistance = 0;
    for (int thisIndex = 0; thisIndex <= 8; thisIndex++) {
        if (this->puzzles[thisIndex] != 0 && this->puzzles[thisIndex] != (thisIndex+1)) {
            this->misplacedDistance++;
        }
    }
    return this->misplacedDistance;
}
```

计算Manhattan distance的代码如下：

```c++
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
```



###Greedy Search

贪心搜索的思想很简单，即expands the node that appears to be closest to goal，它的evaluation function即上一小节描述的heuristic function，然后我们每次搜索选择距离goal state最小的node去expand即可。

使用Misplaced distance作为heuristic function的Greedy Search代码如下，通过一个priority_queue优先队列来维护当前路径里面的还没有expand的节点，优先队列会以Misplaced distance进行排序，Misplaced distance最小的会被优先访问和被expanded（即将它的下一步state加入优先队列），直到找到goal state或者队列为空。

```c++
int GreedySearchByMisplacedDistance(clock_t &runningTime) {
    cout << "Using Greedy Search with Misplaced Distance to Solve..." << endl;
    this->clearRoutes();
    // intialize start state
    State currentState = State(this->startState);
    currentState.computeMisplacedDistance();
    currentState.clearRoutes();
    // intialize queue
    priority_queue<State, vector<State>, LessThanByMisplaced> priorStateQueue;
    priorStateQueue.push(currentState);
    memset(isVisited, 0, sizeof(isVisited));
    clock_t startTimer = clock();
    // start greedy search
    while (!priorStateQueue.empty()) {
        currentState = priorStateQueue.top();
        isVisited[currentState.getUniqueId()] = true;
        if (currentState.isGoalState()) {
            clock_t endTimer = clock();
            runningTime += (endTimer - startTimer);
            cout << "Greedy Search with Misplaced Distance found the goal state!!!" << endl;
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
                        nextState.setRoutes(currentState.getRoutes(), i);
                        priorStateQueue.push(nextState);
                    }
                }
            }
        }
    }
    return -1;
}
```

在优先队列中通过自定义下面的比较函数，从而让队列以Misplaced distance从小到大进行排序：

```c++
struct LessThanByMisplaced {
    bool operator()(const State& lhs, const State& rhs) const {
        return lhs.getMisplacedDistance() > rhs.getMisplacedDistance();
    }
};
```

使用Manhattan distance作为heuristic function的同理，只是需要修改比较函数，让队列以Manhattan distance从小到大进行排序。



### A* Search

A\*搜索和贪心搜索很相似，也是选择距离goal state最小的node去expand，但是它的evaluation function不仅仅是heuristic function。如下图所示，除了用来估计到终点的距离的函数$h(n)$，还有到目前为止已经走过的距离$g(n)$，这两个函数组成了A\*算法的evaluation function。

<img src="./pic/astar.png" style="zoom:50%;" />

使用Manhattan distance作为heuristic function的A\* Search代码如下，和Greedy Search算法十分类似，用一个优先队列来存储目前还没有expand的节点，只不过优先队列的比较函数不同。因为A\* search算法相对于Greedy search算法加入了目前已经走过的距离作为evaluation function的衡量标准之一，所以在比较函数*LessThanByManhattanAndCostG*中还加入*CostG*进行比较，其中*CostG*就是在path中走到当前state需要的步数，*Manhattan distance+CostG*最小的state会优先被访问和expanded。

```c++
struct LessThanByManhattanAndCostG {
    bool operator()(const State& lhs, const State& rhs) const {
        return (lhs.getManhattanDistance()+lhs.getCostG()) > (rhs.getManhattanDistance()+rhs.getCostG());
    }
};

int AStarSearchByManhattanDistance(clock_t &runningTime) {
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
    clock_t startTimer = clock();
    // start A* search
    while (!priorStateQueue.empty()) {
        currentState = priorStateQueue.top();
        isVisited[currentState.getUniqueId()] = true;
        if (currentState.isGoalState()) {
            clock_t endTimer = clock();
            runningTime += (endTimer - startTimer);
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
```

使用Misplaced distance+CostG作为evaluation function的同理，只需要修改比较函数，让队列以Misplaced distance+CostG从小到大进行排序。



## Experiments

### Results

#### times: 100	shuffleTimes: 25

|             Method              | Avg Running Time | Avg Steps | Avg Time(Running+ShowResults) |
| :-----------------------------: | :--------------: | :-------: | :---------------------------: |
|               IDS               |                  |           |                               |
| GreedySearchByManhattanDistance |                  |           |                               |
| GreedySearchByMisplacedDistance |                  |           |                               |
| AStarSearchByManhattanDistance  |                  |           |                               |
| AStarSearchByMisplacedDistance  |                  |           |                               |



#### times: 100	shuffleTimes: 50

|             Method              | Avg Running Time | Avg Steps | Avg Time(Running+ShowResults) |
| :-----------------------------: | :--------------: | :-------: | :---------------------------: |
|               IDS               |                  |           |                               |
| GreedySearchByManhattanDistance |                  |           |                               |
| GreedySearchByMisplacedDistance |                  |           |                               |
| AStarSearchByManhattanDistance  |                  |           |                               |
| AStarSearchByMisplacedDistance  |                  |           |                               |



#### times: 50	shuffleTimes: 50

|             Method              | Avg Running Time | Avg Steps | Avg Time(Running+ShowResults) |
| :-----------------------------: | :--------------: | :-------: | :---------------------------: |
|               IDS               |                  |           |                               |
| GreedySearchByManhattanDistance |                  |           |                               |
| GreedySearchByMisplacedDistance |                  |           |                               |
| AStarSearchByManhattanDistance  |                  |           |                               |
| AStarSearchByMisplacedDistance  |                  |           |                               |



### Analysis

