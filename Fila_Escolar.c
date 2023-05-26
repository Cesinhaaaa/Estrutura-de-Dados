#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define QUEUESIZE 30
#define NAMESIZE 50
#define NUMSIZE 4
#define ALPHABETSIZE 26
#define ROWSIZE 10 // numero de nomes por cada linha do print

#define GAP "\0"
#define LEFT -1
#define RIGHT 1

typedef struct {
    char name[NAMESIZE];
} STR;

bool clearStdin (void) {
    bool clear = false;
    char c;

    while (c = getchar())
        if (c == '\n')
            break;
        else
            clear = true;

    return clear;
}

int isGap (char *name) {
    return !strcmp(name, GAP);
}

int isUper (char c) {
    return c >= 'A' && c <= 'Z';
}

int isNum (char c) {
    return c >= '0' && c <= '9';
}

void swap (STR *i1, STR *i2) {
    STR temp = *i1;
    *i1 = *i2;
    *i2 = temp;
}

int findLastGap(STR *queue, int i, int *gaps, int increment) { // procura o ultimo gap a direita ou a esquerda, e conta os N gaps
    bool expectedGap = true;
    int lastGap = -1;

    for (int init = i; init >= 0 && init < QUEUESIZE; init += increment) {
        if (expectedGap) {
            if (isGap(queue[init].name)) {
                (*gaps)++;
                expectedGap = false;
                lastGap = init;
            }
        } else {
            if (!isGap(queue[init].name))
                break;
            else {
                (*gaps)++;
                lastGap = init;
            }
        }
    }
    
    return lastGap;
}

int occurrenceAccount (STR *queue, int i, char ch) {
    int count = 0;

    while (i < QUEUESIZE) {
        if (queue[i].name[0] == ch)
            count++;
        else
            break;

        i++;
    }

    return count;
}

int findInsertionIdxWithGaps (STR *queue, int i, int increment) {
    int insertionIDX = -1;
    int gaps = 0;
    int init = i;
    char ch = queue[i].name[0];

    for (; i >= 0 && i < QUEUESIZE; i += increment)
        if (isGap(queue[i].name))
            gaps++;
        else if (queue[i].name[0] == ch)
            init = i;
        else if (queue[i].name[0] != ch)
            break;

    if (gaps) {// ha espaço para insercao em casos como: inserir Clara em -> GAP Ana Caio Maria GAP GAP
        if (increment == RIGHT)
            insertionIDX = (init + (init + gaps)) / 2;
        else if (increment == LEFT)
            insertionIDX = (init + (init - gaps)) / 2;

        return insertionIDX;
    }

    return -1; // não ha espaco para insercao em casos como: inserir Clara em -> GAP Ana Caio Maria GAP GAP
}

int findInsertionIdxNoGaps (STR *queue, int insertionIDX) {
    char ch = queue[insertionIDX].name[0];

    while (insertionIDX < QUEUESIZE) {
        if (insertionIDX == QUEUESIZE - 1) {
            if (!isGap(queue[insertionIDX].name)) {
                insertionIDX = -1;
                break;
            }
            break;
        } else if (queue[insertionIDX].name[0] != ch) {
            if (!isGap(queue[insertionIDX].name))
                insertionIDX = -1;
                
            break;
        }

        insertionIDX++;
    }

    return insertionIDX;
}

int findIdxIndicator (int *indexIndicator, char ch) {
    int idx = ch % 'A';

    if (indexIndicator[idx] != -1)
        return indexIndicator[idx];
    
    int i = idx, j = idx;

    for (; i >= 0 || j < ALPHABETSIZE; i--, j++) {
        if (i >= 0)
            if (indexIndicator[i] != -1)
                return indexIndicator[i];

        if (j < ALPHABETSIZE)
            if (indexIndicator[j] != -1)
                return indexIndicator[j];
    }

    return -1; // não existe nenhum valor na lista
}

void show (STR *queue) {
    for (int i = 0; i < QUEUESIZE; i++) {
        printf("%s\n", isGap(queue[i].name) ? "~" : queue[i].name);

        if (i)
            if ((i + 1) % ROWSIZE == 0)
                printf("\n");
    }
}

int setPrecedence (char *s1, char *s2, int i) {
    if (s1[i] > s2[i])
        return 1;
    else if (s1[i] == s2[i])
        return setPrecedence(s1, s2, i + 1);

    return 0;
}

void sortSubQueue (STR *subQueue, int init, int end) {
    while (end != init) {
        if (setPrecedence(subQueue[end - 1].name, subQueue[end].name, 0)) {
            swap(&subQueue[end - 1], &subQueue[end]);
            end--;
        } else break;
    }
}

bool validateName (char *name) {
    bool expectedUper = true;
    
    if (name[0] == '\0')
        return false;

    for (int i = 0; name[i] != '\0'; i++)
        if (!isNum(name[i])) {
            if (name[i] == ' ')
                expectedUper = true;
            else if (expectedUper)
                if (isUper(name[i]))
                    expectedUper = false;
                else
                    return false;
            else if (isUper(name[i]))
                return false;
        } else
            return false;

    return true;
}

void getName (char *name, char *strText) {
    bool valid = false;
    
    while (!valid) {
        printf("\nDigite o nome a ser %s: ", strText);
        
        int i = 0;
        char c;
        
        while (scanf("%c", &c)) {
            if (c == '\n') break;
            
            if (i >= NAMESIZE - 1) {
                clearStdin();
                i = 0;
                break;
            }
            
            name[i++] = c;
        }
        
        name[i] = '\0';

        if (validateName(name))
            valid = 1;
    }
}

void shiftRight (STR *queue, int init, int end, int shift, int *indexIndicator) {
    char lastChar = '~';

    while (end - shift >= init) {
        if (end - shift < init)
            break;

        if (lastChar != queue[end - shift].name[0]) {
            if (queue[end - shift].name[0] != '\0') {
                lastChar = queue[end - shift].name[0];
                indexIndicator[lastChar % 'A'] += shift;
            }
        }

        swap(&queue[end], &queue[end - shift]);

        end--;
    }
}

void shiftLeft (STR *queue, int init, int end, int shift, int *indexIndicator) { 
    char lastChar = '~';

    while (init + shift <= end) {
        if (init + shift > end) 
            break;

        if (lastChar != queue[init + shift].name[0]) {
            if (queue[init + shift].name[0] != '\0') {
                lastChar = queue[init + shift].name[0];
                indexIndicator[lastChar % 'A'] -= shift;
            }
        }

        swap(&queue[init], &queue[init + shift]);

        init++;
    }
}

void enqueue (STR *queue, int *indexIndicator) {
    char name[NAMESIZE];
    getName(name, "inserido");

    char firstChar = name[0];
    int idx = findIdxIndicator(indexIndicator, firstChar);
    char queueChar = queue[idx].name[0];
    
    if (idx == -1) { // nenhum nome na lista
        strcpy(queue[QUEUESIZE / 2].name, name);
        indexIndicator[firstChar % 'A'] = QUEUESIZE / 2;
////////////////////////////////////////////////////////////////////////////////////////////////
    } else if (queueChar != firstChar) { // nao existe um nome que inicie com a mesma letra, insere com gaps
        int insertionIDX = -1;

        if (queueChar > firstChar) // procura até aparecer um nome que não é um gap
            insertionIDX = findInsertionIdxWithGaps(queue, idx, LEFT);
        else
            insertionIDX = findInsertionIdxWithGaps(queue, idx, RIGHT);

        if (insertionIDX != -1) { // ha espaco para insercao
            strcpy(queue[insertionIDX].name, name);
            indexIndicator[firstChar % 'A'] = insertionIDX;
        } else { // nao ha espaco para insercao, shift necessario
            if (queueChar < firstChar) {    
                int newIdx = idx + occurrenceAccount(queue, idx, queueChar); // pega a ultima letra da letra mais proxima, e vai até o fim dela

                int gaps = 0;
                int init, end, shift;
                int lastGap = findLastGap(queue, newIdx, &gaps, RIGHT); // acha os gaps

                if (lastGap != -1) {
                    init = newIdx; // começa no fim da letra de referencia
                    end = lastGap - (gaps / 2);
                    gaps = gaps / 2;
                    shift = gaps ? gaps : 1;

                    shiftRight(queue, init, end, shift, indexIndicator);
                    insertionIDX = findInsertionIdxWithGaps(queue, idx, RIGHT); // insere baseado nos gaps

                    strcpy(queue[insertionIDX].name, name);
                } else {
                    newIdx = newIdx - 1;

                    gaps = 0;
                    lastGap = findLastGap(queue, newIdx, &gaps, LEFT); // procura o gap a partir do ultimo nome da referencia
                        
                    if (lastGap != -1) {
                        end = newIdx - 1;
                        init = lastGap + (gaps / 2);
                        gaps = gaps / 2;
                        shift = gaps ? gaps : 1;

                        shiftLeft(queue, init, end, shift, indexIndicator);
                        idx = indexIndicator[queueChar % 'A'];
                        insertionIDX = findInsertionIdxWithGaps(queue, idx, RIGHT);

                        strcpy(queue[insertionIDX].name, name);
                    } else {
                        printf("\n\n--- A fila esta cheia ---\n\n");
                        getchar();

                    }
                }
            } else { // queue char > first char
                int newIdx = idx; // pega o primeiro nome

                int gaps = 0;
                int init, end, shift;
                int lastGap = findLastGap(queue, newIdx, &gaps, RIGHT); // acha os gaps

                if (lastGap != -1) {
                    init = newIdx; // começa no fim da letra de referencia
                    end = lastGap - (gaps / 2);
                    gaps = gaps / 2;
                    shift = gaps ? gaps : 1;

                    shiftRight(queue, init, end, shift, indexIndicator);
                    idx = indexIndicator[queueChar % 'A'];
                    insertionIDX = findInsertionIdxWithGaps(queue, idx, LEFT); // insere baseado nos gaps

                    strcpy(queue[insertionIDX].name, name);
                } else {
                    newIdx = idx - 1;

                    gaps = 0;
                    lastGap = findLastGap(queue, newIdx, &gaps, LEFT); // procura o gap a partir do ultimo nome da referencia
                        
                    if (lastGap != -1) {
                        end = newIdx - 1;
                        init = lastGap + (gaps / 2);
                        gaps = gaps / 2;
                        shift = gaps ? gaps : 1;

                        shiftLeft(queue, init, end, shift, indexIndicator);
                        insertionIDX = findInsertionIdxWithGaps(queue, idx, LEFT);

                        strcpy(queue[insertionIDX].name, name);
                    } else {
                        printf("\n\n--- A fila esta cheia ---\n\n");
                        getchar();
                    }
                }
            }
////////////////////////////////////////////////////////////////////////////////////////////////
        }

    } else { // exite um nome que inicia com a mesma letra, insere em sequencia
        int insertionIDX = indexIndicator[firstChar % 'A'];

        insertionIDX = findInsertionIdxNoGaps(queue, insertionIDX);

        if (insertionIDX != -1) { // shift não necessario
            strcpy(queue[insertionIDX].name, name);
            sortSubQueue(queue, idx, insertionIDX);
        } else { // shift necessario
            int gaps = 0;
            int init, end, shift;
            int lastGap = findLastGap(queue, idx, &gaps, RIGHT);

            if (lastGap != -1) { // existem gaps a direita para o shift
                init = idx + occurrenceAccount(queue, idx, queueChar);
                end = lastGap - (gaps / 2);
                gaps = gaps / 2;
                shift = gaps ? gaps : 1;
                
                shiftRight(queue, init, end, shift, indexIndicator);
                insertionIDX = findInsertionIdxNoGaps(queue, idx);

                strcpy(queue[insertionIDX].name, name);
                sortSubQueue(queue, idx, insertionIDX);

            } else { // nao existem gaps a direita para o shift
                gaps = 0;
                lastGap = findLastGap(queue, idx, &gaps, LEFT);

                if (lastGap != -1) {
                    end = idx + occurrenceAccount(queue, idx, queueChar) - 1;
                    init = lastGap + (gaps / 2);
                    gaps = gaps / 2;
                    shift = gaps ? gaps : 1;

                    shiftLeft(queue, init, end, shift, indexIndicator);
                    insertionIDX = indexIndicator[firstChar % 'A'];

                    int auxIDX = insertionIDX;
                    insertionIDX = findInsertionIdxNoGaps(queue, insertionIDX);

                    strcpy(queue[insertionIDX].name, name);
                    sortSubQueue(queue, auxIDX, insertionIDX);
                } else {
                    printf("\n\n--- A fila esta cheia ---\n\n");
                    getchar();
                }
            }
        }
    }
}

int strToInt (char *num) {
    int temp = 0, i = 0;

    while (num[i] != '\0')
        temp = temp * 10 + (num[i++] - '0');

    return temp;
}

int validateNum (char *num) {
    if (num[0] == '\0')
        return false;

    for (int i = 0; num[i] != '\0'; i++)
        if (!isNum(num[i]))
            return false;
    
    return true;
}

int getNum (void) {
    char num[NUMSIZE];
    bool valid = false;
    
    printf("\nObs1: Ao digitar um numero de pessoas maior que a quantidade de pessoas existentes na fila, eh parada a remocao");
    printf("\nObs2: Se nao houver ninguem na fila, nao ha alteracao na fila");
    printf("\nObs3: Numero maximo de %d digitos para a desenfileiracao", NUMSIZE - 1);

    while (!valid) {
        printf("\n\nDigite o numero de pessoas que devem sair da fila: ");
        
        int i = 0;
        char c;
        
        while (scanf("%c", &c)) {
            if (c == '\n') break;
            
            if (i >= NUMSIZE - 1) {
                clearStdin();
                i = 0;
                break;
            }
                
            num[i++] = c;
        }
            
        num[i] = '\0';

        if (validateNum(num))
            valid = true;
    }

    return strToInt(num);
}

void dequeue (STR *queue, int *indexIndicator) {
    int num = getNum();

    while (num > 0) {
        int idx = 0;

        while (idx < ALPHABETSIZE) {
            int i = indexIndicator[idx];

            if (i != -1) {
                char ch = queue[i].name[0];
                strcpy(queue[i].name, GAP);

                if (i < QUEUESIZE - 1) { // analisa até o penultimo indice do vetor, para evitar acesso a memoria nao existente
                    if (queue[i + 1].name[0] == ch)
                        indexIndicator[idx] += 1;
                    else
                        indexIndicator[idx] = -1;
                } else
                    indexIndicator[idx] = -1;
            
                break; // quando for feita a remoção, para o laço interno
            }

            idx++;
        }

        if (idx == ALPHABETSIZE - 1) // caso da lista verificada vazia
            if (indexIndicator[idx] == -1)
                break;

        num--;
    }
}

void dequeueSelected (STR *queue, int *indexIndicator) {
    char name[NAMESIZE];
    getName(name, "removido");

    int idx = name[0] % 'A';
    int i = indexIndicator[idx], removeIDX = -1;
    int init = i;
    char ch = name[0];

    while (queue[i].name[0] == ch) {
        if (strcmp(queue[i].name, name) == 0) {
            removeIDX = i;
            break;
        }

        i++;
    }

    if (removeIDX == -1) {
        printf("\nEste nome nao pertence a lista\n");
        getchar();
    } else if (removeIDX == init) {
        strcpy(queue[removeIDX].name, GAP);

        if (removeIDX < QUEUESIZE - 1) { // analisa até o penultimo indice do vetor, para evitar acesso a memoria nao existente
            if (queue[removeIDX + 1].name[0] == ch)
                indexIndicator[idx] += 1;
            else
                indexIndicator[idx] = -1;
        } else
            indexIndicator[idx] = -1;
    } else {
        while (removeIDX < QUEUESIZE) {
            if (removeIDX == QUEUESIZE - 1) {
                strcpy(queue[removeIDX].name, GAP);
                break;
            }

            if (queue[removeIDX + 1].name[0] == ch) {
                swap(&queue[removeIDX + 1], &queue[removeIDX]);
            } else {
                strcpy(queue[removeIDX].name, GAP);
                break;
            }
            
            removeIDX++;
        }
    }
}

int main (void) {
    STR queue[QUEUESIZE];
    int indexIndicator[ALPHABETSIZE];

    memset(indexIndicator, -1, sizeof(indexIndicator)); // inicializa todos os indices do vetor com -1
    memset(queue, '\0', sizeof(queue)); // inicializa todos os indices da fila com o GAP ( '\0' )

    bool exit = false;

    while (!exit) {
        system("clear");

        char choice = ' ';

        show(queue);

        printf ("\n\n"
                "1. Enfileirar\n"
                "2. Desenfileirar\n"
                "3. Remover\n"
                "0. Exit\n");

        scanf(" %c", &choice);

        bool clear;
        clear = clearStdin();

        if (isNum(choice)) {
            if (clear)
                choice = ' ';
        } else {
            choice = ' ';
        }

        switch (choice) {
            case '1':
                enqueue(queue, indexIndicator);
                break;
            case '2':
                dequeue(queue, indexIndicator);
                break;
            case '3':
                dequeueSelected(queue, indexIndicator);
                break;
            case '0':
                exit = true;
                break;
            default:
                choice = ' ';
                break;
        }
    }

    return 0;
}
