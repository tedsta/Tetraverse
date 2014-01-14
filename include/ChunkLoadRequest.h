#ifndef CHUNKLOADREQUEST_H
#define CHUNKLOADREQUEST_H

#include <SFML/System/Mutex.hpp>

class IGridData;

/// \brief A chunk load request. If this is running on a server, this loads the chunk from the grid
/// data file in a separate thread. If this is running on the client, it waits for the chunk data
/// to arrive from the server.
/// \note Must be locked before use and unlocked after use to ensure thread safety.
class ChunkLoadRequest
{
    public:
        ChunkLoadRequest();
        ~ChunkLoadRequest();

        void lock(){mMutex.lock();}
        void unlock(){mMutex.unlock();}

    private:
        IGridData* mGridData;

        sf::Mutex mMutex;
};

#endif // CHUNKLOADREQUEST_H
