#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

// numero de correntes de elevadores, numero de elevadores por corrente, respectivamente
#define ELEVATORCHAINS 3
#define ELEVATORPERCHAIN 5

// numero de andares
#define FLOORS 300

// numero de casas aceitas pelos numerais do programa
#define NUMIMPUTSIZE 5

// define as direções do elevador
#define UP 1
#define DOWN -1
#define NONE 0

/*Enum para guardar a cor do fundo*/
typedef enum FG_COLORS {
    FG_BLACK = 0,
    FG_BLUE = 1,
    FG_GREEN = 2,
    FG_CYAN = 3,
    FG_RED = 4,
    FG_MAGENTA = 5,
    FG_BROWN = 6,
    FG_LIGHTGRAY = 7,
    FG_GRAY = 8,
    FG_LIGHTBLUE = 9,
    FG_LIGHTGREEN = 10,
    FG_LIGHTCYAN = 11,
    FG_LIGHTRED = 12,
    FG_LIGHTMAGENTA = 13,
    FG_YELLOW = 14,
    FG_WHITE = 15
} FG_COLORS;

typedef enum BG_COLORS {
    BG_NAVYBLUE = 16,
    BG_GREEN = 32,
    BG_TEAL = 48,
    BG_MAROON = 64,
    BG_PURPLE = 80,
    BG_OLIVE = 96,
    BG_SILVER = 112,
    BG_GRAY = 128,
    BG_BLUE = 144,
    BG_LIME = 160,
    BG_CYAN = 176,
    BG_RED = 192,
    BG_MAGENTA = 208,
    BG_YELLOW = 224,
    BG_WHITE = 240
} BG_COLORS;

typedef struct distance {
    int chain;
    int elevatorNum;
    int distance;
    struct distance *next;
} Distance;

typedef struct list {
    int floorToStop;
    bool boardingFloor;
    struct list *next;
} List;

typedef struct {
    bool inUse;
    bool fetching;
    int originalStop;
    int direction;
    int currentFloor;
    List *lastFloor;
    List *listOfStops;
} Elevator;

void sleep (float seconds) {
    #ifdef _WIN32
        Sleep(seconds * 1000);
    #else
        usleep(seconds * 1000000);
    #endif
}

void dealloc (Elevator elevatorBank[ELEVATORCHAINS][ELEVATORPERCHAIN]) {
    for (int i = 0; i < ELEVATORCHAINS; i++) {
        for (int j = 0; j < ELEVATORPERCHAIN; j++) {
            List *aux = elevatorBank[i][j].listOfStops, *previous;
            while (aux) {
                previous = aux;
                aux = aux->next;
                free(previous);
            }
        }
    }
}

void enqueue (Elevator *elevator, int current, int destiny) {
    List *head = elevator->listOfStops;
    List *last = elevator->lastFloor;

    List *currentFloor = malloc(sizeof(List));
    currentFloor->floorToStop = current;
    currentFloor->boardingFloor = true;
    currentFloor->next = NULL;

    List *destinyFloor = malloc(sizeof(List));
    destinyFloor->floorToStop = destiny;
    destinyFloor->boardingFloor = false;
    destinyFloor->next = NULL;

    List *aux = head, *previous = NULL;
    if (head == NULL) { // não existe paradas
        head = currentFloor;
        last = currentFloor;
    } else {
        if (elevator->direction == UP) { // acha o local de inserção
            while (aux && aux->floorToStop < current) {
                previous = aux;
                aux = aux->next;
            }
        } else { 
            while (aux && aux->floorToStop > current) {
                previous = aux;
                aux = aux->next;
            }
        }

        if (previous == NULL) { // começo da lista
            currentFloor->next = head;
            head = currentFloor;
        } else {
            if (aux) // caso de: 2 5 [7] 8 9
                currentFloor->next = aux;
            previous->next = currentFloor; // caso anterior e de final da lista
        }
    }

    aux = head, previous = NULL;
    if (elevator->direction == UP) { // acha o local de inserção
        while (aux && aux->floorToStop < destiny) {
            previous = aux;
            aux = aux->next;
        }
    } else { 
        while (aux && aux->floorToStop > destiny) {
            previous = aux;
            aux = aux->next;
        }
    }

    if (previous == NULL) { // começo da lista
        destinyFloor->next = head;
        head = destinyFloor;
    } else {
        if (aux) // caso de: 2 5 [7] 8 9
            destinyFloor->next = aux;
        previous->next = destinyFloor; // caso anterior e de final da lista
    }

    // atualiza (se necessario) a ultima parada
    while (last->next)
        last = last->next;

    elevator->listOfStops = head;
    elevator->lastFloor = last;
}

void dequeue (Elevator *elevator) {
    if (elevator->currentFloor == elevator->lastFloor->floorToStop) {
        free(elevator->listOfStops);
        free(elevator->lastFloor);

        elevator->inUse = false;
        elevator->direction = NONE;
        elevator->lastFloor = NULL;
        elevator->listOfStops = NULL;
    } else {
        List *temp = elevator->listOfStops;
        elevator->listOfStops = elevator->listOfStops->next;
        free(temp);
    }
}

void moveElevators (Elevator elevatorBank[ELEVATORCHAINS][ELEVATORPERCHAIN]) {
    for (int i = 0; i < ELEVATORCHAINS; i++) {
        for (int j = 0; j < ELEVATORPERCHAIN; j++) {
            Elevator *elevator = &elevatorBank[i][j];

            if (elevator->inUse) {
                if (elevator->currentFloor == elevator->listOfStops->floorToStop) { // atenção ao acesso de um ponteiro nulo
                    dequeue(elevator);
                    if (elevator->fetching) {
                        elevator->fetching = false;
                        elevator->direction = ((elevator->currentFloor - elevator->originalStop/*elevator->lastFloor->floorToStop*/ > 0) ? DOWN : UP);
                    }
                } else {
                    elevator->currentFloor += elevator->direction;
                }
            }
        }
    }
}

void showElevators (Elevator elevatorBank[ELEVATORCHAINS][ELEVATORPERCHAIN]) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int j = ELEVATORPERCHAIN - 1; j >= 0; j--) {
        for (int i = 0; i < ELEVATORCHAINS; i++) {
            Elevator elevator = elevatorBank[i][j];

            if (elevator.inUse)
                SetConsoleTextAttribute(hConsole, FG_CYAN | FOREGROUND_INTENSITY);
            else
                SetConsoleTextAttribute(hConsole, FG_YELLOW | FOREGROUND_INTENSITY);

            printf("[%d]", (j % 5) + 1);

            if (elevator.inUse)
                if (elevator.listOfStops != NULL && elevator.currentFloor == elevator.listOfStops->floorToStop)
                    printf(" %s: %.3d", elevator.listOfStops->boardingFloor ? "Embarque" : "Desembarque", elevator.currentFloor);
                else 
                    printf(" %.3d -> %.3d", elevator.currentFloor, elevator.lastFloor->floorToStop); // ficar de olho nessa linha
            else
                printf(" Parado: %.3d", elevator.currentFloor);

            printf("\t\t");

            SetConsoleTextAttribute(hConsole, FG_WHITE);
        }

        printf("\n");
    }
}

void initElevatorBank (Elevator elevatorBank[ELEVATORCHAINS][ELEVATORPERCHAIN]) {
    int gap = FLOORS / ELEVATORPERCHAIN;

    for (int i = 0; i < ELEVATORCHAINS; i++) {
        int initialPosition = gap / 2;

        for (int j = 0; j < ELEVATORPERCHAIN; j++) {
            elevatorBank[i][j].inUse = false;
            elevatorBank[i][j].fetching = false;
            elevatorBank[i][j].originalStop = NONE;
            elevatorBank[i][j].direction = 0;
            elevatorBank[i][j].currentFloor = initialPosition;
            elevatorBank[i][j].lastFloor = NULL;
            elevatorBank[i][j].listOfStops = NULL;

            initialPosition += gap;
        }
    }
}

void clearStdin () {
    char ch;

    while (ch = getchar())
        if (ch == '\n')
            break;
}

bool isNumber (char *input) {
    int i = 0;
    
    if (input[0] == 0)
        return false;

    while (input[i])
        if (!isdigit(input[i++]))
            return false;
    
    return true;
}

int strToInt (char *num) {
    int temp = 0, i = 0;

    while (num[i] != '\0')
        temp = temp * 10 + (num[i++] - '0');

    return temp;
}

int getNum (char *text) {
    bool isValid = false;
    char input[NUMIMPUTSIZE];

    while (!isValid) {
        int i = 0;
        char ch;

        printf("\n%s", text);

        while (scanf("%c", &ch)) { 
            if (ch == '\n')
                break;

            if (i >= NUMIMPUTSIZE - 1) {
                clearStdin();
                i = 0;
                break;
            }

            input[i++] = ch;
        }

        input[i] = '\0';
    
        isValid = isNumber(input);
    }

    return strToInt(input);
}

int getNumInRange (char *text, int min, int max) {
    int floor = -1;
    
    while (floor < min || floor > max)
        floor = getNum(text);

    return floor;
}

void request (Elevator elevatorBank[ELEVATORCHAINS][ELEVATORPERCHAIN], int requestFloor, int requestDestiny) {
    Distance *list = NULL;

    // calcula a distancia da chamada para todos os elevadores e os armazena em uma lista encadeada
    for (int i = 0; i < ELEVATORCHAINS; i++) {
        for (int j = 0; j < ELEVATORPERCHAIN; j++) {
            Distance *newDistance = malloc(sizeof(Distance));
            newDistance->chain = i;
            newDistance->elevatorNum = j;
            newDistance->distance = abs(elevatorBank[i][j].currentFloor - requestFloor);
            newDistance->next = NULL;

            Distance *aux = list, *previous = NULL;

            while (aux && aux->distance < newDistance->distance) {
                previous = aux;
                aux = aux->next;
            }

            if (previous == NULL) {
                newDistance->next = list;
                list = newDistance;
            } else {
                if (aux)
                    newDistance->next = aux;
                previous->next = newDistance;
            }
        }
    }

    Distance *aux = list, *previous;

    // seleciona o elevador que irá atender a chamada e configura o movimento do elevador
    while (aux) {
        Elevator *elevator = &elevatorBank[aux->chain][aux->elevatorNum];

        if (elevator->inUse) {
            if (elevator->direction == UP) {
                if (elevator->currentFloor < requestFloor) {
                    enqueue(elevator, requestFloor, requestDestiny);
                    break;
                }
            } else if (elevator->direction == DOWN) {
                if (elevator->currentFloor > requestFloor)
                    enqueue(elevator, requestFloor, requestDestiny);
                    break;
            } else {
                continue;
            }
        } else {
                elevator->inUse = true;

    /* linear */if ((elevator->currentFloor > requestFloor && requestFloor > requestDestiny) || (elevator->currentFloor < requestFloor && requestFloor < requestDestiny)) {

                    elevator->fetching = true;
                    elevator->originalStop = requestDestiny;
                    elevator->direction = ((elevator->currentFloor - requestFloor > 0) ? DOWN : UP);
                    enqueue(elevator, requestFloor, requestDestiny);
    /*n linear*/} else {
                    elevator->fetching = true;
                    elevator->originalStop = requestDestiny;
                    elevator->direction = (!(requestFloor > elevator->currentFloor) ? UP: DOWN);
                    enqueue(elevator, requestFloor, requestDestiny);
                    elevator->direction = ((requestFloor > elevator->currentFloor) ? UP: DOWN);
                }

                break;
            }

        aux = aux->next;
    }

    // desaloca a lista de distancias
        aux = list;
        while (aux) {
            previous = aux;
            aux = aux->next;
            free(previous);
        }
}


int main (void) {
    Elevator elevatorBank[ELEVATORCHAINS][ELEVATORPERCHAIN];
    initElevatorBank(elevatorBank);

    int loops = getNum("Entre o numero de iteracoes: ");
    int skips = 0;
    bool randomInput = false, exit = false;

    while (loops > 0 && !exit) {
        int currentFloor, destinyFloor;
        
        int choice = -1;

        if (!skips) {
            randomInput = false;

            while (choice < 0 || choice > 4) {
                system("cls || clear");
                showElevators(elevatorBank);

                printf("\nIteracoes restantes: %d\n", loops);

                printf ("\n1. Adicionar uma iteracao manualmente"
                        "\n2. Pular iteracao"
                        "\n3. Pular N iteracoes"
                        "\n4. Randomizar N iteracoes"
                        "\n0. Sair\n");

                choice = getNum("");
            }
            
            if (choice == 1) {
                currentFloor = getNumInRange("Digite o andar que voce esta: ", 1, FLOORS);
                destinyFloor = getNumInRange("Digite o andar de destino: ", 1, FLOORS);
                request(elevatorBank, currentFloor, destinyFloor);
            } else if (choice == 3) {
                skips = getNumInRange("Digite o numero de interacoes que deseja pular: ", 1, loops);
            } else if (choice == 4) {
                skips = getNumInRange("Digite o numero de interacoes que deseja randomizar: ", 1, loops);
                randomInput = true;
            } else if (choice == 0) {
                exit = true;
            }
        }
        
        if (skips) {
            system("cls || clear");
            showElevators(elevatorBank);

            if (randomInput) { // adicionar randomizadores
                srand(time(NULL));
                int doResquest = (rand() % 100) > 60;

                if (doResquest) {
                    currentFloor = (rand() % FLOORS) + 1;
                    destinyFloor = (rand() % FLOORS) + 1;
                    printf ("\n--- Request automatica"
                            "\n--- Chamada: %d" 
                            "\n--- Destino: %d\n"
                            , currentFloor, destinyFloor);
                    request(elevatorBank, currentFloor, destinyFloor);
                } else {
                    printf("\n--- Nao houve request\n");
                }
                
                sleep(1.25);
            }

            skips--;
        }

        moveElevators(elevatorBank);

        loops--;
    }

    dealloc(elevatorBank);

    return 0;
}