#include <iostream>
#include <fstream>
#include <list>
#include <chrono>

using namespace std;
using namespace std::chrono;

typedef struct vertice
{
  int nome;//nome do vértice 0, 1, 2 , ... ,n
  int situacao = 0;//se 0: intocado, 1: Defendido, 2: Queimado
  int marcador = -1;//se for nome do vertice já foi analisado a partir de um vértice n;
  double soma = 0;//critério para defender um vértice, quanto menor a soma de um vértice maior a necessidade de ser defendido.
}Tvertice;

typedef struct defendidos
{
  int v;
  int rod;
}Tdef;

double somatorio(list<Tvertice>* grafo, list<int> indices_focos, int n_def, int foco);
int menor(list<Tvertice>* grafo, list<int>& auxil);
void algoritmo(list<Tvertice>* grafo, list<int> indices_focos, int n_vertices,int n_focos,int& numero_de_vqueimados, list<Tdef>& defendidos, int n_def);

int main()
{
  //declaração de variáveis
  list<Tdef> defendidos;//lista que vai guardar o nome do vértice defendido e a rodada em que isso aconteceu
  list<Tvertice>* grafo;//grafo com vértices e adjacências
  list<int> indices_focos;//salva os indices de focos de incendio
  char nome_arquivo[50];
  int n_vertices, n_arestas,n_def, n_focos, aux, aux2, numero_de_vqueimados;
  Tvertice auxiliar;

  ifstream arq;

  //abro o arquivo texto
  cout <<"\n\nDigite o nome do arquivo texto: ";
  cin >> nome_arquivo;
  arq.open(nome_arquivo);

  arq >> n_vertices >> n_arestas >> n_def >> n_focos;//leio os primeiros dados do problema

  grafo = new list<Tvertice>[n_vertices];//cria um grafo com n vértices

  for(int i = 0; i < n_vertices; i++)//insere os n vertices no grafo
  {
    auxiliar.nome = i;
    grafo[i].push_back(auxiliar);
  }

  for(int i = 0; i <n_focos; i++)//salva numa lista os indices dos focos
  {
    arq >> aux;
    indices_focos.push_back(aux);
  }

  for(int i = 0; i < n_arestas; i++)//salva as adjacencias do grafo
  {
    arq >> aux >> aux2;
    auxiliar.nome = aux2;
    grafo[aux].push_back(auxiliar);

    auxiliar.nome = aux;
    grafo[aux2].push_back(auxiliar);
  }

  arq.close();//fecho o arquivo txt

  duration<double> time_span2;
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  algoritmo(grafo, indices_focos, n_vertices,n_focos,numero_de_vqueimados, defendidos,n_def);//roda o algoritmo

  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double> time_span = duration_cast<duration<double> >(t2 - t1);

  cout << endl << endl;

  for(auto it = defendidos.begin(); it != defendidos.end(); it++)//após a execuçao do algoritmo imprime os vértices defendidos e a rodada em que acontceu a defesa
  {
      cout << "Vértice defendido: " << it->v << "\nNa rodada: " << it->rod;
      cout << endl;
  }

  cout <<"\nNúmero de vértices queimados: " << numero_de_vqueimados << endl;//imprimo a quantidade de vértices queimados

  cout <<"\nDuração "  << time_span.count() <<" s" << endl << endl;

  delete[] grafo;

  return 0;
}

double somatorio(list<Tvertice>* grafo, list<int> indices_focos, int n_def, int foco)//calcula aproximadamente quantos vértices vão se queimar a partir de um vertice defendido
{
  list<int> analise;
  list<int> analise2;
  double soma, sucessores, antecessores, anterior, aux;

  grafo[foco].front().marcador = foco;//primeiramente o vértice analisado fica como marcado

  //colocar na lista a partir de quais vértices serão analisados, excluindo o foco de análise principal. Marcar os adjacentes como já passados
  for(auto it = indices_focos.begin(); it != indices_focos.end(); it++)//a partir dos focos de incendio
  {
    for(auto it2 = grafo[*it].begin(); it2 != grafo[*it].end(); it2++)
    {
      if(grafo[it2->nome].front().situacao == 0 && grafo[it2->nome].front().marcador != foco)//o vértice é intocado e não é o da análise
      {
        analise.push_back(it2->nome);
        grafo[it2->nome].front().marcador = foco;
      }
    }
  }
  //primeira iteração
  soma = analise.size();
  antecessores = soma;
  anterior = antecessores;

  aux = anterior;

  //agora calcula a soma a partir dos vértices restantes
  while(analise.size() != 0 && aux >= 1)
  {
      while(analise.size() != 0)
      {
        auto it = analise.begin();

        for(auto it2 = grafo[*it].begin(); it2 != grafo[*it].end(); it2++)//percorre adjacencias de um vétice
        {
          if(grafo[it2->nome].front().situacao == 0 && grafo[it2->nome].front().marcador != foco)
          {
            analise2.push_back(it2->nome);
            grafo[it2->nome].front().marcador = foco;
            sucessores++;
          }
        }
        analise.pop_front();
      }

      aux = (sucessores/antecessores)*anterior - n_def;

      if(aux >= 1)
      {
        soma = soma + aux;
        antecessores = sucessores;
        anterior = aux;
      }

      for(auto it = analise2.begin(); it != analise2.end(); it++)
        analise.push_back(*it);

      sucessores = 0;
  }
  return soma;
}

int menor(list<Tvertice>* grafo, list<int>& auxil)//a partir de uma lista dos vértices candidatos a serem defendidos escolho o que tive menor soma e retiro ele da lista
{
  double m  = grafo[auxil.front()].front().soma;
  int nome = auxil.front();

  if(auxil.empty())//recebi algo vazio
    return -1;

  for(auto it = auxil.begin(); it != auxil.end(); it++)//acha o maior elemento e salva seu nome(vértice)
  {
    if(grafo[*it].front().soma <= m)
    {
      m = grafo[*it].front().soma;
      nome = grafo[*it].front().nome;
    }
  }
  auxil.remove(nome);//remove o vertice de menor soma pois vai ser escolhido para ter o defensor

  return nome;//retorna o nome do vértice que tem a maior soma ou seja maior perigo de espalhar pelo grafo todo caso o fogo chegue até ele
}

void algoritmo(list<Tvertice>* grafo, list<int> indices_focos, int n_vertices,int n_focos,int& numero_de_vqueimados, list<Tdef>& defendidos, int n_def)
{
  int confere;//confere se nao estou analisando um mesmo vertice 2 vezes
  int m;// m irá recebe valor da funcao menor
  int rodada = 0;
  Tvertice auxiliar;
  Tdef aux;
  list<int> auxil;//lista auxiliar

  //cout << somatorio(grafo, 5);

  numero_de_vqueimados = 0;//inicialização dos vértices queimados

  while(indices_focos.size()!= 0)//enquanto tiver foco de incendio
  {
    rodada++;

    //marcar focos como queimados
    for(auto it = indices_focos.begin(); it != indices_focos.end(); it++)//esse for percorre os vértices com focos de incêndio e marca eles como queimados e insere eles em uma lista auxiliar
    {
      grafo[*it].front().situacao=2;
      numero_de_vqueimados++;//atualiza os vertices queimados
      auxil.push_back(*it);//lista auxiliar
    }

    //ver próximos a serem queimados
    for(auto it = indices_focos.begin(); it != indices_focos.end(); it++)//esse for percorre todos os vértices com focos de incêndio
    {
       for(auto it2 = grafo[*it].begin(); it2 != grafo[*it].end(); it2++)//percorre as adjacências de um foco
       {
         confere =0;

         if(grafo[it2->nome].front().situacao == 0)//vertice intocado
         {
           for(auto it3 = auxil.begin(); it3 != auxil.end(); it3++)//confere se já nao coloquei na lista
           {
             if(*it3 == it2->nome)
             {
              confere = 1;
             }
           }
           if(confere == 0)
           {
             auxil.push_back(it2->nome);//coloco ele na lista auxiliar como próximo a ser queimado
           }
         }
       }
       auxil.pop_front();//remove os elementos já colocados como queimados
    }

    if(auxil.empty())//nao tem mais pra ser queimado caso a lista auxiliar esteja vazia
      break;

    for(int i = 0; i < n_def; i ++)//calcula a soma, coloca os defensores, remove eles da lista de proximos a serem queimados, e salva o nome do vértice defendido e a rodada da defesa
    {
      for(int j = 0; j < n_vertices; j++)//limpar marcadores
        grafo[j].front().marcador = -1;

      for(auto it2 = auxil.begin(); it2 != auxil.end(); it2++)
        grafo[*it2].front().soma = somatorio(grafo, indices_focos, n_def, *it2);//o vertice preste a ser queimado recebe sua soma

      m = menor(grafo, auxil);

      if(m != -1)// nao está vazio
      {
        grafo[m].front().situacao = 1;
        aux.v = m;
        aux.rod = rodada;
        defendidos.push_back(aux);
      }

    }
    indices_focos.clear();//o resto vai ser queimado

    for(auto it = auxil.begin(); it != auxil.end(); it++)//coloco em indices de focos os vértice que não puderam ser defendidos
    {
      indices_focos.push_back(*it);
    }
    auxil.clear();//limpo a lista auxiliar e continuo o loop
  }
}
