/******************************************************************************

 facet.cpp

 Contains methods for facets of an object that is displayed

 Author: Tim Menninger

******************************************************************************/
#include "facet.h"

using namespace std;
using namespace Eigen;


/*
 facet::inView

 This takes a list of indices of vertices that are out of view (which means
 they should not be displayed).  This function returns true if none of the
 facet's vertices are in the list.  It assumes the list is in ascending order
 and thus performs a binary search.

 Arguments: vector<int> outOfView - Vector of indices of vertices that are not
                in the camera's view

 Returns:   (bool) - True if the facet is in view, i.e. not in the argued
                vector, and false otehrwise.
*/
bool facet::inView
(
    vector<int>     outOfView
)
{
    // All vertices are in view
    if (outOfView.size() == 0)
        return true;
    // Assuming the vector is sorted, because indices were pushed as they were
    // passed, which was in increasing order.  Thus, we will do a good old
    // fashioned binary search
    int lowIdx, idx, highIdx;

    int target, val;

    // Will perform on all three vertices.
    int vertices[3] = { this->v1, this->v2, this->v3 };
    for (int i = 0; i < 3; ++i) {
        lowIdx = 0;
        highIdx = outOfView.size() - 1;

        target = vertices[i];
        // Searching for the i'th vertex
        while (highIdx - lowIdx > 0) {
            idx = (highIdx + lowIdx) / 2;
            val = outOfView[idx];

            // If we found a match, the vertex facet is out of view
            if (target == val)
                return false;

            // If the bound separation was 1, then we necessarily checked the
            // low one, and always will because floor((2x+1)/2) = x, so check
            // this boundary case by comparing to the value at the high index
            if (lowIdx == idx && outOfView[highIdx] == val)
                return false;
            else if (lowIdx == idx)
                break;

            // Update our bounds
            highIdx = target < val ? idx : highIdx;
            lowIdx  = target > val ? idx : lowIdx;
        }
    }

    // No vertex found in our out of view list
    return true;
}
