#include <iostream>
#include <vector>
#include <list>
#include <climits>
#include <algorithm>

using namespace std;
using Grafo = vector<vector<pair<int, int>>>;

struct Aresta
{
  int a, b, weight;
};

struct Arvore
{
  int pai;
  int rank;
  list<int> data;
};

class Arvores
{
public:
  int tamanho;
  vector<Arvore> arvores;

  Arvores(int size)
  {
    tamanho = size;
    arvores.resize(size); // uma arvore pra cada nó
    for (int i = 0; i < size; i++)
    {
      arvores[i].pai = i;
      arvores[i].rank = i;
      arvores[i].data.push_back(i);
    }
  }

  int FindRaiz(int node)
  {
    if (arvores[node].pai == node)
      return arvores[node].pai;

    int r = FindRaiz(arvores[node].pai);
    arvores[node].pai = r;
    return r;
  }

  void Union(int node1, int b)
  {
    int r1 = FindRaiz(node1);
    int r2 = FindRaiz(b);

    if (arvores[r1].rank > arvores[r2].rank)
      swap(r1, r2);

    if (arvores[r1].rank == arvores[r2].rank)
      arvores[r2].rank++;

    arvores[r2].data.splice(arvores[r2].data.end(), arvores[r1].data);

    arvores[r1].pai = arvores[r2].pai;
    this->tamanho -= 1;
  }
};

struct ThreadProps
{
  vector<Aresta> *mst;
  Arvores *floresta;
  Aresta bestEdge;
};

Aresta findBestEdge(Grafo grafo, Arvores &arvores, list<int> &arvore)
{
  struct Aresta best = {.weight = INT_MAX};

  for (auto node : arvore)
    for (auto aresta : grafo[node])
    {
      // apenas arestas que ligam a arvore a outra
      if (arvores.FindRaiz(aresta.first) == arvores.FindRaiz(node))
        continue;

      if (best.weight > aresta.second)
        best = Aresta{.a = node, .b = aresta.first, .weight = aresta.second};
    }

  return best;
}

vector<Aresta> findBestEdges(Grafo grafo, Arvores &f, vector<Aresta> &mst)
{
  vector<Aresta> bestEdges;
  for (int node = 0; node < grafo.size(); node++)
  {
    // Apenas raizes
    if (f.arvores[node].pai != node)
      continue;

    Aresta bestEdge = findBestEdge(grafo, f, f.arvores[node].data);
    bestEdges.push_back(bestEdge);
  }
  return bestEdges;
}

void *bestEdge(void *args)
{
  struct ThreadProps *props = (ThreadProps *)args;
  Aresta bestEdge = props->bestEdge;
  int n1 = bestEdge.a;
  int n2 = bestEdge.b;

  if (props->floresta->FindRaiz(n1) != props->floresta->FindRaiz(n2))
  {
    props->mst->push_back(bestEdge);
    props->floresta->Union(n1, n2);
  }
  return NULL;
}

/*
ALGORITMO BORUVKA(G)
    Crie uma floresta F com cada nó do grafo
    Crie um vetor de LIDER com todos os nós

    para cada nó u em G
        LIDER[u] = u

    Enquanto F > 1 faça
        para cada componente c em G
            ache a aresta cv de menor custo
            se FIND(c, LIDER) diferente de FIND(v, LIDER) então
                adicione cv a F
                UNION(c, v)
FIM
*/
vector<Aresta> boruvka(Grafo grafo)
{
  vector<Aresta> mst;
  pthread_t threads[grafo.size()];

  Arvores f(grafo.size());

  while (f.tamanho > 1)
  {
    vector<Aresta> best_edges = findBestEdges(grafo, f, mst);
    ThreadProps *t_params = (ThreadProps *)malloc(sizeof(ThreadProps) * best_edges.size());

    for (int i = 0; i < best_edges.size(); ++i)
    {
      t_params[i].bestEdge = best_edges[i];
      t_params[i].mst = &mst;
      t_params[i].floresta = &f;
      pthread_create(&threads[i], NULL, bestEdge, &t_params[i]);
    }

    for (int i = 0; i < best_edges.size(); ++i)
      pthread_join(threads[i], NULL);

    free(t_params);
  }

  sort(mst.begin(), mst.end(), [](const Aresta &a, const Aresta &b) -> bool
       { return a.weight < b.weight; });

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
  Grafo graph = {
      {make_pair(1, 2), make_pair(3, 6)},                                   // 0
      {make_pair(0, 2), make_pair(2, 3), make_pair(3, 8), make_pair(4, 5)}, // 1
      {make_pair(1, 3), make_pair(4, 7)},                                   // 2
      {make_pair(0, 6), make_pair(1, 8), make_pair(4, 9)},                  // 3
      {make_pair(3, 9), make_pair(1, 5), make_pair(2, 7)}                   // 4
  };

  vector<Aresta> mst = boruvka(graph);
  cout << "(x-y)\tPESO" << endl;
  for (Aresta a : mst)
    cout << "(" << a.a << "-" << a.b << ")\t" << a.weight << endl;

  return 0;
}