#ifndef MAPSERIALIZER_H
#define MAPSERIALIZER_H

#include "GridMap3D.h"
#include <string>

// ============================================================================
// [OOP CONCEPT: File Handling (std::ifstream, std::ofstream, File Modes)]
// Handles loading and saving battlefield layouts and navigation replay logs.
// Meets the requirements for Unit 5 (File Handling).
// ============================================================================
class MapSerializer {
public:
    // Saves grid layout to a text file using std::ofstream
    static bool saveMap(const std::string& filename, const GridMap3D& map);

    // Loads grid layout from a text file using std::ifstream
    static bool loadMap(const std::string& filename, GridMap3D& map);

    // Logs navigation and movement execution in append mode (std::ios::app)
    static void logNavigationEvent(const std::string& logFilename, const std::string& message);
};

#endif // MAPSERIALIZER_H
