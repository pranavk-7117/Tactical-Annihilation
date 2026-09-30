#ifndef TILE_H
#define TILE_H

#include "Vector3i.h"
#include <iostream>

// ============================================================================
// [OOP CONCEPT: Encapsulation & Data Abstraction]
// Represents a single tile/cell on the tactical 3D battlefield grid.
// ============================================================================
class Tile {
private:
    Vector3i position;
    bool walkable;
    int occupyingUnitId;  // -1 if unoccupied
    int occupyingTeamId;  // 0 = none, 1 = friendly/allied, 2 = enemy
    int movementCost;      // Base AP cost (default: 1 AP per tile)

public:
    // [OOP CONCEPT: Parameterized Constructor with Defaults]
    Tile(Vector3i pos = Vector3i(), bool isWalkable = true, int cost = 1)
        : position(pos), walkable(isWalkable), occupyingUnitId(-1), occupyingTeamId(0), movementCost(cost) {}

    // Copy Constructor
    Tile(const Tile& other)
        : position(other.position),
          walkable(other.walkable),
          occupyingUnitId(other.occupyingUnitId),
          occupyingTeamId(other.occupyingTeamId),
          movementCost(other.movementCost) {}

    // Destructor
    ~Tile() {}

    // Getters
    Vector3i getPosition() const { return position; }
    bool isWalkable() const { return walkable; }
    bool isOccupied() const { return occupyingUnitId != -1; }
    int getOccupyingUnitId() const { return occupyingUnitId; }
    int getOccupyingTeamId() const { return occupyingTeamId; }
    int getMovementCost() const { return movementCost; }

    // Setters
    void setWalkable(bool val) { walkable = val; }
    void setMovementCost(int cost) { movementCost = cost; }

    // Unit placement
    void occupy(int unitId, int teamId) {
        occupyingUnitId = unitId;
        occupyingTeamId = teamId;
    }

    void vacate() {
        occupyingUnitId = -1;
        occupyingTeamId = 0;
    }

    // Traversal validation rule:
    // Units can traverse through friendly units (pass-through),
    // but enemy units and solid obstacles block traversal.
    bool canTraverse(int movingTeamId) const {
        if (!walkable) return false;
        if (!isOccupied()) return true;
        // Friendly unit can be passed through
        return occupyingTeamId == movingTeamId;
    }

    // Stopping validation: A unit can NEVER stop on an occupied tile or non-walkable tile
    bool canStop() const {
        return walkable && !isOccupied();
    }

    // [OOP CONCEPT: Friend Function for Debugging]
    friend std::ostream& operator<<(std::ostream& os, const Tile& t) {
        os << "Tile" << t.position << " [Walkable=" << (t.walkable ? "Yes" : "No")
           << ", Occupied=" << (t.isOccupied() ? "Team " + std::to_string(t.occupyingTeamId) : "None")
           << ", Cost=" << t.movementCost << " AP]";
        return os;
    }
};

#endif // TILE_H
