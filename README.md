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

在算法上，我使用了Iterative Deepening Search(IDS), Greedy Search With Manhattan Distance, Greedy Search with Misplaced Distance, A* Search with Manhattan Distance以及A* Search with Misplaced Distance共五种算法去求解这个问题。在实现了这些算法之后，我发现在某些start state情况下，Greedy Search出现了死循环的现象，于是我维护了一个isVisited数组进行剪枝，让每个状态在一个path下只能够出现一次，若下一次访问到该state发现已经访问过则不再访问。同时为了公平起见，在每个算法上我都加入了该剪枝。

最后，我调节参数用不同的初始状态实验了很多次，将平均结果和分析展示在了下面Experiments章节中。并且在每一次不同算法中，我都将从start state走到goal state的action路径以及state在路径中的变化都展示了出来。

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
    isVisited[currentState.getUniqueId()] = false;
    return 0;
}
```

*depthLimitedSearch*使用了递归的方式实现了限制深度的类似深搜的搜索。同时使用了*isVisited*数组记录state是否已经访问过，从而避免在搜索中重复访问相同的state(avoid repeated states along path)。



### Heuristic functions

上面的IDS算法是uninformed search: 

> uninformed strategies use only the information available in the problem definition.

而下面的两个算法greedy search和A* search是informed search: 

> informed search use problem-speciﬁc knowledge to decide the order of node selection.

因此对于每一个node，我们需要提前定义一个heuristic function用来估计到终点的距离，然后在搜索中，我们使用heuristic function的函数值作为node selection的一个指导。

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

贪心搜索的思想很简单，即"expands the node that appears to be closest to goal"，它的evaluation function即上一小节描述的heuristic function，然后我们每次搜索选择距离goal state最小的node去expand即可。

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

times为总共进行的puzzle game的次数，shuffleTimes为初始化start state时的随机游走次数，Avg Running Time为相应的搜索算法的平均运行时间，Avg Steps为相应的搜索算法搜索出来的平均路径步数。

#### times: 100	shuffleTimes: 25

|               Method               | Avg Running Time | Avg Steps |
| :--------------------------------: | :--------------: | :-------: |
|                IDS                 |     0.289974     |  **5.8**  |
|  GreedySearchByManhattanDistance   |     0.000411     |   7.42    |
|  GreedySearchByMisplacedDistance   |     0.00155      |   11.94   |
| **AStarSearchByManhattanDistance** |   **0.000209**   |  **5.8**  |
|   AStarSearchByMisplacedDistance   |     0.000352     |  **5.8**  |

实验记录截图包括每一次的具体路径和state变化见[Appendix Result1](#Result1)。

#### times: 100	shuffleTimes: 50

|               Method               | Avg Running Time | Avg Steps |
| :--------------------------------: | :--------------: | :-------: |
|                IDS                 |     0.449816     | **9.09**  |
|  GreedySearchByManhattanDistance   |     0.001386     |   16.19   |
|  GreedySearchByMisplacedDistance   |     0.007749     |   28.49   |
| **AStarSearchByManhattanDistance** |   **0.00058**    | **9.09**  |
|   AStarSearchByMisplacedDistance   |     0.002429     | **9.09**  |

实验记录截图包括每一次的具体路径和state变化见[Appendix Result2](#Result2)。



### Analysis

从实验结果可以看出以下几点：

- 在IDS, Greedy Search和A\* Search三种算法中，A\* Search算法运行时间最快，Greedy Search次之，而IDS算法最慢。
- 在IDS, Greedy Search和A\* Search三种算法中，IDS和A\*搜索出来的路径长度相同，都是最短的，而Greedy Search路径最长。
- 对于Greedy Search和A* Search，Manhattan Distance作为heuristics function均比Misplaced Distance时间更快，搜索出来路径更短。

以上实验结果的理论支撑分析如下（以下参数均在上面算法分析时有给出符号表示）：

- IDS平均时间为$(d+1)b^0+db^1+(d-1)b^2+...+b^d$，在每一次puzzle game中，IDS都相当于进行了d次的深搜，每一次都会搜索到$limited\    depth$结束，因此相对来说十分耗时。而Greedy Search用启发函数引导搜索，只会从往启发式函数值最小的方向走，直到搜索到终点结束，因此Greedy Search相对于IDS极大地减少了许多重复的搜索时间。最后A\*算法在Greedy Search的evaluation function上加上了真实走过的距离$g(n)$，从而相对矫正了启发式函数$h(n)$所带来的误差，相对于Greedy Search进一步更高效地找到终点。因此平均速度为A* Search>Greedy Search>IDS。

- IDS算法是最优解算法，因为它是从$depth=0$开始一步步增加$depth$进行类深搜，所以一定能够找到路径最短的解。A\* Search有理论证明只要找到一个好的启发函数就能够做到最优解。而Greedy Search不一定能够找到最优解。所以对于解的路径长度，IDS = A* < Greedy Search。

- 在两个启发函数中，Manhattan Distance比Misplaced Distance更好，因为$h_{manhatten}(state) \ge h_{misplaced}(state)\  for\ all\ state$，所以有Manhattan Distance dominates Misplaced Distance，因此Manhattan Distance是对于搜索算法更好的一个heuristics function，从而可以更大幅度地减小search cost。

同时可以看到shuffleTimes越大的实验，各个算法的平均时间和步数都增加了，说明随机游走的方案对于初始化start state是有效的，随机游走的次数越大，start state平均距离goal state越远，游戏平均难度越高。



## Thought

通过两天从零开始的纯手撸代码，拾回了当初写C++的热情，也对这几种算法有了更近一步的了解。之后在写这份报告时，结合着PPT的基础知识以及网上的相关资料，我对于这些算法的原理分析（completeness, time, space...）也有了更深入的认识。总之，这一次的assignment让我有了很大的收获，纸上得来终觉浅，绝知此事要躬行，以后再接再厉！



## Appendix

由于结果是进行了100次后取平均值，因此结果很多，以下仅仅从100次中各自截取一次puzzle game的结果，每个算法跑出来的action sequence和state sequence都可以在截图中看到，以及最后一张是平均结果的截图（Avg Running Time为相应的搜索算法的平均运行时间，Avg Steps为相应的搜索算法搜索出来的平均路径步数，Avg Time为相应的搜索算法的平均运行加上展示出结果的时间）。

可以很明显地看出来IDS和A*跑出来的路径长度一样（均为最短路径）；而Greedy Search有时会找不到最短的路径，不过用Manhattan Distance的Greedy Search跑出来的路径常常会比用Misplaced Distance的Greedy Search路径要短。

### Result1

<img src="./pic/100_25_1.png" style="zoom:50%;" />

<img src="./pic/100_25_2.png" style="zoom:50%;" />

<img src="./pic/100_25_3.png" style="zoom:50%;" />

<img src="./pic/100_25_4.png" style="zoom:50%;" />

<img src="./pic/100_25_5.png" style="zoom:50%;" />

<img src="./pic/100_25_6.png" style="zoom:50%;" />

<img src="./pic/100_25_7.png" style="zoom:50%;" />

<img src="./pic/100_25_8.png" style="zoom:50%;" />

<img src="./pic/100_25_9.png" style="zoom:50%;" />

<img src="./pic/100_25_0.png" style="zoom:50%;" />

### Result2

<img src="./pic/100_50_1.png" style="zoom:50%;" />

<img src="./pic/100_50_2.png" style="zoom:50%;" />

<img src="./pic/100_50_3.png" style="zoom:50%;" />

<img src="./pic/100_50_4.png" style="zoom:50%;" />

<img src="./pic/100_50_5.png" style="zoom:50%;" />

<img src="./pic/100_50_6.png" style="zoom:50%;" />

<img src="./pic/100_50_7.png" style="zoom:50%;" />

<img src="./pic/100_50_8.png" style="zoom:50%;" />

<img src="./pic/100_50_9.png" style="zoom:50%;" />

<img src="./pic/100_50_0.png" style="zoom:50%;" />