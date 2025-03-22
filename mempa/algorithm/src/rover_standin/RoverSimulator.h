#pragma once
#include "DemHandler.h"

namespace mempa
{
    class RoverSimulator
    {
    public:
        explicit RoverSimulator();

    protected:
    private:
        std::pair<int, int> currentPosition; /* The rover's current image-based coordinate position. */
    };
}