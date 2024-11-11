//
// Created by Santiago Devesa on 23/10/2024.
//

#ifndef TP2_SO_SPINLOCK_H
#define TP2_SO_SPINLOCK_H

typedef int lock_t;

extern void acquire(lock_t *lock);
extern void release(lock_t *lock);

#endif // TP2_SO_SPINLOCK_H
