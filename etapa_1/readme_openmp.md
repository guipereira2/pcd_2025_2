# K-means 1D - Implementação Paralela com OpenMP

---

Implementação paralela do algoritmo K-means 1D usando **OpenMP** com duas estratégias de sincronização: **Reduction** e **Critical Sections**. 

1. **Gerar os dados** de forma pseudoaleatória e reprodutiva em um formato com 1 coluna e sem cabeçalho. 
2. **Implementação da opção A (Reduction)**: Criar `kmeans_1d_parallel_reduction.c` paralelizando os laços `assignment_step` e `update_step` usando `#pragma omp reduction`.
3. **Compilação e benchmark da versão Reduction**: Executar testes com diferentes configurações de threads e políticas de escalonamento (`static`, `dynamic` com chunks variados) para medir Speedup e Eficiência.
4. **Implementar versão paralela com Critical**: Criar `kmeans_1d_parallel_critical.c` usando `#pragma omp critical` no `update_step` como alternativa ao reduction. 
5. **Compilaçãp e benchmark da versão Critical**: Realizar os testes similares ao feitos para à versão reduction. 
6. **Analisar desempenho**: Gerar gráficos de Speedup e Eficiência comparando as implementações. 
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
cd etapa_1
```

Ative o ambiente (se usando ambiente virtual)

```
conda activate kmeans-env # ou: source kmeans-env/bin/activate
```

### Opção 1: Usando Jupyter Notebook (Interface Interativa)

Abra o arquivo openmp.ipynb e execute todas as células. 

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

### 2. Implementação da opção A (Reduction) 

Esta é a implementação paralela `kmeans_1d_parallel_reduction.c` usando OpenMP com reduction.

- **assignment_step_1d:** O laço principal é paralelizado com `#pragma omp parallel for` e usa `reduction(+:sse)` para acumular o SSE. 

- **update_step_1d:** O laço de acumulação é paralelizado usando `reduction(+:sum[:K], cnt[:K])` para somar pontos e contar elementos de cada cluster. 

- **Medição de Tempo:** O main usa `omp_get_wtime()` fornecendo medição. 

- **Agendamento:** Usa `schedule(runtime)` para permitir controle da política de escalonamento (static, dynamic, guided) via variável de ambiente `OMP_SCHEDULE` sem necessidade de recompilar.

- **Parâmetros:** Adiciona número de threads como terceiro argumento na linha de comando, configurado via `omp_set_num_threads()`.

Os resultados (centróides e SSE) são **idênticos** à versão sequencial, garantindo corretude, mas com **tempo de execução reduzido** proporcionalmente ao número de threads disponíveis.

### 3. Compilação e benchmark da versão Reduction 

Esta etapa compila o código paralelo e executa testes para avaliar o desempenho com diferentes configurações de threads e políticas de escalonamento.

- **Compilação:** O código é compilado com as flags `-O2 -std=c99 -fopenmp -lm`, onde `-fopenmp` habilita o suporte OpenMP e `-O2` otimiza o código para desempenho.

- **Baseline Sequencial:** Primeiro, o script executa a versão sequencial para obter o tempo de referência usado nos cálculos de speedup. Este tempo é extraído e armazenado para comparação.

- **Configurações Testadas:**
	- **Threads:** 1, 2, 4, 8 (testando escalabilidade)
	- **Políticas de escalonamento:** static, dynamic com chunks (1, 10, 100), guided
	- **Conjunto de dados:** Conjunto grande (N=1M, K=16)

- **Controle de Escalonamento:** A variável de ambiente `OMP_SCHEDULE` controla a política de escalonamento em tempo de execução. Como o código usa `schedule(runtime)`, é possível testar diferentes políticas sem recompilar, simplesmente alterando esta variável antes de cada execução.

- **Métricas Calculadas:**
	- **Speedup:** Razão entre tempo sequencial e tempo paralelo (T_seq / T_parallel). Indica quantas vezes o código paralelo é mais rápido.
	- **Eficiência:** Speedup normalizado pelo número de threads (Speedup / threads × 100%). Valores próximos a 100% indicam uso eficiente dos recursos de processamento.

- **Processo Automatizado:** O script itera por todas as combinações de threads e políticas de escalonamento, executando o programa, extraindo métricas (tempo, iterações, SSE), calculando speedup e eficiência, e salvando tudo em formato CSV.

- **Saída:** Os resultados são salvos em `resultados_reduction/benchmark_reduction.csv` contendo: threads, schedule, tempo, speedup, eficiência, iterações e SSE para cada configuração testada. Este arquivo será usado posteriormente para análise e geração de gráficos comparativos.

### 4. **Implementar versão paralela com Critical**

Esta é a implementação paralela `kmeans_1d_parallel_critical.c` usando **critical sections** do OpenMP como alternativa ao reduction.

- **assignment_step_1d:** O laço principal  é paralelizado com `#pragma omp parallel for`. Para acumular o SSE utiliza `#pragma omp critical` ao invés de reduction.

- **update_step_1d:** O laço é paralelizado, mas usa `#pragma omp critical` para proteger as atualizações de `sum[c]` e `cnt[c]`. 

- **Medição de Tempo:** Usa `omp_get_wtime()` para medição precisa de wall-clock time, igual à versão reduction.

- **Agendamento:** Utiliza `schedule(runtime)` para permitir controle via `OMP_SCHEDULE`.

Os resultados (centróides e SSE) são **idênticos** às versões sequencial e reduction, mas o **tempo de execução será maior** devido ao overhead das seções críticas, especialmente com muitas threads.

### 5. **Compilação e benchmark da versão Critical** 

Esta etapa compila a versão com seções críticas e executa os mesmos testes da versão reduction.

- **Compilação:** O código é compilado com as mesmas flags: `-O2 -std=c99 -fopenmp -lm`.

- **Baseline Compartilhado:** Utiliza o mesmo tempo sequencial de referência da versão reduction.

- **Configurações Testadas:**
	- **Threads:** 1, 2, 4, 8
	- **Políticas de escalonamento:** static, dynamic (chunks 1, 10, 100), guided
	- **Conjunto de dados:** Conjunto grande (N=1M, K=16)

- **Controle de Escalonamento:** A variável `OMP_SCHEDULE` controla a política em tempo de execução via `schedule(runtime)`.

- **Métricas Calculadas:**
	- **Speedup:** T_seq / T_parallel
	- **Eficiência:** (Speedup / threads) × 100%

- **Comparação Esperada:** Esta versão deve apresentar speedups **menores** e eficiências **piores** que a versão reduction devido ao overhead das seções críticas. A contenção aumenta com mais threads, pois elas competem pelo acesso exclusivo, criando gargalos de serialização.

- **Saída:** Os resultados são salvos em `../resultados/benchmark_critical.csv` no mesmo formato da versão reduction, facilitando análises comparativas posteriores.

### 6. Analisar desempenho: Gerar gráficos de Speedup e Eficiência comparando as implementações.  

Esta etapa compara as implementações Reduction e Critical através de gráficos de Speedup e Eficiência. 