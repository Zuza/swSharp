/*
SW# - CUDA parallelized Smith Waterman with applying Hirschberg's algorithm
Copyright (C) 2011 Matija Korpar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Contact the author by mkorpar@gmail.com.
*/

#define SW 0
#define NW 1

typedef struct SWAffineScore {
    MatcherScore score;
    MatcherScore affine;
} SWAffineScore;

typedef struct SWItem {
    MatcherScore matchScr;
    SWAffineScore deleteScr;
    SWAffineScore insertScr[ALPHA];
    SWAffineScore currentScr[ALPHA];
    MatcherScore deleteAffineScr;
} SWItem;

typedef struct HBusItem {
    SWAffineScore deleteScr;
} HBusItem;

typedef struct VBusItem {
    MatcherScore matchScr;
    SWAffineScore insertScr[ALPHA];
} VBusItem;

typedef struct Result {
    MatcherScore score;
    int row;
    int column;
} Result;

typedef struct Bus {
    HBusItem* horizontal;
    VBusItem* vertical;
} Bus;

typedef struct SharedMemory {
    Result result;
    HBusItem hBusItem;
} SharedMemory;

typedef struct SWParams {
    int ROWS;
    int COLUMNS;
    int BLOCKS;
    int THREADS;
    int CELL_WIDTH;
    int CELL_HEIGHT;
    int REAL_ROWS;
    int REAL_COLUMNS;
    SWPrefsPenalty GAP_OPEN;
    SWPrefsPenalty GAP_EXTEND;
} SWParams;

__constant__ static SWParams GPU_CNST;

__constant__ const static HBusItem hBusEmpty = {
    {0, MATCHER_SCORE_MIN}
};

__constant__ const static SWItem swItemEmpty = {
    0,
    {0, MATCHER_SCORE_MIN}, 
    {{0, MATCHER_SCORE_MIN}}, 
    {{0, MATCHER_SCORE_MIN}}, 
    0
};

static SWParams CPU_CNST;

static void prepareConstants(int rows, int columns, SWPrefs* swPrefs) {

    CPU_CNST.BLOCKS = swPrefsGetCudaBlocks(swPrefs);
    CPU_CNST.THREADS = swPrefsGetCudaThreads(swPrefs);

    CPU_CNST.ROWS = rows + rows % ALPHA;
    CPU_CNST.COLUMNS = columns + (CPU_CNST.BLOCKS - columns % CPU_CNST.BLOCKS);

    CPU_CNST.REAL_ROWS = rows;
    CPU_CNST.REAL_COLUMNS = columns;

    CPU_CNST.CELL_WIDTH = CPU_CNST.COLUMNS / CPU_CNST.BLOCKS;
    CPU_CNST.CELL_HEIGHT = ALPHA * CPU_CNST.THREADS;

    CPU_CNST.GAP_OPEN = swPrefsGetGapOpen(swPrefs);
    CPU_CNST.GAP_EXTEND = swPrefsGetGapExtend(swPrefs);

    cudaMemcpyToSymbol(QUOTE(GPU_CNST), &CPU_CNST, sizeof(SWParams));
}

static void prepareParameters(int columns, SWPrefs* swPrefs) {
    
    int threads = CUDA_THREADS_DEFUALT;
    int blocks = CUDA_BLOCKS_DEFUALT;

    // Hardware conditions.
    cudaDeviceProp deviceProperties;
    cudaGetDeviceProperties(&deviceProperties, 0);

    threads = min(threads, deviceProperties.maxThreadsPerBlock);
    blocks = min(blocks, deviceProperties.maxGridSize[0]);
    // End hardware conditions.

    // Problem conditions.
    if (threads * blocks * 2 > columns - ALPHA) {
        blocks = (int) ((float) (columns - ALPHA) / (threads * 2));
    }

    if (blocks == 0) {
        blocks = 1;
        threads = columns / 2;
    }
    // End problem conditions.

    swPrefsSetCudaThreads(swPrefs, threads);
    swPrefsSetCudaBlocks(swPrefs, blocks);
}

static int swapChains(Chain** rowChain, Chain** columnChain) {

    if (chainGetLength(*rowChain) > chainGetLength(*columnChain)) {
        return 0;
    }

    Chain* holder = *rowChain;
    *rowChain = *columnChain;
    *columnChain = holder;

    return 1;
}

static ChainCode* initChainGPU(Chain* chain, int deviceLength, 
    SWPrefs* swPrefs) {

    int length = chainGetLength(chain);
    ChainCode* chainCodes = chainGetCodes(chain);
    ChainCode* chainGPU;

    cudaMalloc(&chainGPU, deviceLength * sizeof(ChainCode));
    cudaMemcpy(chainGPU, chainCodes, length * sizeof(ChainCode), 
        cudaMemcpyHostToDevice);

    int paddingIdx;
    for (paddingIdx = length; paddingIdx < deviceLength; ++paddingIdx) {

        // remeber to update dummy code, it is mutable
        ChainCode dummyCode = matcherGetDummyCode(swPrefsGetMatcher(swPrefs));

        cudaMemcpy(chainGPU + paddingIdx, &dummyCode, 
            sizeof(ChainCode), cudaMemcpyHostToDevice);
    }

    return chainGPU;
}

static HBusItem* initHBusGPU(SWPrefs* swPrefs, int type) { 

    HBusItem* bus;

    SWPrefsPenalty gapOpen = swPrefsGetGapOpen(swPrefs);
    SWPrefsPenalty gapExtend = swPrefsGetGapExtend(swPrefs);

    int hBusLen = CPU_CNST.COLUMNS;
    int hBusSize = hBusLen * sizeof(HBusItem);
    cudaMalloc(&bus, hBusSize);
 
    HBusItem* hBus = (HBusItem*) malloc(hBusSize);
    for (int hBusIdx = 0; hBusIdx < hBusLen; ++hBusIdx) {
        if (type == SW) {
            hBus[hBusIdx] = hBusEmpty;
        } else if (type == NW) {
            hBus[hBusIdx].deleteScr.score = -gapOpen - hBusIdx * gapExtend;
            hBus[hBusIdx].deleteScr.affine = MATCHER_SCORE_MIN;
        }
    }

    cudaMemcpy(bus, hBus, hBusSize, cudaMemcpyHostToDevice);

    free(hBus);

    return bus;
}

static VBusItem* initVBusGPU() { 

    VBusItem* bus;

    int vBusLen = CPU_CNST.BLOCKS * CPU_CNST.THREADS;
    int vBusSize = vBusLen * sizeof(VBusItem);
    cudaMalloc(&bus, vBusSize);

    VBusItem* vBus = (VBusItem*) malloc(vBusSize);
    cudaMemcpy(bus, vBus, vBusSize, cudaMemcpyHostToDevice);

    free(vBus);

    return bus;
}

static Result* initResultsGPU(int size) {

    Result* resultsGPU;

    int resultGPUSize = size * sizeof(Result);

    cudaMalloc(&resultsGPU, resultGPUSize);
    cudaMemset(resultsGPU, 0, resultGPUSize); 

    return resultsGPU;
}

static Result getResult(Result* resultsGPU, int size) {

    Result result = {0, 0, 0};
    Result results[size];

    cudaMemcpy(results, resultsGPU, size * sizeof(Result), cudaMemcpyDeviceToHost);

    for (int resultIdx = 0; resultIdx < size; ++resultIdx) {
        if (results[resultIdx].score > result.score) {
            result = results[resultIdx];
        }
    }

    return result;
}

static void printMemoryUsage() {
    if (VERBOSE) {
        printf("GPU memory needed: %lfMB\n", 
            (
                CPU_CNST.REAL_ROWS + 
                CPU_CNST.REAL_COLUMNS + 
                CPU_CNST.BLOCKS * CPU_CNST.THREADS * sizeof(VBusItem) + 
                CPU_CNST.REAL_COLUMNS * sizeof(HBusItem) +
                CPU_CNST.BLOCKS * sizeof(Result)
            ) / 1024. / 1024.
        );
        printf("Size: %d x %d\n", CPU_CNST.REAL_ROWS, CPU_CNST.REAL_COLUMNS); 
    }
}

static void printPercentage(int current, int total) {
    if (VERBOSE) {
        int percentNew = (current + 1) * 100 / total;
        int percentOld = current * 100 / total;

        if (percentNew > percentOld) {
            printPercentageBar(percentNew);
        }
    }
}

static int getDiagonalNmr() {

    int diagonals = CPU_CNST.BLOCKS + (CPU_CNST.ROWS / CPU_CNST.CELL_HEIGHT);

    if (CPU_CNST.ROWS % CPU_CNST.CELL_HEIGHT > 0) {
        diagonals++;
    }

    return diagonals;
}

static void cudaError() {
    cudaError_t error = cudaGetLastError();
    if (error != 0) {
        printf("GPU error %s.\n", cudaGetErrorString(error));
        exit(-1);
    }
}

__device__ inline int getRow(int diagonal) {

    int row = (diagonal + blockIdx.x - GPU_CNST.BLOCKS + 1) 
        * (GPU_CNST.THREADS * ALPHA) + threadIdx.x * ALPHA;

    return row;
} 

__device__ inline int getColumn() {

    int column = GPU_CNST.CELL_WIDTH * (GPU_CNST.BLOCKS - blockIdx.x - 1) 
        - threadIdx.x;

    return column;
} 

__device__ inline int getVBusIdx(int row) {
    return (row / ALPHA) % (GPU_CNST.BLOCKS * GPU_CNST.THREADS);
} 

__device__ inline void getVBus(VBusItem* item, VBusItem* bus, int row, 
    int column, const int type) {

    int rowValid = ((row >= 0) && (row < GPU_CNST.REAL_ROWS));
    int firstCell = (column == 0);

    if (type == SW) {
        if (rowValid && !firstCell) {
            *item = bus[getVBusIdx(row)];
        } else {
            item->matchScr = 0;
            for (int alpha = 0; alpha < ALPHA; ++alpha) {
                item->insertScr[alpha].score = 0;
                item->insertScr[alpha].affine = MATCHER_SCORE_MIN;
            }
        }
    }

    if (type == NW) {
        if (rowValid) {
            if (firstCell) {
                item->matchScr = (-GPU_CNST.GAP_OPEN 
                    - (row - 1) * GPU_CNST.GAP_EXTEND) * (row > 0);

                for (int alpha = 0; alpha < ALPHA; ++alpha) {
                    item->insertScr[alpha].score = -GPU_CNST.GAP_OPEN 
                        - (row + alpha) * GPU_CNST.GAP_EXTEND;
                    item->insertScr[alpha].affine = MATCHER_SCORE_MIN;
                }
            } else {
                *item = bus[getVBusIdx(row)];
            }
        } else {
            item->matchScr = 0;
            for (int alpha = 0; alpha < ALPHA; ++alpha) {
                item->insertScr[alpha].score = 0;
                item->insertScr[alpha].affine = MATCHER_SCORE_MIN;
            }
        }
    }
} 

__device__ inline void copyChain(ChainCode* source, ChainCode* dest, 
    int start, int length) {

    for (
        int rowIdx = max(0, start); // index can't be negative
        rowIdx < start + length;
        ++rowIdx
    ) {
        dest[rowIdx - start] = source[rowIdx];
    }
}

__device__ inline HBusItem loadHBus(SharedMemory* sharedMemory, 
    HBusItem* globalBus, int index, int iteration) {

    if (threadIdx.x == 0 || iteration == 0) {
        return globalBus[index];
    } else {
        return sharedMemory[threadIdx.x].hBusItem;
    }
}

__device__ inline void storeHBus(HBusItem item, SharedMemory* sharedMemory, 
    HBusItem* globalBus, int index, int last) {

    if (threadIdx.x == GPU_CNST.THREADS - 1 || last == 1) {
        globalBus[index] = item;
    } else {
        sharedMemory[threadIdx.x + 1].hBusItem = item;
    }
}

__device__ inline void swItemToHBus(SWItem* swItem, HBusItem* hBusItem) {
    hBusItem->deleteScr.score = swItem->currentScr[ALPHA - 1].score;
    hBusItem->deleteScr.affine = swItem->deleteAffineScr;
}

__device__ inline void swItemToVBus(SWItem* swItem, VBusItem* vBusItem) {

    vBusItem->matchScr = swItem->deleteScr.score;

    for (int alpha = 0; alpha < ALPHA; ++alpha) {
        vBusItem->insertScr[alpha].score = swItem->currentScr[alpha].score;
        vBusItem->insertScr[alpha].affine = swItem->currentScr[alpha].affine;
    }
}

__device__ inline void hBusToSWItem(SWItem* swItem, HBusItem* hBusItem) {
    swItem->deleteScr.score = hBusItem->deleteScr.score;
    swItem->deleteScr.affine = hBusItem->deleteScr.affine;
}

__device__ inline void vBusToSWItem(SWItem* swItem, VBusItem* vBusItem) {

    swItem->matchScr = vBusItem->matchScr;

    for (int alpha = 0; alpha < ALPHA; ++alpha) {
        swItem->insertScr[alpha].score = vBusItem->insertScr[alpha].score;
        swItem->insertScr[alpha].affine = vBusItem->insertScr[alpha].affine;
    }
}

__device__ inline void swItemProgresses(SWItem* swItem) {

    swItem->matchScr = swItem->deleteScr.score;

    for (int alpha = 0; alpha < ALPHA; ++alpha) {
        swItem->insertScr[alpha].score = swItem->currentScr[alpha].score;
        swItem->insertScr[alpha].affine = swItem->currentScr[alpha].affine;
    }
}

__device__ inline void updateResult(SWItem* swItem, Result* result, int row, 
    int column) {

    for (int alphaIdx = 0; alphaIdx < ALPHA; ++alphaIdx) {
        if (swItem->currentScr[alphaIdx].score > result->score) {
            if (row + alphaIdx < GPU_CNST.REAL_ROWS) {
                result->score = swItem->currentScr[alphaIdx].score;
                result->row = row + alphaIdx;
                result->column = column;
            }
        }
    }
}

__device__ static inline void swItemSolve(SWItem* swItem, ChainCode* rowCodes, 
    ChainCode columnCode, const int type) {

    MatcherScore matching;

    MatcherScore score;
    MatcherScore insertAffineScr;
    MatcherScore matchScr = swItem->matchScr;
    MatcherScore deleteAffineScr = swItem->deleteScr.affine;
    MatcherScore deleteScr = swItem->deleteScr.score;

    for (int alpha = 0; alpha < ALPHA; ++alpha) {

        matching = getMatching(columnCode, rowCodes[alpha]);

        deleteAffineScr = max(
            deleteScr - GPU_CNST.GAP_OPEN, 
            deleteAffineScr - GPU_CNST.GAP_EXTEND
        );

        insertAffineScr = max(
            swItem->insertScr[alpha].score - GPU_CNST.GAP_OPEN, 
            swItem->insertScr[alpha].affine - GPU_CNST.GAP_EXTEND
        );

        if (type == SW){
            score = 0;
            score = max(score, matchScr + matching);
        }

        if (type == NW) {
            score = matchScr + matching;
        }

        score = max(score, deleteAffineScr);
        score = max(score, insertAffineScr);

        swItem->currentScr[alpha].score = score;
        swItem->currentScr[alpha].affine = insertAffineScr;

        matchScr = swItem->insertScr[alpha].score;
        deleteScr = swItem->currentScr[alpha].score;
    }

    swItem->deleteAffineScr = deleteAffineScr;
}

__device__ static inline void updateGlobalResult(Result* localResult, 
    Result* globalResults, SharedMemory* sharedMemory) {
    
    sharedMemory[threadIdx.x].result = *localResult; 
    __syncthreads();

    if (threadIdx.x) return;

    for (
        int resultIdx = 1;
        resultIdx < GPU_CNST.THREADS;
        ++resultIdx
    ) {
        if (localResult->score <= sharedMemory[resultIdx].result.score) {
            *localResult = sharedMemory[resultIdx].result;
        }
    }

    if (localResult->score > globalResults[blockIdx.x].score) {
        globalResults[blockIdx.x] = *localResult;
    }
}
