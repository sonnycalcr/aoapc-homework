#include <cmath>
#include <cstring>
#include <iostream>
#include <queue>
#include <vector>
#include <fstream>

#define _for(i, a, b) for (int i = (a); i < (b); ++i)

using namespace std;

int readint() {
    int x;
    cin >> x;
    return x;
}

template <typename T> struct MemPool {
    vector<T *> buf;
    T *createNew() {
        buf.push_back(new T());
        return buf.back();
    }

    void dispose() {
        for (int i = 0; i < buf.size(); i++)
            delete buf[i];
        buf.clear();
    }
};

const int MAXN = 16; // 节点的最大数量是 16

struct Node { // 表示路径的链表节点，to 表示的是当前的节点，from 是上一个节点
    int from, to;
    Node *next;
};

struct State {
    Node *path; // 路径
    int g;      // 状态压缩，前 4 位(当然是从右往左)表示机器人🤖位置的编号，因为节点数 n 的范围是：4 <= n <= 15，后面 16 位的每一位用来表示位置 i 上是否有石头，比如，(2 + 4) 这个 bit 位为 1，就表示编号为 2 的位置上有石头
    int len;    // 路径长度，到了当前状态已经经历的长度
    State(int gi = 0, int li = 0, Node *pn = NULL) : g(gi), len(li), path(pn) {}
    inline bool operator[](size_t i) const { return g & (1 << (i + 4)); } // 位置 i 上是否有石头，前 4 位被机器人占据了，往后的位才能用来表示石头
    inline void setRock(size_t i, bool val = true) {                      // 设置位置 i 上是否有石头
        if (val)
            g |= 1 << (i + 4); // 位或操作来把第(i + 4)位来置 1
        else
            g &= ~(1 << (i + 4)); // 先取反，再用与操作来把第(i + 4)位来置 0
    }
    // 机器人的位置操作
    inline int getP() const { return g & 15; }           // 与 1111(二进制) 进行与操作，取出前 4 位的值，也就是机器人的位置
    inline void setP(int p) { g = ((g >> 4) << 4) | p; } // 设置机器人的位置，这里先右移再左移 4 位清除原有的数据
};

vector<int> G[MAXN]; // 图的邻接矩阵表示，题目中简化成了树(无环图)
MemPool<Node> pool;  // 链表节点分配，用 MemPool 比较好管理内存
// n: 顶点数；m: 障碍物(石头)数量；S: 源，即机器人的起点位置；T: 目标位置
// O: 每一个石头的位置编号
// VIS: 表示某个位置是否已经 visited 过了
int n, m, S, T, O[MAXN], VIS[1 << 19];
Node *newNode(Node *next = NULL, int u = -1, int v = -1) {
    Node *p = pool.createNew();
    p->next = next, p->from = u, p->to = v;
    return p;
}

// 一个递归的输出，因为 tryMove 保证了最终到达终点时的状态中的 path 是递归地往前指的
ostream &operator<<(ostream &os, Node *p) {
    if (p == NULL)
        return os;
    os << p->next << p->from + 1 << " " << p->to + 1 << endl; // 这里要把 from 和 to 分别加 1 来映射成题目中的从 1 开始的序号
    return os;
}

// 尝试移动在点 from 上的物体(机器人或者石头)
void tryMove(const State &s, int from, queue<State> &q) {
    int rp = s.getP(); // 获取当前状态下机器人的位置编号
    for (auto to : G[from]) {
        if ((to == rp) || s[to]) // to 和当前状态的机器人位置重合或者 to 的位置上有机器人
            continue;
        int ng = s.g;
        if (from == rp)
            ng = ((s.g >> 4) << 4) | to; // 把 s.g 的前 4 位置成 to 这个值，也就是移动机器人到 to 的位置
        else
            ng ^= (1 << (from + 4)), ng ^= (1 << (to + 4)); // 把 ng 的 from 对应的 bit 位置 0，然后把 to 对应的 bit 位置 1，也就是把石头从 from 位置移动到 to 这个位置
        if (VIS[ng])
            continue; // 新的状态已经访问过
        VIS[ng] = 1;  // 标记访问
        // 这里构建的新 State 的 newNode 的 next 属性指向的是移动前的 State 的 path Node
        q.push(State(ng, s.len + 1, newNode(s.path, from, to)));
    }
}

// bfs
void solve() {
    // 定义并初始化原始的状态
    State s;
    _for(i, 0, m) s.setRock(O[i]);
    s.setP(S);
    queue<State> q;
    q.push(s);
    VIS[s.g] = 1; // 标记当前机器人和石头的状态已经被访问过
    // bfs 一层一层地遍历
    while (!q.empty()) {
        const State &st = q.front();
        int rp = st.getP();
        if (rp == T) { // 到达目的地，因为是 bfs，所以一旦到达目的地，也就表明是最短路径之一，后面不可能有移动步数更少的结果，最好的情况也是花费和当前一样的步数
            cout << st.len << endl << st.path;
            return;
        }
        tryMove(st, rp, q);                         // 尝试移动机器人
        _for(i, 0, n) if (st[i]) tryMove(st, i, q); // 尝试移动石头
        q.pop();
    }
    cout << "-1" << endl; // 无法到达目的地
}

int main() {
    // 重定向标准输入到文件
    string relativePathToCurrentCppFile = "./data/UVa12569/input1.txt";
    // relativePathToCurrentCppFile = "./data/UVa12569/input2.txt";
    // 因为我们是在根目录下执行编译出来的可执行文件的
    ifstream inputFile("./ch07" + relativePathToCurrentCppFile.substr(1, relativePathToCurrentCppFile.size() - 1));
    if (!inputFile.is_open()) {
        cerr << "Failed to open input data file." << endl;
        return 2;
    }
    streambuf *cinbuf = cin.rdbuf(); // save original buf
    cin.rdbuf(inputFile.rdbuf());

    int K = readint(); // 读取 case 的数量，为了和 case 关键字区分，一般用 Kase 或者 K 来表示 case
    for (int t = 1; t <= K; t++) {
        memset(VIS, 0, sizeof(VIS)); // 初始化置 0
        cin >> n >> m >> S >> T;     // 读入 n m S T
        --S;                         // 程序用到的编号要减 1
        --T;                         // 同上
        cout << "Case " << t << ": ";
        _for(i, 0, m) O[i] = readint() - 1; // 读入石头的编号，编号是从给的数据减 1 而得来
        _for(i, 0, n) G[i].clear();         // 清除之前的数据

        // 构建邻接矩阵
        _for(i, 0, n - 1) {                           // 读入 n - 1 条边
            int u = readint() - 1, v = readint() - 1; // 每一个顶点的编号都要减 1
            G[u].push_back(v);
            G[v].push_back(u);
        }

        solve();
        pool.dispose(); // 释放内存
        cout << endl;
    }

    // 恢复标准输入
    cin.rdbuf(cinbuf);
    return 0;
}
