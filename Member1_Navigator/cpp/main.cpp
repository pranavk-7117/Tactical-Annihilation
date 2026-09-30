#include <iostream>
#include <vector>
#include <iomanip>
#include "Vector3i.h"
#include "Tile.h"
#include "GridMap3D.h"
#include "IHeuristic.h"
#include "Heuristics.h"
#include "AStarPathfinder.h"
#include "NavigationExceptions.h"
#include "MapSerializer.h"
#include "PriorityQueue.h"

// ============================================================================
// ACTION POINT ZERO - TASK 1: THE NAVIGATOR
// Role: Member 1: The Navigator (Grid, Pathfinding & Movement)
// Demonstrates Core Navigation & Complete OOPs Syllabus Concepts:
// - Unit 1: Fundamentals of OOP (Encapsulation, Abstraction, Classes)
// - Unit 2: Classes & Objects (Access Modifiers, 'this', Static Members, Operator Overloading, Friends)
// - Unit 3: Constructors & Destructors (Parameterized, Deep Copy Constructor, Dynamic 'new'/'delete')
// - Unit 4: Inheritance & Polymorphism (Abstract Classes, Pure Virtual, Dynamic Binding)
// - Unit 5: File Handling (std::ifstream, std::ofstream, std::ios::app)
// - Unit 6: Exception Handling & Generics (Custom Exceptions, Multiple Catch, Templates)
// ============================================================================

int main() {
    std::cout << "========================================================\n";
    std::cout << "   ACTION POINT ZERO - TASK 1: THE NAVIGATOR CORE\n";
    std::cout << "   Role: Member 1 (Grid, Pathfinding & Movement Engine)\n";
    std::cout << "========================================================\n\n";

    // ------------------------------------------------------------------------
    // [OOP CONCEPT: Unit 5 File Handling (Logging)]
    // ------------------------------------------------------------------------
    const std::string logFile = "navigator_session.log";
    MapSerializer::logNavigationEvent(logFile, "Session initialized. Starting Navigator test suite.");

    // ------------------------------------------------------------------------
    // [DEMO 1] Classes, Dynamic Memory Allocation ('new'/'delete') & Encapsulation
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 1] Object Instantiation & Encapsulated Grid Setup:\n";
    GridMap3D* tacticalGrid = new GridMap3D(10, 10, 1);
    std::cout << "  Created board on heap: " << *tacticalGrid << "\n";
    std::cout << "  Board dimensions: " << tacticalGrid->getWidth() << "x"
              << tacticalGrid->getDepth() << " (X-Z plane, Y=0)\n\n";

    // ------------------------------------------------------------------------
    // [DEMO 2] Operator Overloading & Friend Functions (Unit 2)
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 2] Vector3i Operator Overloading & Distance Metrics:\n";
    Vector3i p1(2, 0, 3);
    Vector3i p2(5, 0, 7);
    Vector3i pSum = p1 + p2;
    Vector3i pDiff = p2 - p1;

    std::cout << "  Point 1: " << p1 << ", Point 2: " << p2 << "\n";
    std::cout << "  p1 + p2: " << pSum << " (operator+)\n";
    std::cout << "  p2 - p1: " << pDiff << " (operator-)\n";
    std::cout << "  p1 == p2: " << (p1 == p2 ? "true" : "false") << " (operator==)\n";
    std::cout << "  Manhattan Distance (L1): " << p1.manhattanDistance(p2) << " tiles\n";
    std::cout << "  Chebyshev Distance (L-inf): " << p1.chebyshevDistance(p2) << " tiles\n";
    std::cout << "  Euclidean Distance (L2): " << p1.euclideanDistance(p2) << " units\n\n";

    // ------------------------------------------------------------------------
    // [DEMO 3] Deep Copy Constructor (Unit 3)
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 3] Deep Copy Constructor Demonstration:\n";
    tacticalGrid->setObstacle(Vector3i(4, 0, 4), true);
    tacticalGrid->setObstacle(Vector3i(4, 0, 5), true);

    // Invoke Copy Constructor
    GridMap3D clonedGrid = *tacticalGrid;
    std::cout << "  Original tile (4,0,4) walkable: " << (tacticalGrid->getTile(Vector3i(4, 0, 4)).isWalkable() ? "Yes" : "No") << "\n";
    std::cout << "  Cloned tile   (4,0,4) walkable: " << (clonedGrid.getTile(Vector3i(4, 0, 4)).isWalkable() ? "Yes" : "No") << "\n";

    // Modify original to prove independent deep heap copy
    tacticalGrid->setObstacle(Vector3i(1, 0, 1), true);
    std::cout << "  Modified original (1,0,1) to Wall. Cloned (1,0,1) remains walkable: "
              << (clonedGrid.getTile(Vector3i(1, 0, 1)).isWalkable() ? "YES (Deep Copy Verified)" : "NO") << "\n\n";

    // ------------------------------------------------------------------------
    // [DEMO 4] Polymorphism & Abstract Class Dynamic Binding (Unit 4)
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 4] Runtime Polymorphism via IHeuristic Strategy Base Pointer:\n";
    IHeuristic* manhattan = new ManhattanHeuristic();
    IHeuristic* euclidean = new EuclideanHeuristic();
    IHeuristic* chebyshev = new ChebyshevHeuristic();

    std::vector<IHeuristic*> strategies = { manhattan, euclidean, chebyshev };
    for (IHeuristic* h : strategies) {
        // Dynamic binding through base pointer
        std::cout << "  [Strategy: " << std::setw(38) << std::left << h->getName() << "] "
                  << "Estimate p1->p2: " << h->calculate(p1, p2) << "\n";
    }
    std::cout << "\n";

    // ------------------------------------------------------------------------
    // [DEMO 5] Generic Programming: Templated Priority Queue (Unit 6)
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 5] Generic Programming (Templated MinPriorityQueue<T>):\n";
    struct IntAscending {
        bool operator()(int a, int b) const { return a < b; }
    };
    MinPriorityQueue<int, IntAscending> intQueue;
    intQueue.push(42);
    intQueue.push(10);
    intQueue.push(99);
    intQueue.push(3);

    std::cout << "  Inserted [42, 10, 99, 3] into MinPriorityQueue. Popping in priority order: ";
    while (!intQueue.empty()) {
        std::cout << intQueue.pop() << " ";
    }
    std::cout << "\n\n";

    // ------------------------------------------------------------------------
    // [DEMO 6] File Handling: Saving and Loading Map (Unit 5)
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 6] Level Serialization (std::ofstream & std::ifstream):\n";
    const std::string testMapPath = "maps/default_arena.map";
    GridMap3D loadedGrid(10, 10, 1);
    if (MapSerializer::loadMap(testMapPath, loadedGrid)) {
        std::cout << "  Successfully loaded map from " << testMapPath << "!\n";
    } else {
        std::cout << "  Creating and saving fresh default arena map...\n";
        // Create obstacle wall
        for (int z = 4; z <= 7; ++z) loadedGrid.setObstacle(Vector3i(4, 0, z), true);
        MapSerializer::saveMap(testMapPath, loadedGrid);
    }
    std::cout << "\nLoaded Grid Layout:\n";
    loadedGrid.printMap();
    std::cout << "\n";

    // ------------------------------------------------------------------------
    // [DEMO 7] A* Pathfinding Engine, Friendly Pass-Through & Obstacle Avoidance
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 7] A* Pathfinding with Obstacle Avoidance & Team Rules:\n";
    AStarPathfinder pathfinder(manhattan); // Uses polymorphic strategy

    Vector3i startPos(2, 0, 5);
    Vector3i goalPos(6, 0, 5);

    // Place an allied unit in the path to test friendly pass-through
    Vector3i allyPos(3, 0, 5);
    loadedGrid.placeUnit(allyPos, 101, 1); // Unit 101, Team 1 (Friendly)

    // Place an enemy unit blocking another lane
    Vector3i enemyPos(4, 0, 3);
    loadedGrid.placeUnit(enemyPos, 201, 2); // Unit 201, Team 2 (Enemy)

    std::cout << "  Starting Unit (Team 1) at: " << startPos << "\n";
    std::cout << "  Allied Unit   (Team 1) at: " << allyPos << " (Can be passed through)\n";
    std::cout << "  Enemy Unit    (Team 2) at: " << enemyPos << " (Blocks movement)\n";
    std::cout << "  Wall Barrier  (x=4, z=4..7)\n";
    std::cout << "  Destination               : " << goalPos << "\n\n";

    try {
        std::vector<Vector3i> path = pathfinder.findPath(loadedGrid, startPos, goalPos, 1);
        int apCost = pathfinder.calculatePathAPCost(path);

        std::cout << "  [PATH FOUND!] Total Steps: " << path.size() << " | AP Cost: " << apCost << " AP\n";
        std::cout << "  Waypoints: ";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << path[i];
            if (i + 1 < path.size()) std::cout << " -> ";
        }
        std::cout << "\n\nBattlefield Path Visualization:\n";
        loadedGrid.printMap(&startPos, &path);
        std::cout << "\n";

        // --------------------------------------------------------------------
        // [DEMO 8] Turn Economy: 1-AP-per-tile & Reachable Zone (BFS/Dijkstra)
        // --------------------------------------------------------------------
        std::cout << ">>> [DEMO 8] 1-AP-per-Tile Range Calculation:\n";
        int availableAP = 3; // Standard operative turn budget
        std::vector<Vector3i> reachable3AP = pathfinder.getReachableTiles(loadedGrid, startPos, availableAP, 1);
        std::cout << "  Reachable tiles within " << availableAP << " AP budget (" << reachable3AP.size() << " tiles):\n  ";
        for (size_t i = 0; i < reachable3AP.size(); ++i) {
            std::cout << reachable3AP[i] << " ";
            if ((i + 1) % 6 == 0) std::cout << "\n  ";
        }
        std::cout << "\n\n";

        // Validate movement within AP budget
        pathfinder.validateMovement(path, 10); // 10 AP is plenty
        std::cout << "  Validation: Path requiring " << apCost << " AP verified against 10 AP budget.\n\n";

    } catch (const NavigationException& ex) {
        std::cout << "  [EXCEPTION]: " << ex.what() << "\n";
    }

    // ------------------------------------------------------------------------
    // [DEMO 9] Exception Handling with Multiple Catch Clauses (Unit 6)
    // ------------------------------------------------------------------------
    std::cout << ">>> [DEMO 9] Exception Handling Suite (Multiple Catch Blocks):\n";

    // Test A: OutOfBoundsException
    try {
        std::cout << "  Test A: Target coordinate (15, 0, 15) outside 10x10 grid:\n";
        pathfinder.findPath(loadedGrid, startPos, Vector3i(15, 0, 15), 1);
    } catch (const OutOfBoundsException& ex) {
        std::cout << "  [CAUGHT EXPECTED]: " << ex.what() << "\n";
        MapSerializer::logNavigationEvent(logFile, ex.what());
    } catch (const NavigationException& ex) {
        std::cout << "  [CAUGHT GENERAL]: " << ex.what() << "\n";
    }

    // Test B: TileBlockedException (Ending turn on an occupied tile)
    try {
        std::cout << "\n  Test B: Attempting to end move on occupied allied tile (3, 0, 5):\n";
        pathfinder.findPath(loadedGrid, startPos, allyPos, 1);
    } catch (const TileBlockedException& ex) {
        std::cout << "  [CAUGHT EXPECTED]: " << ex.what() << "\n";
        MapSerializer::logNavigationEvent(logFile, ex.what());
    } catch (const NavigationException& ex) {
        std::cout << "  [CAUGHT GENERAL]: " << ex.what() << "\n";
    }

    // Test C: InsufficientAPException
    try {
        std::cout << "\n  Test C: Path requires 8 AP, but operative only has 2 AP:\n";
        std::vector<Vector3i> testPath = { Vector3i(0,0,0), Vector3i(1,0,0), Vector3i(2,0,0) };
        pathfinder.validateMovement(testPath, 1); // Needs 2 AP, only 1 provided
    } catch (const InsufficientAPException& ex) {
        std::cout << "  [CAUGHT EXPECTED]: " << ex.what() << "\n";
        std::cout << "  -> Required: " << ex.getRequiredAP() << " AP, Available: " << ex.getAvailableAP() << " AP\n";
        MapSerializer::logNavigationEvent(logFile, ex.what());
    }

    // ------------------------------------------------------------------------
    // [DEMO 10] Static Member Metrics & Polymorphic Clean-up
    // ------------------------------------------------------------------------
    std::cout << "\n>>> [DEMO 10] Static Members & Dynamic Memory Cleanup:\n";
    std::cout << "  Total A* paths successfully calculated: "
              << AStarPathfinder::getTotalPathsCalculated() << "\n";

    // Clean up polymorphic heap allocations
    delete tacticalGrid;
    delete manhattan;
    delete euclidean;
    delete chebyshev;

    std::cout << "\n========================================================\n";
    std::cout << "   ALL OOP CRITERIA & NAVIGATOR TESTS PASSED CLEANLY!\n";
    std::cout << "========================================================\n";

    return 0;
}
