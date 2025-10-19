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

### Opção 1: Usando conda (Anaconda/Miniconda)

Cria ambiente virtual
```
conda create -n kmeans-env python=3.11
```

Instala as dependẽncias
```
conda install numpy pandas jupyter
conda install -c conda-forge altair
```

Desativa ao terminar
```
conda deactivate
``` 

### Opção 2: Usando venv + pip (Python nativo)

Cria ambiente virtual
```
python3 -m venv kmeans-env
```
Ativa o ambiente: 

No Linux/macOS:
```
source kmeans-env/bin/activate
```

No Windows:
```
kmeans-env\Scripts\activate
```

Instala dependências
```
pip install numpy pandas altair jupyter
```

Para desativar quando terminar
```
deactivate
```
