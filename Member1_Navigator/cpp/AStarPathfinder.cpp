#include "AStarPathfinder.h"
#include "PriorityQueue.h"
#include <queue>
#include <map>
#include <set>
#include <algorithm>

// [OOP CONCEPT: Static Data Member Definition]
int AStarPathfinder::totalPathsCalculated = 0;

// Custom comparator for PathNode pointers (Min-Heap based on fCost)
struct NodeComparator {
    bool operator()(const PathNode* a, const PathNode* b) const {
        if (std::abs(a->fCost() - b->fCost()) < 0.0001f) {
            return a->hCost > b->hCost; // Tie-breaker: prefer node closer to goal
        }
        return a->fCost() > b->fCost();
    }
};

AStarPathfinder::AStarPathfinder(IHeuristic* h)
    : heuristic(h), ownsHeuristic(false) {
    if (heuristic == nullptr) {
        // Default to Manhattan heuristic for 4-directional grid
        heuristic = new ManhattanHeuristic();
        ownsHeuristic = true;
    }
}

AStarPathfinder::~AStarPathfinder() {
    if (ownsHeuristic && heuristic != nullptr) {
        delete heuristic;
        heuristic = nullptr;
    }
}

void AStarPathfinder::setHeuristic(IHeuristic* h) {
    if (ownsHeuristic && heuristic != nullptr) {
        delete heuristic;
    }
    heuristic = h;
    ownsHeuristic = false; // External ownership
}

int AStarPathfinder::calculatePathAPCost(const std::vector<Vector3i>& path) const {
    if (path.empty()) return 0;
    // 1 AP per tile moved (start tile costs 0 AP)
    return static_cast<int>(path.size() - 1);
}

void AStarPathfinder::validateMovement(const std::vector<Vector3i>& path, int currentAP) const {
    int requiredAP = calculatePathAPCost(path);
    if (requiredAP > currentAP) {
        throw InsufficientAPException(requiredAP, currentAP);
    }
}

std::vector<Vector3i> AStarPathfinder::findPath(
    const GridMap3D& map,
    const Vector3i& start,
    const Vector3i& goal,
    int movingTeamId
) {
    // 1. Boundary checking
    if (!map.isInBounds(start)) {
        throw OutOfBoundsException(start, map.getWidth(), map.getDepth());
    }
    if (!map.isInBounds(goal)) {
        throw OutOfBoundsException(goal, map.getWidth(), map.getDepth());
    }

    // 2. Goal validation: cannot stop on an obstacle or an occupied tile
    const Tile& goalTile = map.getTile(goal);
    if (!goalTile.isWalkable()) {
        throw TileBlockedException(goal, "Destination is a solid obstacle / wall.");
    }
    if (goalTile.isOccupied()) {
        std::string occupant = (goalTile.getOccupyingTeamId() == movingTeamId) ? "Allied unit" : "Enemy unit";
        throw TileBlockedException(goal, "Destination is occupied by an " + occupant + ".");
    }

    // Trivial case: already at destination
    if (start == goal) {
        return { start };
    }

    // Priority queue for Open Set
    std::priority_queue<PathNode*, std::vector<PathNode*>, NodeComparator> openSet;
    std::map<Vector3i, PathNode*> allNodes; // For memory tracking and fast lookup
    std::set<Vector3i> closedSet;

    // Create starting node
    float startH = heuristic->calculate(start, goal);
    PathNode* startNode = new PathNode(start, 0.0f, startH, nullptr);
    allNodes[start] = startNode;
    openSet.push(startNode);

    PathNode* goalNode = nullptr;

    while (!openSet.empty()) {
        PathNode* current = openSet.top();
        openSet.pop();

        // Check if destination reached
        if (current->position == goal) {
            goalNode = current;
            break;
        }

        closedSet.insert(current->position);

        // Expand 4 orthogonal neighbors
        std::vector<Vector3i> neighbors = map.getOrthogonalNeighbors(current->position);
        for (const Vector3i& neighborPos : neighbors) {
            // Skip if already evaluated in closed set
            if (closedSet.find(neighborPos) != closedSet.end()) {
                continue;
            }

            const Tile& neighborTile = map.getTile(neighborPos);

            // Traversal rule: can pass through open tiles and friendly units, but NOT enemies or walls
            if (!neighborTile.canTraverse(movingTeamId)) {
                continue;
            }

            float stepCost = static_cast<float>(neighborTile.getMovementCost());
            float tentativeG = current->gCost + stepCost;

            auto it = allNodes.find(neighborPos);
            if (it == allNodes.end()) {
                // Discover new node
                float h = heuristic->calculate(neighborPos, goal);
                PathNode* neighborNode = new PathNode(neighborPos, tentativeG, h, current);
                allNodes[neighborPos] = neighborNode;
                openSet.push(neighborNode);
            } else if (tentativeG < it->second->gCost) {
                // Found a better path to this existing node
                it->second->gCost = tentativeG;
                it->second->parent = current;
                openSet.push(it->second);
            }
        }
    }

    std::vector<Vector3i> path;
    if (goalNode != nullptr) {
        // Reconstruct path backwards from goal to start
        PathNode* curr = goalNode;
        while (curr != nullptr) {
            path.push_back(curr->position);
            curr = curr->parent;
        }
        std::reverse(path.begin(), path.end());
        ++totalPathsCalculated;
    }

    // Clean up dynamic memory
    for (auto& pair : allNodes) {
        delete pair.second;
    }

    if (path.empty()) {
        throw PathNotFoundException(start, goal);
    }

    return path;
}

std::vector<Vector3i> AStarPathfinder::getReachableTiles(
    const GridMap3D& map,
    const Vector3i& start,
    int currentAP,
    int movingTeamId
) const {
    std::vector<Vector3i> reachable;
    if (currentAP <= 0 || !map.isInBounds(start)) {
        return reachable;
    }

    // Dijkstra / BFS search within AP budget
    std::queue<std::pair<Vector3i, int>> frontier;
    std::map<Vector3i, int> costSoFar;

    frontier.push({ start, 0 });
    costSoFar[start] = 0;

    while (!frontier.empty()) {
        auto current = frontier.front();
        frontier.pop();

        Vector3i currPos = current.first;
        int currCost = current.second;

        // Valid destination tile: within AP, not the start tile, and can be stopped on
        if (currCost > 0 && map.getTile(currPos).canStop()) {
            reachable.push_back(currPos);
        }

        if (currCost >= currentAP) {
            continue;
        }

        // Expand neighbors
        std::vector<Vector3i> neighbors = map.getOrthogonalNeighbors(currPos);
        for (const Vector3i& nextPos : neighbors) {
            const Tile& nextTile = map.getTile(nextPos);

            // Must be traversable (open or friendly unit)
            if (!nextTile.canTraverse(movingTeamId)) {
                continue;
            }

            int newCost = currCost + nextTile.getMovementCost();
            if (newCost <= currentAP) {
                if (costSoFar.find(nextPos) == costSoFar.end() || newCost < costSoFar[nextPos]) {
                    costSoFar[nextPos] = newCost;
                    frontier.push({ nextPos, newCost });
                }
            }
        }
    }

    return reachable;
}
