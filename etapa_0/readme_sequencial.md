# K-means 1D - Implementação Sequencial

--- 

Implementação sequencial (baseline) do algoritmo K-means para dados unidimensionais em um notebook python (`sequencial.ipynb`). Este projeto serve como ponto de referência para futuras implementações paralelizadas (OpenMP, CUDA e MPI). 

O notebook, tem como objetivo: 
1. **Gerar os dados** de forma pseudoaleatória e reprodutiva em um formato com 1 coluna e sem cabeçalho. 
2. **Visualizar** os dados gerados.
3. **Implementar o código** C `kmeans_Id_naive.c` para a versão sequencial. 
4. **Executar** o código em C e coletar **SSE por iteração, tempo total, iterações** que servirão de baseline. 
5. **Analisar** graficamente a convergência do algoritmo. 

---

## Pré-requisitos 

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

Navegue até a pasta do etapa sequencial 
```
cd etapa_0
```

Ative o ambiente (se usando ambiente virtual)

```
conda activate kmeans-env # ou: source kmeans-env/bin/activate
```

### Opção 1: Usando Jupyter Notebook (Interface Interativa)

Abra o arquivo sequencial.ipynb e execute todas as células. 

### Opção 2: Execução Automática via Linha de Comando

jupyter nbconvert --execute --to notebook --inplace sequencial.ipynb

--- 

## Fluxo do notebook

### 1. Geração dos dados pseudoaleatórios e reprodutivos 

Será usado a biblioteca `NumPy` para gerar dados sintéticos 1D que serão usados posteriormente no K-means. O processo garante a reprodutibilidade. 

A geração consiste em: 
1. Fixar a semente aleatória `np.random.seed(24)`para garantir reprodutibilidade.
2. Criar pontos distribuídos em faixas distintas (ex: 0, 10, 20, 30) usando distribuição normal.
3. Gerar três conjuntos de teste com tamanhos crescentes:
	- Pequeno: N=10⁴ pontos, K=4 clusters
	- Médio: N=10⁵ pontos, K=8 clusters
	- Grande: N=10⁶ pontos, K=16 clusters

4. Selecionar centróides iniciais aleatoriamente dos dados gerados para cada conjunto.
5. Embaralhar os pontos usando `np.random.shuffle`
6. Converter os dados para DataFrame do pandas e salvar em arquivos CSV (1 coluna, sem cabeçalho, sem índice) usando `to_csv(index=False, header=False)`:
	- `dados_*.csv` contendo os N pontos
	- `centroides_*.csv` contendo os K centróides iniciais 

### 2. Visualização dos dados 

Nesta etapa, é feita a visualização dos dados usando a biblioteca Altair para criar visualizações interativas dos três conjuntos de dados. 

A visualização consiste em:

1. Preparar os dados convertendo os arrays NumPy para DataFrames do pandas, separando pontos e centróides.

2. Aplicar jitter (dispersão aleatória) no eixo Y usando a transformação Box-Muller, pois os dados são 1D e ficariam sobrepostos. O jitter é apenas visual. 

3. Criar scatter plots com pontos azuis representando os dados, cruzes vermelhas marcando os centróides iniciais, e linha horizontal cinza como referência.

### 3: Implementação k-means sequencial 

Implementação do código `kmeans_1d_naive.c`com o uso `%%writefile%%`.   

### 4: Compilação do k-means sequencial 

Agora com os dados gerados e o arquivo C gerado, é possível executar o k-means.

A **compilação** é feita usando o `GCC` com as flags `02` para uma medição justa e `-lm` para vincular a biblioteca matemática e o padrão é específicado com `-std=c99`. 

Já a `execução` é feita para três conjuntos de dados distintos, passando os seguintes parâmetros: 

- `dados_[tamanho].csv`: Arquivos com os pontos de dados (10k, 100k e 1M pontos).
- `centroides_[tamanho].csv`: Centróides iniciais (4, 8 e 16 centróides respectivamente).
- `100`: max_iter (número máximo de iterações).
- `1e-6`: eps (critério de parada por convergência, o algoritmo para quando a variação relativa do SSE é menor que este valor).
- `assign_[tamanho].csv`: Arquivo de saída com as atribuições de cluster para cada ponto.
- `centroids_[tamanho].csv`: Arquivo de saída com os centróides finais após convergência.
- `sse_evolution_[tamanho].csv`: Arquivo que registra a evolução do SSE a cada iteração.

O programa imprime o **número de iterações, SSE final e tempo de execução em milissegundos**. Estes valores servirão como baseline para calcular o speedup das versões paralelizadas (OpenMP, CUDA e MPI).

### 5. Analisar graficamente a convergência do algoritmo. 

Para visualizar a convergência dos diferentes conjuntos de dados, foi criado um gráfico de linha que mostra a evolução do SSE a cada iteraçao para os três conjuntos de dados (pequeno, médio e grande). 

--- 

## Arquivos gerados 

Após a execução completa do notebook, os seguintes arquivos serão criados:

### Dados de Entrada
- **dados_pequeno.csv**: Conjunto de 10.000 pontos 1D gerados (N=10k, K=4)
- **centroides_pequeno.csv**: 4 centróides iniciais para o conjunto pequeno
- **dados_medio.csv**: Conjunto de 100.000 pontos 1D gerados (N=100k, K=8)
- **centroides_medio.csv**: 8 centróides iniciais para o conjunto médio
- **dados_grande.csv**: Conjunto de 1.000.000 pontos 1D gerados (N=1M, K=16)
- **centroides_grande.csv**: 16 centróides iniciais para o conjunto grande

### Código Compilado
- **kmeans_1d_naive.c**: Código-fonte C do algoritmo K-means sequencial
- **kmeans_1d_naive**: Executável compilado da versão sequencial

### Resultados (Pasta `resultados/`)
- **assign_pequeno.csv**: (Saída) Índice do cluster final para cada um dos 10k pontos
- **centroids_pequeno.csv**: (Saída) Coordenadas finais dos 4 centróides após convergência
- **sse_evolution_pequeno.csv**: (Saída) Log do valor do SSE em cada iteração, usado para gerar gráficos
- **assign_medio.csv**: (Saída) Índice do cluster final para cada um dos 100k pontos
- **centroids_medio.csv**: (Saída) Coordenadas finais dos 8 centróides após convergência
- **sse_evolution_medio.csv**: (Saída) Log do valor do SSE em cada iteração
- **assign_grande.csv**: (Saída) Índice do cluster final para cada um dos 1M pontos
- **centroids_grande.csv**: (Saída) Coordenadas finais dos 16 centróides após convergência
- **sse_evolution_grande.csv**: (Saída) Log do valor do SSE em cada iteração