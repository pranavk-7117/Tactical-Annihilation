#ifndef NAVIGATIONEXCEPTIONS_H
#define NAVIGATIONEXCEPTIONS_H

#include <exception>
#include <string>
#include "Vector3i.h"

// ============================================================================
// [OOP CONCEPT: Custom Exception Hierarchy & Inheritance (Unit 6)]
// Exception classes for spatial boundary, obstacle, and AP movement validation.
// ============================================================================

class NavigationException : public std::exception {
protected:
    std::string message;

public:
    explicit NavigationException(const std::string& msg) : message(msg) {}
    virtual ~NavigationException() throw() {}
    virtual const char* what() const throw() override {
        return message.c_str();
    }
};

// Thrown when an requested coordinate lies outside the board boundaries
class OutOfBoundsException : public NavigationException {
public:
    OutOfBoundsException(const Vector3i& pos, int maxW, int maxD)
        : NavigationException("OutOfBoundsException: Coordinate " + [pos]() {
            return "(" + std::to_string(pos.getX()) + ", " + std::to_string(pos.getY()) + ", " + std::to_string(pos.getZ()) + ")";
        }() + " is outside grid boundaries [0.." + std::to_string(maxW - 1) + ", 0.." + std::to_string(maxD - 1) + "].") {}
};

// Thrown when attempting to move into or end turn on a solid wall or occupied cell
class TileBlockedException : public NavigationException {
public:
    TileBlockedException(const Vector3i& pos, const std::string& reason)
        : NavigationException("TileBlockedException: Tile (" +
            std::to_string(pos.getX()) + ", " + std::to_string(pos.getY()) + ", " + std::to_string(pos.getZ()) +
            ") is blocked. Reason: " + reason) {}
};

// Thrown when no valid traversable route can connect start to destination
class PathNotFoundException : public NavigationException {
public:
    PathNotFoundException(const Vector3i& start, const Vector3i& dest)
        : NavigationException("PathNotFoundException: No traversable route exists between (" +
            std::to_string(start.getX()) + ", " + std::to_string(start.getZ()) + ") and (" +
            std::to_string(dest.getX()) + ", " + std::to_string(dest.getZ()) + ").") {}
};

// Thrown when unit does not possess enough Action Points (AP) for the planned path
class InsufficientAPException : public NavigationException {
private:
    int requiredAP;
    int availableAP;

public:
    InsufficientAPException(int required, int available)
        : NavigationException("InsufficientAPException: Movement requires " +
            std::to_string(required) + " AP, but operative only has " +
            std::to_string(available) + " AP remaining."),
          requiredAP(required), availableAP(available) {}

    int getRequiredAP() const { return requiredAP; }
    int getAvailableAP() const { return availableAP; }
};

#endif // NAVIGATIONEXCEPTIONS_H
