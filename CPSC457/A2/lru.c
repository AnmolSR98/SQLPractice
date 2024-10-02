#include "main.h"

// function to the frame to update
// need to account for the page that will be doing the replacing
// identical to the one for FIFO
int getFrameToUpdateLRU(frame** frameArray, int numFrames, int pageNumber) {

    int i;
    int oldestFrameIndex;
    int oldest = largeNumber;
    
    // loop to cycle through the various frames to see if any of them are free
    for (i = 0; i < numFrames; i++) {
        // return if one is found
        if (frameArray[i]->currentPage == NULL) {
            return i;
        }

        else if (frameArray[i]->currentPage->pageNumber == pageNumber) {
            return i;
        }

        else {
            frameArray[i]->pageFaults += 1;
        }

        // check if that frame is the oldest
        if (frameArray[i]->timeArrived < oldest) {
            oldest = frameArray[i]->timeArrived;
            oldestFrameIndex = i;
        }
    }
    
    // if no free page was found then return the oldest frame
    return oldestFrameIndex;
}

void LRU(page** pageArray, int numFrames, int numPages) {

    // creating a new array of frames and filling it
    frame** frameArray = malloc(sizeof(frame) * numFrames);
    int i;
    for (i = 0; i < numFrames; i++) {
        frameArray[i] = createFrame(i+1);
    }

    int frameToUpdate;
    page* currentPage = (page*) malloc(sizeof(page));
    frame* currentFrame = (frame*) malloc(sizeof(frame));

    // now moving onto actually simulating checking the frames
    for (i = 0; i < numPages; i++) {

        // get the current page from the queue
        currentPage = pageArray[i];

        // gonna create a function to check for the index of the first free frame
        currentFrame = frameArray[getFrameToUpdateLRU(frameArray, numFrames, currentPage->pageNumber)];

        // only changes for LRU is to change timeArrived only when the pageNumber is different

        // adding on a update if the frame was null before or if the current page has a dirty bit
        // in the case of a null frame or change in page number, add on a write back
        if ((currentFrame->currentPage == NULL) || (currentFrame->currentPage->pageNumber != currentPage->pageNumber)) {
            currentFrame->totalWriteBacks += 1;
        }

        // also do this in the case for identical page numbers but with a dirty bit
        else if ((currentFrame->currentPage->pageNumber == currentPage->pageNumber) && (currentPage->dirty == 1)) {
            currentFrame->totalWriteBacks += 1;
        }

        currentFrame->timeArrived = i;
        currentFrame->currentPage = currentPage;
    }

    printTable(frameArray, numFrames);
    printf("%d\n", frameArray[0]->currentPage->pageNumber);
}