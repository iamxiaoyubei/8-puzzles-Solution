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



###Greedy Search



### A* Search



## Experiments



