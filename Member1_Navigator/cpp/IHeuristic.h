#ifndef IHEURISTIC_H
#define IHEURISTIC_H

#include "Vector3i.h"
#include <string>

// ============================================================================
// [OOP CONCEPT: Abstract Base Class, Interface & Pure Virtual Functions]
// Strategy Interface for A* Heuristic calculation. Demonstrates Run-time
// Polymorphism (Dynamic Binding).
// ============================================================================
class IHeuristic {
public:
    // [OOP CONCEPT: Virtual Destructor for Clean Polymorphic Deallocation]
    virtual ~IHeuristic() {}

    // [OOP CONCEPT: Pure Virtual Functions (Abstract Interface)]
    virtual float calculate(const Vector3i& start, const Vector3i& target) const = 0;
    virtual std::string getName() const = 0;
};

#endif // IHEURISTIC_H
