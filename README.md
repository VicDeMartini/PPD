# Investigação da escalabilidade em multiplicação de duas matrizes 



### Aluna

*   Victoria de Martini de Souza -  759378

## Geral

*   Todos os testes foram feitos com matrizes quadradas de 1000 linhas por 1000 colunas

### Características do processador utilizado

*   Model name:                      Intel(R) Core(TM) i7-10510U CPU @ 1.80GHz
*   CPU(s):                          12
*   On-line CPU(s) list:             0-7
*   Thread(s) per núcleo:            2
*   Núcleos(s) per socket:           4
*   Soquetes (s):                    1
*   NUMA node(s):                    1
*   Ordem dos bytes:                 Little Endian
*   L1d cache:                       128 KiB
*   L1i cache:                       128 KiB
*   L2 cache:                        1 MiB
*   L3 cache:                        8 MiB

## Testes

### Loop externo

*   Usando a paralelização no loop externo temos o seguinte código:
```c
    #pragma omp parallel for
    for (i = 0; i < lin_c; i++)             // loop externo
        for (j = 0; j < col_c; j++){        // loop intermediario
            soma = 0;
            for (k = 0; k < col_a; k++)     // loop interno
                soma += A[i * col_a + k] * B[k * col_b + j];
            C[i * col_c+j] = soma;   
        }
```
*   Neste caso as iterações de 'i' serão divididas entre as 12 threads que vão ser criadas, devido ao valor default para o processador utilizado, veremos mais sobre adiante
*   Obtemos o tempo de execução do bloco em 10.8s como observado abaixo:

```bash
magalu@vic-labs ~/ppd (master)> ./escalabilidade
Linhas A: 1000
Colunas A / Linhas B: 1000
Colunas B: 1000

Total time = 10.898156 seconds
```

### Loop intermediario

*   Realizando a paralelização no loop intermediario obtemos o seguinte código:
```c
    for (i = 0; i < lin_c; i++)             // loop externo
        #pragma omp parallel for
        for (j = 0; j < col_c; j++){        // loop intermediario
            soma = 0;
            for (k = 0; k < col_a; k++)     // loop interno
                soma += A[i * col_a + k] * B[k * col_b + j];
            C[i * col_c+j] = soma;   
        }
```

*   Aqui as iterações de 'j' serçao divididas entre as 12 threads como dito anteriormente
*   Resultando em um tempo de ~6s como pode ser observado abaixo:

```bash
magalu@vic-labs ~/ppd (master)> ./escalabilidade
Linhas A: 1000
Colunas A / Linhas B: 1000
Colunas B: 1000

Total time = 6.100534 seconds
```

### Loop interno

*   Realizando a paralelização no loop interno obtemos o seguinte código:
```c
    for (i = 0; i < lin_c; i++)             // loop externo
        for (j = 0; j < col_c; j++){        // loop intermediario
            soma = 0;
            #pragma omp parallel for
            for (k = 0; k < col_a; k++)     // loop interno
                soma += A[i * col_a + k] * B[k * col_b + j];
            C[i * col_c+j] = soma;   
        }
```

*   Aqui as iterações de 'k' serão divididas entre as 12 threads 
*   Resultando em um tempo de ~4s como pode ser observado abaixo:

```bash
magalu@vic-labs ~/ppd (master)> ./escalabilidade
Linhas A: 1000
Colunas A / Linhas B: 1000
Colunas B: 1000

Total time = 4.506828 seconds
```

### Definindo número de threads

*   Como foi comentado acima o número de threads utilizadas foi o padrão definido pela definição do "#pragma", que por outro lado utiliza todos os cores disponíveis no processador, por padrão. 
Porém, utilizando o argumento "num_threads(x)" podemos definir um número 'x' de threads que irão ser utilizadas na paralelização do nosso bloco. Podendo entretanto afetar o desempenho obtido, e é esse nosso objetivo nos testes abaixo.

*   Levando em conta os resultados obtidos, vamos realizar os testes de número de threads no loop interno do nosso programa (o que obteve o melhor desempenho) e observaremos como o desempenho da aplicação é afetado.

#### 6 Threads

*   O código utilizando o argumento "num_threads()", para definirmos o número de threads desejadas, fica da seguinte maneira no loop externo:
```c
    for (i = 0; i < lin_c; i++)             // loop externo
        for (j = 0; j < col_c; j++){        // loop intermediario
            soma = 0;
            #pragma omp parallel for num_threads(6)
            for (k = 0; k < col_a; k++)     // loop interno
                soma += A[i * col_a + k] * B[k * col_b + j];
            C[i * col_c+j] = soma;   
        }
```

*   Até agora utilizamos 12 threads (default do processador) porém cortando esse número pela metade obtemos um resultado muito parecido de ~5s. Isso se deve pelo fato de as iterações foram divididas pelas 6 threads normalmente e a complexidade do cálculo, nem dos dados, não ser muito grande. 

```bash
magalu@vic-labs ~/ppd (master)> ./escalabilidade
Linhas A: 1000
Colunas A / Linhas B: 1000
Colunas B: 1000

Total time = 4.958933 seconds
```

#### 24 Threads

*   Agora o código utilizando o mesmo argumento porém dobrando o número de threads, também no loop externo:
```c
    #pragma omp parallel for num_threads(24)
    for (i = 0; i < lin_c; i++)             // loop externo
        for (j = 0; j < col_c; j++){        // loop intermediario
            soma = 0;
            for (k = 0; k < col_a; k++)     // loop interno
                soma += A[i * col_a + k] * B[k * col_b + j];
            C[i * col_c+j] = soma;   
        }
```

*   Observando esse resultado podemos observar que dobrando o número de threads o desempenho do programa foi muito comprometido (para o caso de teste em questão). Neste teste obtemos ~58s de tempo de execução, que podem ser conferidos abaixo:

```bash
magalu@vic-labs ~/ppd (master)> ./escalabilidade
Linhas A: 1000
Colunas A / Linhas B: 1000
Colunas B: 1000

Total time = 57.911264 seconds
```

*   Isso se deve ao fato de que foi definido um número de threads maior que o número de cores disponíveis no processador (no caso 12). Dessa forma, as iterações do for são divididas entre as 24 threads, fazendo com que o número de cálculos por thread diminua, contudo, enquanto as 12 threads que o processador comporta estão em execução as outras 12 restantes estão em espera e por conta disso nota uma piora no desempenho apresentado, logo que demandará mais tempo até que todas as threads sejam executadas.

### Perguntas

#### Qual/quais loop(s) paralelizar? Vale a pena paralelizar todos?

*   Como visto acima, se formos paralelizar apenas um loop a melhor escolha é paralelizar o loop mais interno, onde obtivemos o menor tempo de execução.

#### Qual é o efeito de fazer um parallel for em cada um dos fors abaixo?

*    Iriamos notar uma otimização melhor, na paralelização, do que foi possível notar nesse relatório 

#### É necessários sincronizar alguma operação, garantindo exclusão mútua?

*   No código a seguir dado pelo professor

```c
  for(i=0; i < lin_c; i++) 
    for(j=0; j < col_c; j++) {
      C[i*col_c+j]=0;
      // #pragma omp parallel for <-- gera erros no resultado
      for(k=0; k < col_a; k++) 
        C[i*col_c+j] = C[i*col_c+j] + A[i*col_a+k] * B[k*col_b+j];
    }
```

*   É necessária a exclusão mútua do for mais interno, pois se houver a paralelização pode acontecer de mais de uma thread alterar o valor de C[i*col_c+j] ao mesmo tempo,
 isso ocorre pois dentro do for o valor das variáveis 'i' e 'j' são constantes. 