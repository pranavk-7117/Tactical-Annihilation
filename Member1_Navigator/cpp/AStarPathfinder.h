#ifndef ASTARPATHFINDER_H
#define ASTARPATHFINDER_H

#include "Vector3i.h"
#include "GridMap3D.h"
#include "IHeuristic.h"
#include "Heuristics.h"
#include "NavigationExceptions.h"
#include <vector>

// Internal Node structure for A* graph search
struct PathNode {
    Vector3i position;
    float gCost; // Cost from start
    float hCost; // Heuristic estimate to goal
    PathNode* parent;

    PathNode(Vector3i pos, float g, float h, PathNode* p = nullptr)
        : position(pos), gCost(g), hCost(h), parent(p) {}

    float fCost() const { return gCost + hCost; }
};

// ============================================================================
// [OOP CONCEPT: Abstraction, Run-time Polymorphism & Static Members]
// The core A* Pathfinding Engine for Member 1: The Navigator.
// Calculates optimal 4-directional paths avoiding obstacles and occupied tiles,
// enforces the 1-AP-per-tile rule, and computes reachable AP zones.
// ============================================================================
class AStarPathfinder {
private:
    // [OOP CONCEPT: Polymorphic Pointer to Abstract Base Class]
    IHeuristic* heuristic;
    bool ownsHeuristic;

    // [OOP CONCEPT: Static Data Member]
    static int totalPathsCalculated;

public:
    // [OOP CONCEPT: Parameterized Constructor]
    explicit AStarPathfinder(IHeuristic* h = nullptr);

    // Destructor
    ~AStarPathfinder();

    // Polymorphic Strategy Setter & Getter
    void setHeuristic(IHeuristic* h);
    IHeuristic* getHeuristic() const { return heuristic; }

    // [OOP CONCEPT: Static Member Function]
    static int getTotalPathsCalculated() { return totalPathsCalculated; }
    static void resetPathCounter() { totalPathsCalculated = 0; }

    // Core Pathfinding: Returns list of grid points from start to goal
    std::vector<Vector3i> findPath(
        const GridMap3D& map,
        const Vector3i& start,
        const Vector3i& goal,
        int movingTeamId = 1
    );

    // Turn Economy: 1 AP per tile calculation
    int calculatePathAPCost(const std::vector<Vector3i>& path) const;

    // AP Validation: Throws InsufficientAPException if cost > available
    void validateMovement(const std::vector<Vector3i>& path, int currentAP) const;

    // Reachable tile flood-fill: Returns all valid tiles reachable within current AP
    std::vector<Vector3i> getReachableTiles(
        const GridMap3D& map,
        const Vector3i& start,
        int currentAP,
        int movingTeamId = 1
    ) const;
};

#endif // ASTARPATHFINDER_H
