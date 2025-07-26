#pragma once

struct Frame {
    int frameNumber;
    bool isAllocated;
    int processPID;
    int pageNumber;
};

struct PageTableEntry {
    int pageNumber;
    int frameNumber;
    bool valid;
};
