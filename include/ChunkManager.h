#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <vector>

#include "GridChunk.h"

class ChunkManager
{
    public:
        ChunkManager();
        ~ChunkManager();

    private:
        std::vector<GridChunk> mChunks; // Loaded chunks
};

#endif // CHUNKMANAGER_H
