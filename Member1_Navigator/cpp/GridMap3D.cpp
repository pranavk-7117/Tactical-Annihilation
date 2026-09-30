#include "GridMap3D.h"
#include <iomanip>

// Helper to allocate 2D array of Tiles on the heap
void GridMap3D::allocateGrid() {
    grid = new Tile*[width];
    for (int x = 0; x < width; ++x) {
        grid[x] = new Tile[depth];
        for (int z = 0; z < depth; ++z) {
            grid[x][z] = Tile(Vector3i(x, 0, z), true, 1);
        }
    }
}

// Helper to clean up dynamically allocated heap memory
void GridMap3D::deallocateGrid() {
    if (grid != nullptr) {
        for (int x = 0; x < width; ++x) {
            delete[] grid[x];
        }
        delete[] grid;
        grid = nullptr;
    }
}

// [OOP CONCEPT: Parameterized Constructor]
GridMap3D::GridMap3D(int w, int d, int h)
    : width(w), depth(d), height(h), grid(nullptr) {
    allocateGrid();
}

// [OOP CONCEPT: Copy Constructor (Deep Copy)]
GridMap3D::GridMap3D(const GridMap3D& other)
    : width(other.width), depth(other.depth), height(other.height), grid(nullptr) {
    allocateGrid();
    for (int x = 0; x < width; ++x) {
        for (int z = 0; z < depth; ++z) {
            grid[x][z] = other.grid[x][z];
        }
    }
}

// [OOP CONCEPT: Copy Assignment Operator (Deep Copy)]
GridMap3D& GridMap3D::operator=(const GridMap3D& other) {
    if (this != &other) {
        deallocateGrid();
        width = other.width;
        depth = other.depth;
        height = other.height;
        allocateGrid();
        for (int x = 0; x < width; ++x) {
            for (int z = 0; z < depth; ++z) {
                grid[x][z] = other.grid[x][z];
            }
        }
    }
    return *this;
}

// [OOP CONCEPT: Destructor]
GridMap3D::~GridMap3D() {
    deallocateGrid();
}

bool GridMap3D::isInBounds(const Vector3i& pos) const {
    return (pos.getX() >= 0 && pos.getX() < width &&
            pos.getY() >= 0 && pos.getY() < height &&
            pos.getZ() >= 0 && pos.getZ() < depth);
}

Tile& GridMap3D::getTile(const Vector3i& pos) {
    return grid[pos.getX()][pos.getZ()];
}

const Tile& GridMap3D::getTile(const Vector3i& pos) const {
    return grid[pos.getX()][pos.getZ()];
}

void GridMap3D::setObstacle(const Vector3i& pos, bool isObstacle) {
    if (isInBounds(pos)) {
        grid[pos.getX()][pos.getZ()].setWalkable(!isObstacle);
    }
}

void GridMap3D::placeUnit(const Vector3i& pos, int unitId, int teamId) {
    if (isInBounds(pos)) {
        grid[pos.getX()][pos.getZ()].occupy(unitId, teamId);
    }
}

void GridMap3D::removeUnit(const Vector3i& pos) {
    if (isInBounds(pos)) {
        grid[pos.getX()][pos.getZ()].vacate();
    }
}

std::vector<Vector3i> GridMap3D::getOrthogonalNeighbors(const Vector3i& pos) const {
    std::vector<Vector3i> neighbors;
    // 4 orthogonal directions on the X-Z plane: North, South, East, West
    const int dx[4] = { 0, 0, 1, -1 };
    const int dz[4] = { -1, 1, 0, 0 };

    for (int i = 0; i < 4; ++i) {
        Vector3i n(pos.getX() + dx[i], pos.getY(), pos.getZ() + dz[i]);
        if (isInBounds(n)) {
            neighbors.push_back(n);
        }
    }
    return neighbors;
}

void GridMap3D::printMap(const Vector3i* activeUnitPos, const std::vector<Vector3i>* highlightPath) const {
    std::cout << "    ";
    for (int x = 0; x < width; ++x) {
        std::cout << std::setw(2) << x << " ";
    }
    std::cout << "\n   +";
    for (int x = 0; x < width; ++x) std::cout << "---";
    std::cout << "+\n";

    for (int z = 0; z < depth; ++z) {
        std::cout << std::setw(2) << z << " | ";
        for (int x = 0; x < width; ++x) {
            Vector3i current(x, 0, z);
            char symbol = '.';

            const Tile& tile = grid[x][z];
            if (!tile.isWalkable()) {
                symbol = '#'; // Obstacle
            } else if (activeUnitPos && *activeUnitPos == current) {
                symbol = '@'; // Active moving unit
            } else if (tile.isOccupied()) {
                symbol = (tile.getOccupyingTeamId() == 1) ? 'A' : 'E'; // Friendly Ally vs Enemy
            }

            // Path highlight overlay
            if (highlightPath && symbol == '.') {
                for (size_t p = 0; p < highlightPath->size(); ++p) {
                    if ((*highlightPath)[p] == current) {
                        symbol = '*'; // Waypoint
                        break;
                    }
                }
            }

            std::cout << " " << symbol << " ";
        }
        std::cout << "|\n";
    }

    std::cout << "   +";
    for (int x = 0; x < width; ++x) std::cout << "---";
    std::cout << "+\n";
    std::cout << "Legend: [.] Open  [#] Wall  [A] Ally  [E] Enemy  [@] Active Unit  [*] Path\n";
}

std::ostream& operator<<(std::ostream& os, const GridMap3D& map) {
    os << "GridMap3D [" << map.width << "x" << map.depth << ", Height=" << map.height << "]";
    return os;
}
