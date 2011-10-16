#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "sw_gpu_param_search.h"

#include "sw_solve_gpu.h"
#include "chain.h"
#include "sw_data.h"
#include "sw_prefs.h"
#include "sw_result.h"

#define MAX_THREADS 700
#define MAX_BLOCKS 65000

#define POPULATION_NMR 50
#define GENERATION_NMR 500
#define CHILD_NMR 20
#define PC 0.95
#define PM 0.05

typedef struct Cromosome {
    int blocks;
    int threads;
    long fitness;
} Cromosome;

typedef struct Parents {
    Cromosome parent1;
    Cromosome parent2;
} Parents;

static Cromosome population[POPULATION_NMR];
static Cromosome children[CHILD_NMR];

static Chain* rowChain;
static Chain* columnChain;
static SWPrefs* swPrefs;
static int columns;
MatcherScore expectedResult;

void swapCromosomes(Cromosome *first,Cromosome *second);
void sortCromosomes(Cromosome list[], int start, int end);

static void initPopulation();
static Cromosome crossover(Parents parents);
static void mutation(Cromosome* cromosome);
static Parents selection();
static void evaluateChildren();
static void evaluatePopulation();
static void evaluateCromosome(Cromosome* cromosome);

static int randomInt(int range) {
    return rand() % range;
}

static double randomFloat() {
    return (double) rand() / RAND_MAX;
}

extern void swGPUParamSearch(Chain* rowChainL, Chain* columnChainL, 
    SWPrefs* swPrefsL) {

    SWResult* swResult = 
        swDataGetResult(swSolveGPU(rowChain, columnChain, swPrefs), 0);

    rowChain = rowChainL;
    columnChain = columnChainL;
    swPrefs = swPrefsL;
    columns = chainGetLength(columnChainL);
    expectedResult = swResultGetScore(swResult);

    srand(time(NULL));

    Cromosome child;

    initPopulation();
    evaluatePopulation();

    int generationIdx;
    int childIdx;

    for (generationIdx = 0; generationIdx < GENERATION_NMR; ++generationIdx) {

        sortCromosomes(population, 0, POPULATION_NMR - 1);

        printf(".............................................\n");
        printf("Generation: %d\n", generationIdx);

        for (childIdx = 0; childIdx < CHILD_NMR; ++childIdx) {

            child = crossover(selection());
            mutation(&child);

            children[childIdx] = child;
        }

        evaluateChildren();

        for (childIdx = 0; childIdx < CHILD_NMR; ++childIdx) {
            population[POPULATION_NMR - childIdx - 1] = children[childIdx];
        }
    }
}

static void initPopulation() {

    int cromosomeIdx;
    int blocks;
    int threads;

    for (cromosomeIdx = 0; cromosomeIdx < POPULATION_NMR; ++cromosomeIdx) {
        
        do {
            blocks = randomInt(columns - 1) + 1;
            threads = randomInt(columns - 1) + 1;
        } while (2l * blocks * threads >= (long) columns 
            || threads > MAX_THREADS || blocks > MAX_BLOCKS);

        population[cromosomeIdx].blocks = blocks;
        population[cromosomeIdx].threads = threads;
    }
}

static void evaluatePopulation() {

    int cromosomeIdx;

    for (cromosomeIdx = 0; cromosomeIdx < POPULATION_NMR; ++cromosomeIdx) {
        evaluateCromosome(&population[cromosomeIdx]);
    }
}

static void evaluateChildren() {

    int cromosomeIdx;

    for (cromosomeIdx = 0; cromosomeIdx < CHILD_NMR; ++cromosomeIdx) {
        evaluateCromosome(&children[cromosomeIdx]);
    }
}

static void evaluateCromosome(Cromosome* cromosome) {

    swPrefsSetCudaThreads(swPrefs, cromosome->blocks);
    swPrefsSetCudaThreads(swPrefs, cromosome->threads);

    clock_t start = clock();

    SWResult* swResult = 
        swDataGetResult(swSolveGPU(rowChain, columnChain, swPrefs), 0);

    MatcherScore score = swResultGetScore(swResult);

    if(score != expectedResult) {
        printf("error---> %f", (float) score);
    }

    clock_t finish = clock();

    cromosome->fitness = finish - start;

    printf("eval: %ld %d %d\n", 
        cromosome->fitness, cromosome->blocks, cromosome->threads);
}

static Parents selection() {

    int index[3];
    Parents parents;

    index[0] = randomInt(POPULATION_NMR);

    do { 
        index[1] = randomInt(POPULATION_NMR); 
    } while (index[0] == index[1]);

    do { 
        index[2] = randomInt(POPULATION_NMR); 
    } while (index[0] == index[2] || index[1] == index[2]);

    if (index[0] > index[1] && index[0] > index[2]) { 
        parents.parent1 = population[index[1]];
        parents.parent2 = population[index[2]];
    } else if (index[1] > index[2] && index[1] > index[0]) { 
        parents.parent1 = population[index[0]];
        parents.parent2 = population[index[2]];
    } else { 
        parents.parent1 = population[index[0]];
        parents.parent2 = population[index[1]];
    }

    return parents;
}

static Cromosome crossover(Parents parents) {

    if (randomFloat() < PC) {
        Cromosome child;
        child.blocks = (parents.parent1.blocks + parents.parent2.blocks) / 2;
        child.threads = (parents.parent1.threads + parents.parent2.threads) / 2;
        return child;
    } else {
        if (randomFloat() < 0.5) return parents.parent1;
        return parents.parent2;
    }
}

static void mutation(Cromosome* cromosome) {

    do {
        if (randomFloat() < PM) {
            cromosome->blocks = randomInt(sqrt(columns) - 1) + 1;
        }

        if (randomFloat() < PM) {
            cromosome->threads = randomInt(sqrt(columns) - 1) + 1;
        }
    } while (2l * cromosome->blocks * cromosome->threads >= (long) columns 
        || cromosome->threads > MAX_THREADS || cromosome->blocks > MAX_BLOCKS);
}

void swapCromosomes(Cromosome *first,Cromosome *second) {
   Cromosome holder;
   holder = *first;
   *first = *second;
   *second = holder;
}

void sortCromosomes(Cromosome list[], int start, int end) {

    int key;
    int frontIdx;
    int backIdx;
    int pivot;

    if (start < end) {

        pivot = (start + end) / 2;
        swapCromosomes(&list[start], &list[pivot]);
        key = list[start].fitness;

        frontIdx = start + 1;
        backIdx = end;

        while (frontIdx <= backIdx) {

            while ((frontIdx <= end) && (list[frontIdx].fitness <= key)) {
                frontIdx++;
            }

            while ((backIdx >= start) && (list[backIdx].fitness > key)) {
                backIdx--;
            }

            if (frontIdx < backIdx) {
                swapCromosomes(&list[frontIdx],&list[backIdx]);
            }
        }

        swapCromosomes(&list[start],&list[backIdx]);

        sortCromosomes(list,start,backIdx-1);
        sortCromosomes(list,backIdx+1,end);
    }
}

