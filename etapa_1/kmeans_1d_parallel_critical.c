/* kmeans_1d_parallel_critical.c
   K-means 1D com OpenMP usando CRITICAL SECTION
   - Paraleliza assignment_step com critical para acumular SSE
   - Paraleliza update_step com critical para atualizar sum e count
   
   Compilar: gcc -O2 -std=c99 -fopenmp kmeans_1d_parallel_critical.c -o kmeans_1d_parallel_critical -lm
   Uso:      ./kmeans_1d_parallel_critical dados.csv centroides_iniciais.csv [num_threads] [max_iter=100] [eps=1e-6] [assign.csv] [centroids.csv] [sse_evolution.csv]
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

/* ---------- util CSV 1D: cada linha tem 1 número ---------- */
static int count_rows(const char *path){
    FILE *f = fopen(path, "r");
    if(!f){ fprintf(stderr,"Erro ao abrir %s\n", path); exit(1); }
    int rows=0; char line[8192];
    while(fgets(line,sizeof(line),f)){
        int only_ws=1;
        for(char *p=line; *p; p++){
            if(*p!=' ' && *p!='\t' && *p!='\n' && *p!='\r'){ only_ws=0; break; }
        }
        if(!only_ws) rows++;
    }
    fclose(f);
    return rows;
}

static double *read_csv_1col(const char *path, int *n_out){
    int R = count_rows(path);
    if(R<=0){ fprintf(stderr,"Arquivo vazio: %s\n", path); exit(1); }
    double *A = (double*)malloc((size_t)R * sizeof(double));
    if(!A){ fprintf(stderr,"Sem memoria para %d linhas\n", R); exit(1); }
    FILE *f = fopen(path, "r");
    if(!f){ fprintf(stderr,"Erro ao abrir %s\n", path); free(A); exit(1); }
    char line[8192];
    int r=0;
    while(fgets(line,sizeof(line),f)){
        int only_ws=1;
        for(char *p=line; *p; p++){
            if(*p!=' ' && *p!='\t' && *p!='\n' && *p!='\r'){ only_ws=0; break; }
        }
        if(only_ws) continue;
        const char *delim = ",; \t";
        char *tok = strtok(line, delim);
        if(!tok){ fprintf(stderr,"Linha %d sem valor em %s\n", r+1, path); free(A); exit(1); }
        A[r] = atof(tok);
        r++;
        if(r>R) break;
    }
    fclose(f);
    *n_out = R;
    return A;
}

static void write_assign_csv(const char *path, const int *assign, int N){
    if(!path) return;
    FILE *f = fopen(path, "w");
    if(!f){ fprintf(stderr,"Erro ao abrir %s para escrita\n", path); return; }
    for(int i=0;i<N;i++) fprintf(f, "%d\n", assign[i]);
    fclose(f);
}

static void write_centroids_csv(const char *path, const double *C, int K){
    if(!path) return;
    FILE *f = fopen(path, "w");
    if(!f){ fprintf(stderr,"Erro ao abrir %s para escrita\n", path); return; }
    for(int c=0;c<K;c++) fprintf(f, "%.6f\n", C[c]);
    fclose(f);
}

/* ---------- k-means 1D PARALELO COM CRITICAL ---------- */

/* assignment: para cada X[i], encontra c com menor (X[i]-C[c])^2 
   PARALELIZADO: usa critical section para somar SSE */
static double assignment_step_1d(const double *X, const double *C, int *assign, int N, int K){
    double sse = 0.0;
    
    #pragma omp parallel for schedule(runtime)
    for(int i=0; i<N; i++){
        int best = -1;
        double bestd = 1e300;
        for(int c=0; c<K; c++){
            double diff = X[i] - C[c];
            double d = diff*diff;
            if(d < bestd){ bestd = d; best = c; }
        }
        assign[i] = best;
        
        // Usar critical section para acumular SSE de forma segura
        #pragma omp critical
        {
            sse += bestd;
        }
    }
    return sse;
}

/* update: média dos pontos de cada cluster (1D)
   PARALELIZADO: usa critical section para atualizar sum e count */
static void update_step_1d(const double *X, double *C, const int *assign, int N, int K){
    double *sum = (double*)calloc((size_t)K, sizeof(double));
    int *cnt = (int*)calloc((size_t)K, sizeof(int));
    if(!sum || !cnt){ fprintf(stderr,"Sem memoria no update\n"); exit(1); }
    
    // Paralelizar com critical section para atualizações seguras
    #pragma omp parallel for schedule(runtime)
    for(int i=0; i<N; i++){
        int c = assign[i];
        
        // Usar critical section para atualizar sum e count
        #pragma omp critical
        {
            sum[c] += X[i];
            cnt[c] += 1;
        }
    }
    
    // Atualizar centróides
    for(int c=0; c<K; c++){
        if(cnt[c] > 0) C[c] = sum[c] / (double)cnt[c];
        else           C[c] = X[0]; // cluster vazio recebe primeiro ponto
    }
    
    free(sum); 
    free(cnt);
}

static void kmeans_1d(const double *X, double *C, int *assign,
                      int N, int K, int max_iter, double eps,
                      int *iters_out, double *sse_out, const char *sse_path)
{
    // Abrir arquivo para salvar evolução do SSE
    FILE *sse_file = NULL;
    if(sse_path){
        sse_file = fopen(sse_path, "w");
        if(sse_file) fprintf(sse_file, "iteration,sse\n");
    }
    
    double prev_sse = 1e300;
    double sse = 0.0;
    int it;
    
    for(it=0; it<max_iter; it++){
        sse = assignment_step_1d(X, C, assign, N, K);
        
        // Salvar SSE desta iteração
        if(sse_file) fprintf(sse_file, "%d,%.6f\n", it, sse);
        
        /* parada por variação relativa do SSE */
        double rel = fabs(sse - prev_sse) / (prev_sse > 0.0 ? prev_sse : 1.0);
        if(rel < eps){ it++; break; }
        
        update_step_1d(X, C, assign, N, K);
        prev_sse = sse;
    }
    
    // Fechar arquivo de evolução do SSE
    if(sse_file) fclose(sse_file);
    
    *iters_out = it;
    *sse_out = sse;
}

/* ---------- main ---------- */

int main(int argc, char **argv){
    if(argc < 3){
        printf("Uso: %s dados.csv centroides_iniciais.csv [num_threads=4] [max_iter=100] [eps=1e-6] [assign.csv] [centroids.csv] [sse_evolution.csv]\n", argv[0]);
        printf("Obs: arquivos CSV com 1 coluna (1 valor por linha), sem cabeçalho\n");
        return 1;
    }
    const char *pathX = argv[1];
    const char *pathC = argv[2];
    int num_threads = (argc>3)? atoi(argv[3]) : 4;
    int max_iter = (argc>4)? atoi(argv[4]) : 100;
    double eps   = (argc>5)? atof(argv[5]) : 1e-6;
    const char *outAssign   = (argc>6)? argv[6] : NULL;
    const char *outCentroid = (argc>7)? argv[7] : NULL;
    const char *outSSE      = (argc>8)? argv[8] : NULL;
    
    if(max_iter <= 0 || eps <= 0.0){
        fprintf(stderr,"Parâmetros inválidos: max_iter>0 e eps>0\n");
        return 1;
    }
    
    // Configurar número de threads OpenMP
    omp_set_num_threads(num_threads);
    
    int N=0, K=0;
    double *X = read_csv_1col(pathX, &N);
    double *C = read_csv_1col(pathC, &K);
    int *assign = (int*)malloc((size_t)N * sizeof(int));
    if(!assign){ fprintf(stderr,"Sem memoria para assign\n"); free(X); free(C); exit(1); }
    
    double t0 = omp_get_wtime();
    int iters = 0; double sse = 0.0;
    kmeans_1d(X, C, assign, N, K, max_iter, eps, &iters, &sse, outSSE);
    double t1 = omp_get_wtime();
    
    double ms = (t1 - t0) * 1000.0;
    
    printf("K-means 1D (OpenMP - Critical)\n");
    printf("N=%d K=%d max_iter=%d eps=%g threads=%d\n", N, K, max_iter, eps, num_threads);
    printf("Iterações: %d | SSE final: %.6f | Tempo: %.1f ms\n", iters, sse, ms);
    
    write_assign_csv(outAssign, assign, N);
    write_centroids_csv(outCentroid, C, K);
    
    free(assign); free(X); free(C);
    return 0;
}
