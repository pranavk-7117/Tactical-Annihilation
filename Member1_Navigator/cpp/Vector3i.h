#ifndef VECTOR3I_H
#define VECTOR3I_H

#include <cmath>
#include <algorithm>
#include <iostream>

// ============================================================================
// [OOP CONCEPT: Classes, Encapsulation, Operator Overloading & Friend Function]
// Represents a 3D Integer Grid Coordinate for the battlefield navigation.
// ============================================================================
class Vector3i {
private:
    int x;
    int y;
    int z;

public:
    // [OOP CONCEPT: Default & Parameterized Constructor with Default Arguments]
    Vector3i(int xVal = 0, int yVal = 0, int zVal = 0) : x(xVal), y(yVal), z(zVal) {}

    // [OOP CONCEPT: Copy Constructor]
    Vector3i(const Vector3i& other) : x(other.x), y(other.y), z(other.z) {}

    // Destructor
    ~Vector3i() {}

    // Getters (Data Abstraction & Encapsulation)
    int getX() const { return x; }
    int getY() const { return y; }
    int getZ() const { return z; }

    // Setters
    void setX(int xVal) { x = xVal; }
    void setY(int yVal) { y = yVal; }
    void setZ(int zVal) { z = zVal; }

    // [OOP CONCEPT: Operator Overloading (Compile-Time Polymorphism)]
    bool operator==(const Vector3i& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    bool operator!=(const Vector3i& other) const {
        return !(*this == other);
    }

    Vector3i operator+(const Vector3i& other) const {
        return Vector3i(x + other.x, y + other.y, z + other.z);
    }

    Vector3i operator-(const Vector3i& other) const {
        return Vector3i(x - other.x, y - other.y, z - other.z);
    }

    // Strict weak ordering for std::set, std::map, and priority queues
    bool operator<(const Vector3i& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }

    // Copy assignment operator
    Vector3i& operator=(const Vector3i& other) {
        if (this != &other) {
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
        }
        return *this;
    }

    // Distance metrics
    int manhattanDistance(const Vector3i& other) const {
        return std::abs(x - other.x) + std::abs(y - other.y) + std::abs(z - other.z);
    }

    int chebyshevDistance(const Vector3i& other) const {
        return std::max(std::abs(x - other.x), std::max(std::abs(y - other.y), std::abs(z - other.z)));
    }

    float euclideanDistance(const Vector3i& other) const {
        float dx = static_cast<float>(x - other.x);
        float dy = static_cast<float>(y - other.y);
        float dz = static_cast<float>(z - other.z);
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    // 4-directional adjacency check on flat X-Z plane (North, South, East, West)
    bool isAdjacent4(const Vector3i& other) const {
        if (y != other.y) return false;
        int dx = std::abs(x - other.x);
        int dz = std::abs(z - other.z);
        return (dx + dz) == 1;
    }

    // [OOP CONCEPT: Static Methods & Factory Presets]
    static Vector3i zero() { return Vector3i(0, 0, 0); }
    static Vector3i up() { return Vector3i(0, 1, 0); }
    static Vector3i down() { return Vector3i(0, -1, 0); }

    // [OOP CONCEPT: Friend Function for Stream Insertion]
    friend std::ostream& operator<<(std::ostream& os, const Vector3i& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

#endif // VECTOR3I_H
