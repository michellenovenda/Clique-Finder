#include <vector>
#include <csignal>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <algorithm>
#include <string>
#include <list>
#define VERT_NUM 82167

#include <fstream>
#include <iostream>
#include <set>
#include <string.h>
#include <map>
#include <assert.h>

using namespace std;
vector<int> cur_best_clique(VERT_NUM+100);
vector<int> k_core_index;
vector<int> k_core_degree;
vector<vector<bool>> conn(VERT_NUM+100, vector<bool>(VERT_NUM+100));
vector<vector<bool>> e(VERT_NUM+100, vector<bool>(VERT_NUM+100));

multimap<int, int> graph;
set<int> v;
int K;

class MaxClique
{
    private:
        int pk, level, K;
        float time_limit;

        class Vertices
        {
            private:
                class Vertex
                {
                    private:
                        int i, degree;
                    public:
                        void set_i(const int val) {
                            i = val; }
                        int get_i() const {
                            return i; }
                        void set_deg(int deg) {
                            degree = deg; }
                        int get_deg() const {
                            return degree; }
                };

                Vertex* vertex;
                int size;
                static bool desc_deg(const Vertex A, const Vertex B) {
                    bool val = (A.get_deg() > B.get_deg());
                    return val;
                }

            public:
                Vertices(int size) : size(0) {vertex = new Vertex[size];}
                ~Vertices() {}
                void del() {
                    delete[] vertex; }
                void sort_vertices() {
                    sort(vertex, vertex+size, desc_deg);
                }
                void initialize_colors() {
                    const int max_deg = vertex[0].get_deg();

                    for(int i=0; i<max_deg; i++)
                        vertex[i].set_deg(i+1);
                    for(int i=max_deg; i<size; i++)
                        vertex[i].set_deg(max_deg+1);
                }
                void set_degree(MaxClique& m) {
                    for(int i=0; i<size; i++)
                    {
                        int deg = 0;
                        for(int j=0; j<size; j++)
                        {
                            if(m.connection(vertex[i].get_i(), vertex[j].get_i()))
                                deg++;
                        }
                        vertex[i].set_deg(deg);
                    }
                }
                int get_size() const {
                    return size; }
                void push(const int val) {
                    int idx = size++;
                    vertex[idx].set_i(val); };
                void pop() {
                    size--; };
                Vertex& get_vertex(const int index) const {
                    return vertex[index]; };
                Vertex& top() const {
                    return vertex[size-1]; };
                int get_index(const int i) const {
                    return vertex[i].get_i();
                }
                int get_degree(const int i) const {
                    return vertex[i].get_deg();
                }
        };

        class ColorClass
        {
            private:
                vector<int> arr;
                int size;
            public:
                ColorClass() : size(0), arr(0) {}
                ColorClass(const int size) : size(size), arr(0) {initialize(size);}
                ~ColorClass() {}
                void initialize(const int size) {
                    vector<int> arr(size);
                    clear();    
                }
                void push(const int val) {
                    arr.push_back(val);
                    size = arr.size();
                }
                void pop() {
                    arr.pop_back();
                    size = arr.size();
                }
                void clear() {
                    arr.clear();
                    size = arr.size();
                }
                int get_size() const {
                    return arr.size(); }
                int get_arr(const int index) const {
                    return arr.at(index); }
        };

        class StepCount
        {
            private:
                int step1, step2;
            public:
                StepCount() : step1(0), step2(0) {}
                void set_1(const int val) {
                    step1 = val; }
                int get_1() const {
                    return step1; }
                void set_2(const int val) {
                    step2 = val; }
                int get_2() const {
                    return step2; }
                void increment() {
                    step1++; }
        };

        Vertices V;
        ColorClass * C;
        ColorClass clique_max;
        ColorClass clique;
        StepCount* S;

        bool connection(const int i, const int j) const
        {
            return e.at(i).at(j);
        }

        bool intersection_1(const int idx1, const ColorClass& idx2)
        {
            for(int i=0; i<idx2.get_size(); i++) {
                if(connection(idx1, idx2.get_arr(i)))
                    return true;
            }
            return false;
        }

        void intersection_2(const Vertices& A, Vertices& B)
        {
            for(int i=0; i<A.get_size()-1; i++)
            {
                if(connection(A.top().get_i(), A.get_vertex(i).get_i()))
                    B.push(A.get_vertex(i).get_i());
            }
        }

        void color_sort(Vertices& R)
        {
            int idx = 0;
            int max_num = 1;
            int min_k = clique_max.get_size() - clique.get_size() + 1;

            C[1].clear();
            C[2].clear();

            int k = 1;

            for(int i=0; i<R.get_size(); i++)
            {
                int index = R.get_vertex(i).get_i();
                k = 1;

                while(intersection_1(index, C[k]))
                    k++;

                if(k > max_num)
                {
                    max_num = k;
                    C[max_num+1].clear();
                }

                C[k].push(index);

                if(k < min_k)
                    R.get_vertex(idx++).set_i(index);
            }

            if(idx > 0)
                R.get_vertex(idx-1).set_deg(0);

            if(min_k <= 0)
                min_k = 1;

            for(k = min_k; k<=max_num; k++)
            {
                for(int i=0; i<C[k].get_size(); i++)
                {
                    R.get_vertex(idx).set_i(C[k].get_arr(i));
                    R.get_vertex(idx++).set_deg(k);
                }
            }
        }

        void expand_dyn(Vertices R)
        {
            S[level].set_1(S[level].get_1() + S[level-1].get_1() - S[level].get_2());
            S[level].set_2(S[level-1].get_1());

            while(R.get_size())
            {
                if(clique.get_size() + R.top().get_deg() > clique_max.get_size())
                {
                    clique.push(R.top().get_i());
                    Vertices R2(R.get_size());
                    intersection_2(R, R2);
                    if(R2.get_size())
                    {
                        if((float)(S[level].get_1() / ++pk < time_limit))
                        {
                            sort_deg(R2);
                        }

                        color_sort(R2);
                        S[level].increment();
                        level++;
                        expand_dyn(R2);
                        level--;
                    }

                    else if(clique.get_size() > clique_max.get_size())
                    {
                        clique_max.initialize(clique.get_size());
                        clique_max = clique;
                        cur_best_clique.push_back(clique_max.get_size());
                        cout << "Current best clique size: " << cur_best_clique.back() << endl;
                    }

                    R2.del();
                    clique.pop();
                }

                else
                    return;

                R.pop();
            }
        }

        void _mcq(int& size, const int K)
        {
            V.set_degree(*this);
            V.sort_vertices();

            for(int i=0; i<V.get_size(); i++) {
                k_core_index.push_back(i);
                k_core_degree.push_back(V.get_degree(i));
            }
            V.initialize_colors();

            for(int i=0; i<V.get_size()+1; i++)
            {
                S[i].set_1(0);
                S[i].set_2(0);
            }

            expand_dyn(V);

            size = clique_max.get_size();
        }

        void sort_deg(Vertices& R)
        {
            R.set_degree(*this);
            R.sort_vertices();
        }

    public:
        MaxClique(const int size, const int K, const float dec = 0.25) : K(K), pk(0), level(1), time_limit(dec), V(size), clique(size), clique_max(size) {}

        int steps() const {
            return pk;
        }

        void initial(const int size)
        {
            for(int i=0; i<size; i++) {
                V.push(i);
            }
            e = conn;
            C = new ColorClass[size+1];

            for(int i=0; i<size+1; i++) {
                C[i].initialize(size+1);
            }

            S = new StepCount[size+1];
        }

        void maxclique(int& size)
        {
            _mcq(size, K);
        }

        ~MaxClique() {
            delete[] C;
            delete[] S;
            V.del();
        }
};

void signalHandler( int signum ) {
    /*
    In the signal handler, we output the current best clique that we found.
    */
    fstream out;
    out.open("clique.txt" , ios::out);
    sort(cur_best_clique.begin() , cur_best_clique.end());

    for(int v : cur_best_clique){
        out<<v<<endl;
    }

    fstream printout;
    printout.open("kcore.txt", ios::out);
    for(int i=0; i<VERT_NUM; i++){
        if(k_core_degree.at(i) >= K)
            printout << k_core_index.at(i) << " " << k_core_degree.at(i) << endl;
    }

    exit(signum);
}

int main(int argc , char *argv[])
{
    signal(SIGINT, signalHandler);
    freopen(argv[1] , "r" , stdin);
    int K = stoi(argv[2]);
    
    int A , B;
    while(cin >> A >> B){
        if(A == B){
            continue;
        }
        v.insert(B);
        v.insert(A);
        graph.insert(make_pair(A, B));
    }
    
    int graph_size = *v.rbegin() + 1;

    for(multimap<int, int>::iterator it=graph.begin(); it != graph.end(); it++)
    {
        conn.at(it->first).at(it->second) = true;
        conn.at(it->second).at(it->first) = true;
    }

    MaxClique m(graph_size, K);
    m.initial(graph_size);
    int cur_best_clique_size;
    m.maxclique(cur_best_clique_size);

    sort(cur_best_clique.begin() , cur_best_clique.end());
    cur_best_clique.erase(unique(cur_best_clique.begin(), cur_best_clique.end()), cur_best_clique.end());

    fstream out;
    out.open("clique.txt" , ios::out);
    for(int v : cur_best_clique){
        out<<v<<endl;
    }

    fstream printout;
    printout.open("kcore.txt", ios::out);
    for(int i=0; i<VERT_NUM; i++){
        if(k_core_degree.at(i) >= K)
            printout << k_core_index.at(i) << " " << k_core_degree.at(i) << endl;
    }
}