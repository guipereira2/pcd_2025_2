# Projeto Programação Concorrente e Distribuida

--- 

Implementação do algoritmo K-means unidimensional com versões serial e paralela OpenMP.

## Estrutura do projeto 

```
pcd_2025_2/
│
├── dados/ # Dados compartilhados entre etapas
│ ├── dados_pequeno.csv # 10k pontos, K=4
│ ├── centroides_pequeno.csv # 4 centróides iniciais
│ ├── dados_medio.csv # 100k pontos, K=8
│ ├── centroides_medio.csv # 8 centróides iniciais
│ ├── dados_grande.csv # 1M pontos, K=16
│ └── centroides_grande.csv # 16 centróides iniciais
│
├── resultados/ # Resultados compartilhados entre etapas
│ ├── assign_.csv # Atribuições de clusters
│ ├── centroids_.csv # Centróides finais
│ ├── sse_evolution_*.csv # Evolução do SSE
│ ├── benchmark_reduction.csv # Métricas da versão reduction
│ ├── benchmark_critical.csv # Métricas da versão critical
│ └── analise_desempenho.png # Gráficos comparativos
│
├── etapa_0/ # Implementação sequencial (baseline)
│ ├── README.md # Documentação da etapa 0
│ ├── sequencial.ipynb # Notebook principal
│ ├── kmeans_1d_naive.c # Código C sequencial
│ └── kmeans_1d_naive # Executável compilado
│
├── etapa_1/ # Implementação paralela OpenMP
│ ├── README.md # Documentação da etapa 1
│ ├── paralelo_openmp.ipynb # Notebook principal
│ ├── kmeans_1d_parallel_reduction.c # Versão com reduction
│ ├── kmeans_1d_parallel_reduction # Executável reduction
│ ├── kmeans_1d_parallel_critical.c # Versão com critical
│ └── kmeans_1d_parallel_critical # Executável critical
```

## Requisitos 

Para executar o projeto é necessário ter um ambiente Python exstente ou criar um novo ambiente virtual. Depois disso é necessário instalar as depedências. Para criar um ambiente virtual e instalar as dependências escolha uma das opções abaixo: 

**Nota Importante**: É necessário ter o compilador GCC instalado no sistema. 
 - **Linux/Ubuntu**: `sudo apt install build-essential`.
 - **macOS**: `brew install gcc` (ou use Xcode Command Line Tools).
 - **Windows**: Use WSL ou MinGW-w64. 


### Opção 1: Usando conda (Anaconda/Miniconda)

Cria ambiente virtual
```
conda create -n kmeans-env python=3.11
```

Ativa o ambiente 

```
conda activate kmeans-env
```

Instala as dependẽncias
```
conda install numpy pandas jupyter
conda install -c conda-forge altair
```

### Opção 2: Usando venv + pip (Python nativo)

Cria ambiente virtual
```
python3 -m venv kmeans-env
```
Ativa o ambiente: 

```
# No Linux/macOS:
source kmeans-env/bin/activate

# No Windows:
kmeans-env\Scripts\activate
```

Instala dependências
```
pip install numpy pandas altair jupyter
```

## Execução 

Clone o repositório 
``` 
git clone https://github.com/guipereira2/pcd_2025_2.git
```

Navegue até a pasta do projeto 

```
cd pcd_2025_2
```

Navegue até a pasta do etapa desejada

```
cd etapa_X
```

Ative o ambiente (se usando ambiente virtual)

```
conda activate kmeans-env # ou: source kmeans-env/bin/activate
```

### Opção 1: Usando Jupyter Notebook (Interface Interativa)

Abra o arquivo xxxx.ipynb e execute todas as células. 

### Opção 2: Execução Automática via Linha de Comando

```
jupyter nbconvert --execute --to notebook --inplace xxxx.ipynb
```

