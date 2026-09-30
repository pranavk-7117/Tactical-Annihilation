#include "MapSerializer.h"
#include <fstream>
#include <iostream>
#include <ctime>

// [OOP CONCEPT: File Output Stream (std::ofstream)]
bool MapSerializer::saveMap(const std::string& filename, const GridMap3D& map) {
    std::ofstream outFile(filename, std::ios::out | std::ios::trunc);
    if (!outFile.is_open()) {
        std::cerr << "[MapSerializer] Error opening file for writing: " << filename << "\n";
        return false;
    }

    outFile << "# ACTION POINT ZERO MAP FILE\n";
    outFile << "WIDTH " << map.getWidth() << "\n";
    outFile << "DEPTH " << map.getDepth() << "\n";
    outFile << "HEIGHT " << map.getHeight() << "\n";
    outFile << "DATA\n";

    for (int z = 0; z < map.getDepth(); ++z) {
        for (int x = 0; x < map.getWidth(); ++x) {
            Vector3i pos(x, 0, z);
            const Tile& tile = map.getTile(pos);
            if (!tile.isWalkable()) {
                outFile << '#';
            } else {
                outFile << '.';
            }
        }
        outFile << "\n";
    }

    outFile.close();
    return true;
}

// [OOP CONCEPT: File Input Stream (std::ifstream)]
bool MapSerializer::loadMap(const std::string& filename, GridMap3D& map) {
    std::ifstream inFile(filename, std::ios::in);
    if (!inFile.is_open()) {
        std::cerr << "[MapSerializer] Error opening map file for reading: " << filename << "\n";
        return false;
    }

    std::string line;
    int w = 10, d = 10, h = 1;

    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("WIDTH", 0) == 0) {
            w = std::stoi(line.substr(6));
        } else if (line.rfind("DEPTH", 0) == 0) {
            d = std::stoi(line.substr(6));
        } else if (line.rfind("HEIGHT", 0) == 0) {
            h = std::stoi(line.substr(7));
        } else if (line.rfind("DATA", 0) == 0) {
            // Read grid matrix
            map = GridMap3D(w, d, h); // Invokes copy assignment
            for (int z = 0; z < d; ++z) {
                if (std::getline(inFile, line)) {
                    for (int x = 0; x < w && x < static_cast<int>(line.length()); ++x) {
                        Vector3i pos(x, 0, z);
                        if (line[x] == '#') {
                            map.setObstacle(pos, true);
                        } else {
                            map.setObstacle(pos, false);
                        }
                    }
                }
            }
            break;
        }
    }

    inFile.close();
    return true;
}

// [OOP CONCEPT: File Append Mode (std::ios::app)]
void MapSerializer::logNavigationEvent(const std::string& logFilename, const std::string& message) {
    std::ofstream logFile(logFilename, std::ios::out | std::ios::app);
    if (logFile.is_open()) {
        std::time_t now = std::time(nullptr);
        char timeBuf[32];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        logFile << "[" << timeBuf << "] [NAVIGATOR] " << message << "\n";
        logFile.close();
    }
}
