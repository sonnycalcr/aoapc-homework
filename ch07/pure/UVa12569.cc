#include <cmath>
#include <cstring>
#include <iostream>
#include <queue>
#include <vector>

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

const int MAXN = 16;

struct Node {
    int from, to;
    Node *next;
};

struct State {
    Node *path;
    int g;
    int len;
    State(int gi = 0, int li = 0, Node *pn = NULL) : g(gi), len(li), path(pn) {}
    inline bool operator[](size_t i) const { return g & (1 << (i + 4)); }
    inline void setRock(size_t i, bool val = true) {
        if (val)
            g |= 1 << (i + 4);
        else
            g &= ~(1 << (i + 4));
    }

    inline int getP() const { return g & 15; }
    inline void setP(int p) { g = ((g >> 4) << 4) | p; }
};

vector<int> G[MAXN];
MemPool<Node> pool;

int n, m, S, T, O[MAXN], VIS[1 << 19];
Node *newNode(Node *next = NULL, int u = -1, int v = -1) {
    Node *p = pool.createNew();
    p->next = next, p->from = u, p->to = v;
    return p;
}

ostream &operator<<(ostream &os, Node *p) {
    if (p == NULL)
        return os;
    os << p->next << p->from + 1 << " " << p->to + 1 << endl;
    return os;
}

void tryMove(const State &s, int from, queue<State> &q) {
    int rp = s.getP();
    for (auto to : G[from]) {
        if ((to == rp) || s[to])
            continue;
        int ng = s.g;
        if (from == rp)
            ng = ((s.g >> 4) << 4) | to;
        else
            ng ^= (1 << (from + 4)), ng ^= (1 << (to + 4));
        if (VIS[ng])
            continue;
        VIS[ng] = 1;

        q.push(State(ng, s.len + 1, newNode(s.path, from, to)));
    }
}

void solve() {

    State s;
    _for(i, 0, m) s.setRock(O[i]);
    s.setP(S);
    queue<State> q;
    q.push(s);
    VIS[s.g] = 1;

    while (!q.empty()) {
        const State &st = q.front();
        int rp = st.getP();
        if (rp == T) {
            cout << st.len << endl << st.path;
            return;
        }
        tryMove(st, rp, q);
        _for(i, 0, n) if (st[i]) tryMove(st, i, q);
        q.pop();
    }
    cout << "-1" << endl;
}

int main() {
    int K = readint();
    for (int t = 1; t <= K; t++) {
        memset(VIS, 0, sizeof(VIS));
        cin >> n >> m >> S >> T;
        --S;
        --T;
        cout << "Case " << t << ": ";
        _for(i, 0, m) O[i] = readint() - 1;
        _for(i, 0, n) G[i].clear();

        _for(i, 0, n - 1) {
            int u = readint() - 1, v = readint() - 1;
            G[u].push_back(v);
            G[v].push_back(u);
        }

        solve();
        pool.dispose();
        cout << endl;
    }

    return 0;
}
