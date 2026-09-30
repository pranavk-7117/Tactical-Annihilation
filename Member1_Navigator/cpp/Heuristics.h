#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "IHeuristic.h"
#include <cmath>

// ============================================================================
// [OOP CONCEPT: Hierarchical Inheritance & Dynamic Binding]
// Concrete Heuristic Strategies implementing IHeuristic.
// ============================================================================

// Standard Manhattan Heuristic (L1 Norm): Ideal for 4-directional grid movement
class ManhattanHeuristic : public IHeuristic {
public:
    virtual float calculate(const Vector3i& start, const Vector3i& target) const override {
        return static_cast<float>(start.manhattanDistance(target));
    }

    virtual std::string getName() const override {
        return "Manhattan (L1 Norm - 4-Directional Grid)";
    }
};

// Euclidean Heuristic (L2 Norm): Straight-line geometric distance
class EuclideanHeuristic : public IHeuristic {
public:
    virtual float calculate(const Vector3i& start, const Vector3i& target) const override {
        return start.euclideanDistance(target);
    }

    virtual std::string getName() const override {
        return "Euclidean (L2 Norm - Straight Line)";
    }
};

// Chebyshev Heuristic (L-infinity Norm): Diagonal/King's movement distance
class ChebyshevHeuristic : public IHeuristic {
public:
    virtual float calculate(const Vector3i& start, const Vector3i& target) const override {
        return static_cast<float>(start.chebyshevDistance(target));
    }

    virtual std::string getName() const override {
        return "Chebyshev (L-infinity Norm - King Movement)";
    }
};

#endif // HEURISTICS_H
