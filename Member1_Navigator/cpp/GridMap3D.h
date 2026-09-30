#ifndef GRIDMAP3D_H
#define GRIDMAP3D_H

#include "Vector3i.h"
#include "Tile.h"
#include <vector>
#include <iostream>

// ============================================================================
// [OOP CONCEPT: Classes, Dynamic Memory Allocation ('new'/'delete'), Deep Copy]
// Manages the 10x10 3D Tactical Battlefield Grid.
// ============================================================================
class GridMap3D {
private:
    int width;   // X-dimension (default: 10)
    int depth;   // Z-dimension (default: 10)
    int height;  // Y-dimension (default: 1 for flat grid)
    Tile** grid; // 2D dynamic array of Tile pointers (heap allocated)

    // Helper to allocate grid memory
    void allocateGrid();
    // Helper to free grid memory
    void deallocateGrid();

public:
    // [OOP CONCEPT: Parameterized Constructor with Defaults]
    GridMap3D(int w = 10, int d = 10, int h = 1);

    // [OOP CONCEPT: Copy Constructor (Deep Copy)]
    GridMap3D(const GridMap3D& other);

    // [OOP CONCEPT: Copy Assignment Operator (Deep Copy)]
    GridMap3D& operator=(const GridMap3D& other);

    // [OOP CONCEPT: Destructor with Dynamic Memory Cleanup]
    virtual ~GridMap3D();

    // Dimensions
    int getWidth() const { return width; }
    int getDepth() const { return depth; }
    int getHeight() const { return height; }

    // Coordinate validation
    bool isInBounds(const Vector3i& pos) const;

    // Tile access (data encapsulation)
    Tile& getTile(const Vector3i& pos);
    const Tile& getTile(const Vector3i& pos) const;

    // Board configuration
    void setObstacle(const Vector3i& pos, bool isObstacle);
    void placeUnit(const Vector3i& pos, int unitId, int teamId);
    void removeUnit(const Vector3i& pos);

    // Traversal neighbors (4-directional orthogonal on X-Z plane)
    std::vector<Vector3i> getOrthogonalNeighbors(const Vector3i& pos) const;

    // Visual ASCII render of the battlefield
    void printMap(const Vector3i* activeUnitPos = nullptr, const std::vector<Vector3i>* highlightPath = nullptr) const;

    // [OOP CONCEPT: Friend Function for Stream Insertion]
    friend std::ostream& operator<<(std::ostream& os, const GridMap3D& map);
};

#endif // GRIDMAP3D_H
