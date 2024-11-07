//
// Created by Tizifuchi12 on 6/11/2024.
//

//AGREGUE LA FUNCION STRCAT Y SU DECLARACION EN STRINGUTILS
//LIMITACIONES: 12 FILOSOFOS MAXIMO, NO SE PUEDE ELEGIR QUE FILOSOFO BORRAR NI AGREGAR, SIEMPRE ES EL ULTIMO EN (DES)USO DEL VECTOR
#include <philosophers.h>
#include <syscalls.h>
#include <iolib.h>
#include <stringutils.h>

#define MAX_PHILOSOPHERS 12
#define MIN_PHILOSOPHERS 3
#define MUTEX "mutex"
#define LEFT(i) ((i + cantPhilosophers - 1) % cantPhilosophers)
#define RIGHT(i) ((i + 1) % cantPhilosophers)

#define ADD 'a'
#define REMOVE 'r'
#define QUIT 'q'

typedef enum {
    NONE = 0,
    THINKING,
    HUNGRY,
    EATING
} state_t;

typedef struct {
    uint16_t pid; //LO GUARDO POR SI LO LLEGO A NECESITAR DESPUES
    state_t state;
    char * sem; //VER SI ES CHAR* O SEMAPHORE_T
} philosopher_t; //aca meto toda la info de ese filosofo, con esto no hace falta que cree los vectores que usa en la teorica

static int cantPhilosophers = 0;
static philosopher_t * philosophers[MAX_PHILOSOPHERS];

static int addPhilosopher(int i);
static int deletePhilo(int i );
static uint64_t philoActions(int argc, char * argv[]);
static void putForks(int i);
static void takeForks(int i);
static void showState();
static void test(int i);



void philosopherProgram(int argc, char * argv[]) {

    if(_sys_semOpen(MUTEX, 1) == -1) { //genero el semaforo general
        printf("Error opening mutex\n");
        return;
    }

    for(int i = 0; i < MAX_PHILOSOPHERS; i++){ //armo el lugar para el maximo posbile
        philosophers[i] = _sys_malloc(sizeof(philosopher_t)); //hacerle el free
        philosophers[i]->state = NONE;
        philosophers[i]->pid = -1; //es que todavia no hay filosofo en esa posicion
    }

    for(int i = 0; i < MIN_PHILOSOPHERS; i++){
        addPhilosopher(i);
    }

    char c;
    while((c = getchar()) != QUIT){
        switch(c){
            case ADD:
                addPhilosopher(cantPhilosophers);
                break;
            case REMOVE:
                deletePhilo(cantPhilosophers - 1);
                break;
        }
    }

    printf("problema de los filosofos terminado\n");

    for(int i = 0; i < MAX_PHILOSOPHERS; i++){ //libero todo
        if(i < cantPhilosophers){
            _sys_semClose(philosophers[i]->sem);
            _sys_kill(philosophers[i]->pid);
        }
        _sys_free(philosophers[i]);
    }

    cantPhilosophers = 0;
    _sys_semClose(MUTEX);
}

static int addPhilosopher(int i){
    if(cantPhilosophers == MAX_PHILOSOPHERS){
        printf("Max philosophers reached\n");
        return -1;
    }

    _sys_semWait(MUTEX);
    printf("Adding philosopher %d\n", i);

    //REVISAR ESTAS LINEAS DE ABAJO YA ME MAREE
    char philoName[20];
    char philoNumberBuffer[2];
    strcpy(philoName, "philosopher ");
    char* philoNumber = itoa(cantPhilosophers, philoNumberBuffer);
    strcat(philoName, philoNumber);

    _sys_semOpen(philoName, 1); //VER ESTE VALOR CUANDO HAGA EL PROGRAMA
    philosophers[i]->sem = philoName; //los semaforos de cada filosofo tienen el nombre del filosofo
    philosophers[i]->state = THINKING;

    /*char philoNumber[2];
    sprintf(philoNumber, "%d", i);*/
    char * argv[] = {philoNumber, NULL};
    philosophers[i]->pid = (int16_t) _sys_createProcess((mainFunction)&philoActions, argv, philoName, 1, 0); //VER EL CASTEO QUE TIRE AHI
    if(philosophers[i]->pid == -1){
        printf("Error creating philosopher\n");
        return -1;
    }
    cantPhilosophers++;

    showState();

    _sys_semPost(MUTEX);
    return 0;
}

//NO LIBERO EL ESPACIO DEL FILOSOFO PORQUE DESPPUES DE BORRAR PUEDO LLEGAR A QUERER HACER ADD
static int deletePhilo(int i){
    if(cantPhilosophers == MIN_PHILOSOPHERS){
        printf("Min philosophers reached\n");
        return -1;
    }

    _sys_semWait(MUTEX);
    while(philosophers[LEFT(i)]->state == EATING && philosophers[RIGHT(i)]->state == EATING){
        _sys_semPost(MUTEX); //hago que el resto siga con sus cosas
        _sys_semWait(philosophers[i]->sem); //aca espera a qu arranque a comer para eliminarse, en ese caso los del costado no vana estarr comiendo
        _sys_semWait(MUTEX); //procedo como si no hubiera entrado al while
    }

    _sys_semClose(philosophers[i]->sem);
    philosophers[i]->state = NONE; //pongo que no hay filosofo
    _sys_kill(philosophers[i]->pid);
    philosophers[i]->pid = -1; //marco como que ya no hay filosofo ahi, para que no quede el pid viejo pero no es necesario para el programa, se piede sacar
    cantPhilosophers--;

    showState();

    _sys_semPost(MUTEX);
    return 0;
}

static uint64_t philoActions(int argc, char * argv[]){
    int i = atoi(argv[0]);

    while(1){
        _sys_sleep(THINKING); //ver que valor poner aca
        takeForks(i);
        _sys_sleep(EATING); //ver que valor poner aca
        putForks(i);
    }
    return 0;
}


static void takeForks(int i){ //igual al de la presentaciion
    _sys_semWait(MUTEX);
    philosophers[i]->state = HUNGRY;
    test(i);
    _sys_semPost(MUTEX);
    _sys_semWait(philosophers[i]->sem);
}


static void putForks(int i){ //igual al de la presentacion
    _sys_semWait(MUTEX);
    philosophers[i]->state = THINKING;
    test(LEFT(i));
    test(RIGHT(i));
    _sys_semPost(MUTEX);
}


static void test(int i){
    if(philosophers[i]->state == HUNGRY && philosophers[LEFT(i)]->state != EATING && philosophers[RIGHT(i)]->state != EATING){
        philosophers[i]->state = EATING;
        _sys_semPost(philosophers[i]->sem);
        showState();
    }
}

static void showState(){
    for(int i = 0; i < cantPhilosophers; i++){
        if(philosophers[i]->state == EATING)
            printf("E ");
        else if(philosophers[i]->state == THINKING || philosophers[i]->state == HUNGRY)
            printf(". ");
        else
            printf("\n");
    }
}

