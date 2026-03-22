#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

// ==============================
// 資料結構定義
// ==============================
struct Edge {
    int u, v, w;
};

// 並查集 Disjoint Set Union
class DSU {
private:
    vector<int> parent, rankv;

public:
    DSU(int n) {
        parent.resize(n + 1);
        rankv.resize(n + 1, 0);
        for (int i = 1; i <= n; i++) {
            parent[i] = i;
        }
    }

    int find(int x) {
        if (parent[x] == x) return x;
        return parent[x] = find(parent[x]);
        return x; 
    }

    bool unite(int a, int b) {
       int rootA = find(a);
        int rootB = find(b);
        if (rootA != rootB) {
            if (rankv[rootA] < rankv[rootB]) parent[rootA] = rootB;
            else if (rankv[rootA] > rankv[rootB]) parent[rootB] = rootA;
            else {
                parent[rootB] = rootA;
                rankv[rootA]++;
            }
            return true;
        }
        return false;
    }
};

// ==============================
// 工具函式
// ==============================
void printEdge(const Edge& e) {
    cout << e.u << " - " << e.v << " : " << e.w;
}

void printMST(const vector<Edge>& mst) {
    int total = 0;
    cout << "\nMST edges:\n";
    for (const auto& e : mst) {
        printEdge(e);
        cout << "\n";
        total += e.w;
    }
    cout << "Total weight = " << total << "\n";
}

// ==============================
// 1. Kruskal Algorithm Template
// ==============================
void kruskalMST(int n, vector<Edge> edges) {
    cout << "==============================\n";
    cout << "Kruskal Algorithm\n";
    cout << "==============================\n";

    vector<Edge> mst;

    sort(edges.begin(), edges.end(), [](Edge a, Edge b) {
        return a.w < b.w;
    });

    DSU dsu(n);

    cout << "Selection steps:\n";

    for (const auto& e : edges) {
        if (dsu.unite(e.u, e.v)) { 
            mst.push_back(e);
            cout << "Selected: "; printEdge(e); cout << "\n";
            
            if (mst.size() == n - 1) break;
        } else {
            cout << "Skipped (cycle): "; printEdge(e); cout << "\n";
        }
    }

    printMST(mst);
    cout << "\n";
}

// ==============================
// 2. Prim Algorithm Template
//    從 start 開始
// ==============================
void primMST(int n, const vector<vector<pair<int, int>>>& adj, int start = 1) {
    cout << "==============================\n";
    cout << "Prim Algorithm\n";
    cout << "==============================\n";

    vector<bool> inMST(n + 1, false);
    vector<Edge> mst;

    // 最小堆: {權重, from, to}
    priority_queue<
        tuple<int, int, int>,
        vector<tuple<int, int, int>>,
        greater<tuple<int, int, int>>
    > pq;

    inMST[start] = true;

    for (auto& edge : adj[start]) {
        pq.push(make_tuple(edge.second, start, edge.first));
    }

    cout << "Selection steps:\n";

    while (!pq.empty() && (int)mst.size() < n - 1) {
        auto [w, u, v] = pq.top(); 
        pq.pop();
        if (inMST[v]) continue;
        inMST[v] = true;
        mst.push_back({u, v, w});
        
        cout << "Selected: "; printEdge({u, v, w}); cout << "\n";

        // - 再將新頂點可到達的候選邊放入 pq
        for (auto& next : adj[v]) {
            if (!inMST[next.first]) {
                pq.push(make_tuple(next.second, v, next.first));
            }
        }
    }

    printMST(mst);
    cout << "\n";
}

// ==============================
// 3. Sollin / Boruvka Template
// ==============================
void boruvkaMST(int n, const vector<Edge>& edges) {
    cout << "==============================\n";
    cout << "Sollin / Boruvka Algorithm\n";
    cout << "==============================\n";

    vector<Edge> mst;

    DSU dsu(n);
    int numComponents = n;
    int round = 1;

    while (numComponents > 1) {
        cout << "Round " << round << ":\n";

        // cheapest[i] = 第 i 個 component 目前找到的最便宜邊的 index
        vector<int> cheapest(n + 1, -1);

        for (int i = 0; i < (int)edges.size(); i++) {
            int set1 = dsu.find(edges[i].u);
            int set2 = dsu.find(edges[i].v);

            if (set1 == set2) continue; // 同一 component，跳過

            // 更新 set1 找出的最便宜邊
            if (cheapest[set1] == -1 || edges[i].w < edges[cheapest[set1]].w) {
                cheapest[set1] = i;
            }
            // 更新 set2 找出的最便宜邊
            if (cheapest[set2] == -1 || edges[i].w < edges[cheapest[set2]].w) {
                cheapest[set2] = i;
            }
        }

        bool merged = false;

        for (int i = 1; i <= n; i++) {
            if (cheapest[i] != -1) {
                int index = cheapest[i];
                if (dsu.unite(edges[index].u, edges[index].v)) {
                    mst.push_back(edges[index]);
                    cout << "Selected: "; printEdge(edges[index]); cout << "\n";
                    numComponents--;
                    merged = true;
                }
            }
        }

        if (!merged) break;

        cout << "\n";
        round++;
    }

    printMST(mst);
    cout << "\n";
}

// ==============================
// 主程式
// ==============================
int main() {
    int n = 6;

    // 圖中的無向邊
    vector<Edge> edges = {
        {1, 2, 16},
        {1, 5, 19},
        {5, 4, 18},
        {2, 4, 6},
        {2, 3, 5},
        {4, 3, 10},
        {1, 6, 22},
        {5, 6, 33},
        {6, 2, 12},
        {6, 4, 14}
    };

    // adjacency list，供 Prim 使用
    vector<vector<pair<int, int>>> adj(n + 1);
    for (const auto& e : edges) {
        adj[e.u].push_back({e.v, e.w});
        adj[e.v].push_back({e.u, e.w});
    }

    kruskalMST(n, edges);
    primMST(n, adj, 1);
    boruvkaMST(n, edges);

    return 0;
}
