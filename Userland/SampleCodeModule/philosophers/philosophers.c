// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <iolib.h>
#include <philosophers.h>
#include <shell.h>
#include <stringutils.h>
#include <syscalls.h>

#define STDIN 0
#define STDOUT 1
#define MAX_PHILOSOPHERS 15
#define THINKING_TICKS 5
#define MIN_PHILOSOPHERS 3
#define MUTEX "philo_mutex_id"
#define LEFT(i) ((i + numbPhilosophers - 1) % numbPhilosophers)
#define RIGHT(i) ((i + 1) % numbPhilosophers)

#define ADD 'a'
#define REMOVE 'r'
#define QUIT 'q'
#define CLEAR 'c'
#define START 's'
#define RED_COLOR 0x00FF0000
#define GREEN_COLOR 0x0000FF00
#define ORANGE_COLOR 0X00F19743
#define BLUE_COLOR 0x0000AFFF

typedef enum { NONE = 0,
			   THINKING,
			   HUNGRY,
			   EATING } state_t;

typedef struct {
	short pid;
	state_t state;
	char *semName;
} philosopher_t;

static int numbPhilosophers = 0;
static philosopher_t philosophers[MAX_PHILOSOPHERS];

static int addPhilosopher(int i);
static int deletePhilo(int i);
static void philoActions(int argc, char *argv[]);
static void putForks(int i);
static void takeForks(int i);
static void showState();
static void test(int i);
static void instructions();
static void set_own_priority_to_max();

void philosopherProgram(int argc, char *argv[]) {
	if (_sys_semOpen(MUTEX, 1) == -1) { // genero el semaforo general
		printf("Error opening mutex\n");
		return;
	}

	for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
		philosophers[i].state = NONE;
		philosophers[i].pid = -1;
	}

	instructions();

	set_own_priority_to_max();
	while (getchar() != START)
		;

	for (int i = 0; i < MIN_PHILOSOPHERS; i++) {
		addPhilosopher(i);
	}

	char c;
	while ((c = getchar()) != QUIT) {
		switch (c) {
			case ADD:
				addPhilosopher(numbPhilosophers);
				break;
			case REMOVE:
				deletePhilo(numbPhilosophers - 1);
				break;
			case CLEAR:
				_sys_clearScreen();
				break;
		}
	}

	printf("Problema de los filosofos terminado\n");

	for (int i = 0; i < MAX_PHILOSOPHERS; i++) {
		if (i < numbPhilosophers) {
			_sys_semClose(philosophers[i].semName);
			_sys_kill(philosophers[i].pid);
		}
	}

	numbPhilosophers = 0;
	_sys_semClose(MUTEX);
}

static int addPhilosopher(int i) {
	if (numbPhilosophers == MAX_PHILOSOPHERS) {
		printStringColor("Max philosophers reached\n", RED_COLOR);
		return -1;
	}

	_sys_semWait(MUTEX);

	printf("Added philosopher %d\n", i + 1);

	char philoName[20];
	char philoNumberBuffer[3];
	strcpy(philoName, "philosopher ");
	char *philoNumber = itoa(numbPhilosophers, philoNumberBuffer);
	strcat(philoName, philoNumber);

	_sys_semOpen(philoName, 1);
	philosophers[i].semName = philoName;
	philosophers[i].state = THINKING;

	char *argv[] = {philoNumber, NULL};
	int aux_fd[] = {STDIN, STDOUT};
	philosophers[i].pid = (short) _sys_createProcess((mainFunction) &philoActions,
													 argv, philoName, 0, aux_fd);
	if (philosophers[i].pid == -1) {
		printStringColor("Error creating philosopher\n", RED_COLOR);
		return -1;
	}
	_sys_changePriority(philosophers[i].pid, 4);
	numbPhilosophers++;

	showState();

	_sys_semPost(MUTEX);
	return 0;
}

static int deletePhilo(int i) {
	if (numbPhilosophers == MIN_PHILOSOPHERS) {
		printf("Min philosophers reached\n");
		return -1;
	}

	_sys_semWait(MUTEX);

	while (philosophers[LEFT(i)].state == EATING &&
		   philosophers[RIGHT(i)].state == EATING) {
		_sys_semPost(MUTEX); // hago que el resto siga con sus cosas
		_sys_semWait(
			philosophers[i]
				.semName);	 // aca espera a que arranque a comer para eliminarse, en
							 // ese caso los del costado no vana estarr comiendo
		_sys_semWait(MUTEX); // procedo como si no hubiera entrado al while
	}

	_sys_semClose(philosophers[i].semName);
	philosophers[i].state = NONE; // pongo que no hay filosofo
	_sys_kill(philosophers[i].pid);
	philosophers[i].pid =
		-1; // marco como que ya no hay filosofo ahi, para que no quede el pid
			// viejo pero no es necesario para el programa, se piede sacar
	numbPhilosophers--;

	printf("Delete philosopher %d\n", i + 1);

	showState();

	_sys_semPost(MUTEX);
	return 0;
}

static void philoActions(int argc, char *argv[]) {
	int i = atoi(argv[0]);

	while (1) {
		_sys_sleep(THINKING_TICKS); // ver que valor poner aca
		takeForks(i);
		if (philosophers[i].state ==
			EATING) {					// IF DE BALBOA, HACE QUE NINGUN SE MUERA DE HAMBRE PERO HAY
										// PATRON TODAVIA
			_sys_sleep(THINKING_TICKS); // ver que valor poner aca
			putForks(i);
		}
	}
}

static void takeForks(int i) { // igual al de la presentaciion
	_sys_semWait(MUTEX);
	philosophers[i].state = HUNGRY;
	test(i);
	_sys_semPost(MUTEX);
	_sys_semWait(philosophers[i].semName);
}

static void putForks(int i) { // igual al de la presentacion
	_sys_semWait(MUTEX);
	philosophers[i].state = THINKING;
	test(LEFT(i));
	test(RIGHT(i));
	_sys_semPost(MUTEX);
}

static void test(int i) {
	if (philosophers[i].state == HUNGRY &&
		philosophers[LEFT(i)].state != EATING &&
		philosophers[RIGHT(i)].state != EATING) {
		philosophers[i].state = EATING;
		_sys_semPost(philosophers[i].semName);
		showState();
		_sys_sleep(THINKING_TICKS); // ver que valor poner aca
	}
}

static void showState() {
	if (noScreenSpace())
		_sys_clearScreen();

	for (int i = 0; i < numbPhilosophers; i++) {
		if (philosophers[i].state == EATING)
			printf("E ");
		else if (philosophers[i].state == THINKING ||
				 philosophers[i].state == HUNGRY)
			printf(". ");
	}
	printf("\n"); // Añade una nueva línea después de imprimir todos los filósofos
}

static void instructions() {
	_sys_clearScreen();
	printStringColor("Welcome to the philosophers program\n\n", GREEN_COLOR);
	printf("Press 'a' to add a philosopher\n");
	printf("Press 'r' to remove a philosopher\n");
	printf("Press 'q' to quit\n");
	printf("Press 'c' to clear the screen\n\n");

	printStringColor("Press 's' to start the philosophers program !\n\n",
					 ORANGE_COLOR);
}

static void set_own_priority_to_max() {
	_sys_changePriority(_sys_getpid(), 4);
}