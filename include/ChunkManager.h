#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include <set>

class GridChunk;

class ChunkManager
{
    public:
        ChunkManager();
        ~ChunkManager();

    private:
        std::set<GridChunk*> mChunks; // Loaded chunks
};

#endif // CHUNKMANAGER_H
