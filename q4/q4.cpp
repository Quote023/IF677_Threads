#include <iostream>
#include <vector>
#include <list>

using namespace std;

void print(vector<int> &A)
{
  for (auto &&a : A)
  {
    cout << a << " ";
  }
  cout << "\n";
}

// Disjoint sets (union find)
class Floresta
{
public:
  int n_arvores;
  vector<int> pai;
  vector<int> rank;
  vector<list<int>> arvore;

  Floresta(int n_nodes)
  {

    n_arvores = n_nodes;
    pai.resize(n_nodes);
    rank.resize(n_nodes);
    arvore.resize(n_nodes);
    for (int i = 0; i < n_nodes; i++)
    {
      pai[i] = i;
      rank[i] = 0;
      arvore[i].push_back(i);
    }
  }

  int Find(int node)
  {
    if (pai[node] == node)
    {
      return pai[node];
    }

    int raiz = Find(pai[node]);
    pai[node] = raiz;
    return raiz;
  }

  void Union(int node1, int node2)
  {
    int raiz1 = Find(node1);
    int raiz2 = Find(node2);

    if (rank[raiz1] > rank[raiz2])
      swap(raiz1, raiz2);

    if (rank[raiz1] == rank[raiz2])
      rank[raiz2]++;

    arvore[raiz2].splice(arvore[raiz2].end(),
                         arvore[raiz1]);

    pai[raiz1] = pai[raiz2];
    this->n_arvores -= 1;
  }
};

struct aresta
{
  int node1;
  int node2;
  int peso;
  aresta(int p)
  {
    peso = p;
  }
  aresta(int n1, int n2, int p)
  {
    node1 = n1;
    node2 = n2;
    peso = p;
  }
};

aresta find_best_edge(vector<vector<pair<int, int>>> grafo, Floresta &f, list<int> &arvore)
{
  struct aresta best_edge(INT_MAX);

  // para cada node na arvore
  for (auto node : arvore)
  {
    // olha todas arestas ligadas a este node
    for (auto a : grafo[node])
    {
      int vizinho = a.first;
      int peso_node_vizinho = a.second;

      // apenas arestas que ligam a arvore a outra
      if (f.Find(vizinho) == f.Find(node))
        continue;

      if (best_edge.peso > peso_node_vizinho)
      {
        best_edge = aresta(node, vizinho, peso_node_vizinho);
      };
    }
  }
  return best_edge;
}

vector<aresta> find_best_edges(vector<vector<pair<int, int>>> grafo, Floresta &f, vector<aresta> &mst)
{
  int n_nodes = grafo.size();
  vector<aresta> best_edges;
  for (int node = 0; node < n_nodes; node++)
  {
    // achando raiz que representa a arvore
    if (f.pai[node] == node)
    {
      int raiz = node;

      // melhor aresta que liga esta arvore a outra
      aresta best_edge = find_best_edge(grafo, f, f.arvore[raiz]);
      best_edges.push_back(best_edge);
    }
  }
  return best_edges;
}

struct Params
{
  aresta best_edge;
  vector<aresta> *mst;
  Floresta *f;
};

void *bestEdge(void *param)
{
  aresta best_edge = ((Params *)param)->best_edge;
  int n1 = best_edge.node1;
  int n2 = best_edge.node2;

  if ((*((Params *)param)->f).Find(n1) != (*((Params *)param)->f).Find(n2))
  {
    (*((Params *)param)->mst).push_back(best_edge);
    (*((Params *)param)->f).Union(n1, n2);
  }
  return NULL;
}

vector<aresta> boruvka(vector<vector<pair<int, int>>> grafo)
{
  vector<aresta> mst;

  int n_nodes = grafo.size();
  pthread_t threads[grafo.size()];
  Params *t_params = (Params *)malloc(sizeof(Params) * grafo.size());

  Floresta f(n_nodes);

  while (f.n_arvores > 1)
  {
    vector<aresta> best_edges = find_best_edges(grafo, f, mst);

    int i = 0;
    for (auto best_edge : best_edges)
    {
      t_params[i].best_edge = best_edge;
      t_params[i].mst = &mst;
      t_params[i].f = &f;
      pthread_create(&threads[i], NULL, bestEdge, &t_params[i]);
      i++;
    }

    i = 0;
    for (auto best_edge : best_edges)
    {
      pthread_join(threads[i], NULL);
      i++;
    }
  }
  return mst;
}

int main()
{
  /* Grafo Utilizado:
        2    3
    (0)--(1)--(2)
     |   / \   |
    6| 8/   \5 |7
     | /     \ |
    (3)-------(4)
          9          */

  // estrutura: tupla (Peso, Nó)
  vector<vector<pair<int, int>>> graph = {
      {make_pair(1, 2), make_pair(3, 6)},                                   // 0
      {make_pair(0, 2), make_pair(2, 3), make_pair(3, 8), make_pair(4, 5)}, // 1
      {make_pair(1, 3), make_pair(4, 7)},                                   // 2
      {make_pair(0, 6), make_pair(1, 8), make_pair(4, 9)},                  // 3
      {make_pair(3, 9), make_pair(1, 5), make_pair(2, 7)}                   // 4
  };

  vector<aresta> mst = boruvka(graph);
  for (auto a : mst)
  {
    cout << a.node1 << " - " << a.node2 << " " << a.peso << endl;
  }
  return 0;
}