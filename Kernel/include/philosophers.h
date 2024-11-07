//
// Created by Tizifuchi12 on 6/11/2024.
//

#ifndef TP2_SO_PHILOSOPHERS_H
#define TP2_SO_PHILOSOPHERS_H

typedef struct {
    int16_t pid; //LO GUARDO POR SI LO LLEGO A NECESITAR DESPUES
    state_t state;
    semaphore_t * sem;
} philosopher_t; //aca meto toda la info de ese filosofo, con esto no hace falta que cree los vectores que usa en la teorica

#endif //TP2_SO_PHILOSOPHERS_H
