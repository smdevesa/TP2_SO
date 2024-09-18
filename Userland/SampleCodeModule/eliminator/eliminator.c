#include <eliminator.h>
#include <iolib.h>
#include <color.h>
#include <syscalls.h>

#define ESC 27

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// Colors and animations
#define SCREEN_COLOR 0x00000000
#define BORDER_COLOR 0x00FFFFFF
#define BORDER_WIDTH 16
#define BORDER_SEGMENT_LENGTH 128
#define BORDER_DELAY 10
#define ANIMATED_CLEAR_DELAY 10
#define ANIMATED_CLEAR_LENGTH 128

#define BIKE_COLOR_AMOUNT 4
#define SPEEDS_AMOUNT 4

// Font scales
#define MENU_FONT_SCALE 2
#define GAME_FONT_SCALE 1

#define BIKE_SQUARE_SIZE 16

// Matrix dimensions
#define ROWS (SCREEN_HEIGHT / BIKE_SQUARE_SIZE)
#define COLUMNS (SCREEN_WIDTH / BIKE_SQUARE_SIZE)

// Matrix values
#define VOID 0
#define BORDER 1
#define BIKE_1 2
#define BIKE_2 3

// Players
typedef enum players {PLAYER_1, PLAYER_2, CPU} TPlayer;

// Game constants
#define X_SPAWN_DIVISOR 2
#define Y_SPAWN_OFFSET 10

typedef enum directions {UP, RIGHT, DOWN, LEFT} TDirection;

typedef struct BikeColor {
    uint32_t color;
    char * name;
} TBikeColor;

typedef struct Bike {
    uint64_t x;
    uint64_t y;
    TDirection direction;
    uint32_t color;
    uint8_t player;
} TBike;

static TBikeColor allBikeColors[BIKE_COLOR_AMOUNT] = {
        {0x0064D8F4, "TRON Blue"},
        {0x00E6A43C, "TRON Orange"},
        {RED, "Red"},
        {GREEN, "Green"}
};

static uint64_t speeds[SPEEDS_AMOUNT] = {200, 100, 50, 25};

static unsigned char board[SCREEN_HEIGHT / BIKE_SQUARE_SIZE][SCREEN_WIDTH / BIKE_SQUARE_SIZE];

static int numOfPlayers;
static uint32_t bikeColor1;
static uint32_t bikeColor2;
static uint64_t speed;

static uint32_t player1Score;
static uint32_t player2Score;

static uint32_t setBikeColor(uint32_t other);
static uint64_t setSpeed();
static int fillBikeColors(TBikeColor * bikeColors, uint32_t other);
static void startScreen();
static int getNumOfPlayers();
static void startGame();
static void initBoard();
static void drawBorder();
static int moveBike(TBike * bike);
static void readInput1(TBike * bike);
static void readInput2(TBike * bike1, TBike * bike2);
static void CPUInput(TBike * bike);
static int getDepth(TBike * bike, TDirection direction);
static int wonGame(TPlayer p);
static void animatedClear();
static void playMelody();

int eliminator() {
    startScreen();
    startGame();
    return 1;
}

static void startScreen() {
    clearScreen();
    setFontScale(MENU_FONT_SCALE);
    puts("Welcome to the Eliminator Game!");
    numOfPlayers = getNumOfPlayers();
    bikeColor1 = setBikeColor(0);
    bikeColor2 = setBikeColor(bikeColor1);
    speed = setSpeed();

    player1Score = 0;
    player2Score = 0;
}


static void playMelody() {
    
    uint32_t frequencies[] = {261, 329, 392};
    uint32_t duration = 100; 
    uint32_t duration_sleep = 150; 

    for (int i = 0; i < 3; i++) {
        _sys_playSound(frequencies[i], duration);
        _sys_sleep(duration_sleep); 
    }
}

static void startGame() {
    char returned = 0;
    do {
        clearScreen();
        setFontScale(GAME_FONT_SCALE);
        initBoard();
        drawBorder();
        TBike bike1 = {COLUMNS / X_SPAWN_DIVISOR, Y_SPAWN_OFFSET, DOWN, bikeColor1, BIKE_1};
        TBike bike2 = {COLUMNS / X_SPAWN_DIVISOR, ROWS - Y_SPAWN_OFFSET, UP, bikeColor2, BIKE_2};
        char playing = 1;

        while (playing) {
            if (numOfPlayers == 1)
                readInput1(&bike1);
            else
                readInput2(&bike1, &bike2);

            if (!moveBike(&bike1)) {
                playMelody();
                player2Score++;
                returned = wonGame(numOfPlayers == 1 ? CPU : PLAYER_2);
                playing = 0;
            }

            // If there is only one player, the CPU will move the second bike
            if (numOfPlayers == 1)
                CPUInput(&bike2);

            if (playing) {
                if (!moveBike(&bike2)) {
                    playMelody();
                    player1Score++;
                    returned = wonGame(PLAYER_1);
                    playing = 0;
                }
            }
            _sys_sleep(speed);
        }
    } while (returned);
    clearScreen();
}

static int wonGame(TPlayer p) {
    animatedClear();
    setFontScale(MENU_FONT_SCALE);
    switch (p) {
        case PLAYER_1:
            printStringColor("Player 1 won!\n", bikeColor1);
            break;
        case PLAYER_2:
            printStringColor("Player 2 won!\n", bikeColor2);
            break;
        case CPU:
            printStringColor("Eliminator won!\n", bikeColor2);
            break;
    }
    putchar('\n');
    puts("Scoreboard:");
    printf("Player 1: %d\n", player1Score);
    if(numOfPlayers == 2)
        printf("Player 2: %d\n", player2Score);
    else
        printf("Eliminator: %d\n", player2Score);
    putchar('\n');
    puts("Press SPACE to play again or ESC to exit:");

    char c;
    while ((c = getchar()) != ' ' && c != ESC);
    return c == ' ';
}

static void animatedClear() {
    for(int i = 0; i < SCREEN_WIDTH; i += ANIMATED_CLEAR_LENGTH) {
        drawRectangle(SCREEN_COLOR, i, 0, ANIMATED_CLEAR_LENGTH, SCREEN_HEIGHT);
        _sys_sleep(ANIMATED_CLEAR_DELAY);
    }
}

static int moveBike(TBike * bike) {
    int x = bike->x;
    int y = bike->y;
    switch(bike->direction) {
        case UP:
            y--;
            break;
        case RIGHT:
            x++;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
    }
    if(board[y][x] == VOID) {
        board[y][x] = bike->player;
        drawRectangle(bike->color, x * BIKE_SQUARE_SIZE, y * BIKE_SQUARE_SIZE, BIKE_SQUARE_SIZE, BIKE_SQUARE_SIZE);
        bike->x = x;
        bike->y = y;
        return 1;
    }
    return 0;
}

static void readInput1(TBike * bike) {
    char c;
    int readBytes = _sys_read(STDIN, &c, 1);
    if(readBytes) {
        c = toLower(c);
        switch (c) {
            case 'w':
                if(bike->direction != DOWN)
                    bike->direction = UP;
                break;
            case 'd':
                if(bike->direction != LEFT)
                    bike->direction = RIGHT;
                break;
            case 's':
                if(bike->direction != UP)
                    bike->direction = DOWN;
                break;
            case 'a':
                if(bike->direction != RIGHT)
                    bike->direction = LEFT;
                break;
        }
    }
}

static void readInput2(TBike * bike1, TBike * bike2) {
    char c;
    int readBytes = _sys_read(STDIN, &c, 1);
    // player 1 with WASD, player 2 with IJKL
    if(readBytes) {
        c = toLower(c);
        switch(c) {
            case 'w':
                if(bike1->direction != DOWN)
                    bike1->direction = UP;
                break;
            case 'd':
                if(bike1->direction != LEFT)
                    bike1->direction = RIGHT;
                break;
            case 's':
                if(bike1->direction != UP)
                    bike1->direction = DOWN;
                break;
            case 'a':
                if(bike1->direction != RIGHT)
                    bike1->direction = LEFT;
                break;
            case 'i':
                if(bike2->direction != DOWN)
                    bike2->direction = UP;
                break;
            case 'l':
                if(bike2->direction != LEFT)
                    bike2->direction = RIGHT;
                break;
            case 'k':
                if(bike2->direction != UP)
                    bike2->direction = DOWN;
                break;
            case 'j':
                if(bike2->direction != RIGHT)
                    bike2->direction = LEFT;
                break;

        }
    }
}

static void CPUInput(TBike * bike) {
    uint64_t direction = bike->direction;
    TDirection bestDirection = direction;
    int maxDepth;
    char change = 0;

    if((maxDepth = getDepth(bike, direction) <= 5)) {
        change = 1;
    }

    if(change) {
        for (TDirection dir = UP; dir <= LEFT; dir++) {
            int depth = getDepth(bike, dir);
            if (depth > maxDepth) {
                maxDepth = depth;
                bestDirection = dir;
            }
        }
    }
    bike->direction = bestDirection;
}

static int getDepth(TBike * bike, TDirection direction) {
    int x = bike->x;
    int y = bike->y;
    int depth = 0;
    switch(direction) {
        case UP:
            y--;
            break;
        case RIGHT:
            x++;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
    }
    while(board[y][x] == VOID) {
        depth++;
        switch(direction) {
            case UP:
                y--;
                break;
            case RIGHT:
                x++;
                break;
            case DOWN:
                y++;
                break;
            case LEFT:
                x--;
                break;
        }
    }
    return depth;
}

static void initBoard() {
    for(int i = 0; i < ROWS; i++) {
        for(int j = 0; j < COLUMNS; j++) {
            if(i == 0 || i == (ROWS - 1) || j == 0 || j == (COLUMNS - 1))
                board[i][j] = BORDER;
            else
                board[i][j] = VOID;
        }
    }
}

static void drawBorder() {
    for(int i=0; i<SCREEN_WIDTH; i += BORDER_SEGMENT_LENGTH) {
        drawRectangle(BORDER_COLOR, i, 0, BORDER_SEGMENT_LENGTH, BORDER_WIDTH);
        if((i + BORDER_SEGMENT_LENGTH) >= SCREEN_WIDTH)
            drawRectangle(BORDER_COLOR, SCREEN_WIDTH - BORDER_SEGMENT_LENGTH, 0, BORDER_SEGMENT_LENGTH, BORDER_WIDTH);
        _sys_sleep(BORDER_DELAY);
    }
    for(int i=0; i<SCREEN_HEIGHT; i += BORDER_SEGMENT_LENGTH) {
        drawRectangle(BORDER_COLOR, SCREEN_WIDTH - BORDER_WIDTH, i, BORDER_WIDTH, BORDER_SEGMENT_LENGTH);
        if((i + BORDER_SEGMENT_LENGTH) >= SCREEN_HEIGHT)
            drawRectangle(BORDER_COLOR, SCREEN_WIDTH - BORDER_WIDTH, SCREEN_HEIGHT - BORDER_SEGMENT_LENGTH, BORDER_WIDTH, BORDER_SEGMENT_LENGTH);
        _sys_sleep(BORDER_DELAY);
    }
    for(int i=SCREEN_WIDTH; i>0; i -= BORDER_SEGMENT_LENGTH) {
        drawRectangle(BORDER_COLOR, i, SCREEN_HEIGHT - BORDER_WIDTH, BORDER_SEGMENT_LENGTH, BORDER_WIDTH);
        if((i - BORDER_SEGMENT_LENGTH) <= 0)
            drawRectangle(BORDER_COLOR, 0, SCREEN_HEIGHT - BORDER_WIDTH, BORDER_SEGMENT_LENGTH, BORDER_WIDTH);
        _sys_sleep(BORDER_DELAY);
    }
    for(int i=SCREEN_HEIGHT; i>0; i -= BORDER_SEGMENT_LENGTH) {
        drawRectangle(BORDER_COLOR, 0, i, BORDER_WIDTH, BORDER_SEGMENT_LENGTH);
        if((i - BORDER_SEGMENT_LENGTH) <= 0)
            drawRectangle(BORDER_COLOR, 0, 0, BORDER_WIDTH, BORDER_SEGMENT_LENGTH);
        _sys_sleep(BORDER_DELAY);
    }
}

static uint64_t setSpeed() {
    char c;
    uint64_t speed;
    puts("Choose speed: 1-4");
    do {
        c = getchar();
        if(c >= '1' && c <= '4') {
            speed = speeds[c - '1'];
        }
    } while(c < '1' || c > '4');
    putchar('\n');
    return speed;
}

static uint32_t setBikeColor(uint32_t other){
    char c;
    uint32_t color;
    TBikeColor bikeColors[BIKE_COLOR_AMOUNT];
    int dim = fillBikeColors(bikeColors, other);

    printf("Choose bike color");
    if(other != 0){
        puts(" for Player 2:"); 
    }
    else {
        puts(" for Player 1:"); 
    }
    for(int i = 0; i < dim; i++){
        printf("%d. ", i+1);
        printStringColor(bikeColors[i].name, bikeColors[i].color);
        putchar('\n');
    }
    do {
        c = getchar();
        if(c >= '1' && c <= '4') {
            if(dim == 3 && c == '4') {
                c = 0;
            }
            else {
                color = bikeColors[c - '1'].color;
            }
        }
    } while(c < '1' || (c > '4' || (c > '3' && other != 0)));
    putchar('\n');
    return color;
}

static int fillBikeColors(TBikeColor * bikeColors, uint32_t other){
    int dim = 0;
    for(int i = 0; i < BIKE_COLOR_AMOUNT; i++) {
        if(allBikeColors[i].color != other){
            bikeColors[dim++] = allBikeColors[i];
        }
    }
    return dim;
}

static int getNumOfPlayers() {
    char c;
    int numOfPlayers;
    puts("Choose number of players: 1 or 2");
    do {
        c = getchar();
        if(c == '1') {
            numOfPlayers = 1;
        }
        else if(c == '2') {
            numOfPlayers = 2;
        }
    } while(c != '1' && c != '2');
    putchar('\n');
    return numOfPlayers;
}