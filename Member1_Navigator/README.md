# Action Point Zero - Task 1: The Navigator

**Assigned Role:** Member 1: The Navigator (Grid, Pathfinding & Movement)  
**Academic Alignment:** Computer Engineering Object-Oriented Programming (OOP) Syllabus (Units 1 to 6)  
**Game Engine:** Godot 4.7 (3D) + Standalone C++ Engine

---

## 📌 Mapping Code to OOPs Syllabus Topics

| Syllabus Unit | Key Concept | Implementation in Code |
| :--- | :--- | :--- |
| **Unit 1: Fundamentals of OOP** | Objects, Classes, Encapsulation, Abstraction | Found across [`Vector3i`](cpp/Vector3i.h), [`Tile`](cpp/Tile.h), [`GridMap3D`](cpp/GridMap3D.h), and [`AStarPathfinder`](cpp/AStarPathfinder.h). Internal grid storage and costs are hidden behind clean accessors. |
| **Unit 2: Classes & Objects** | Access Modifiers (`private`, `protected`, `public`) | `Vector3i.h`, `Tile.h`, `GridMap3D.h`, `NavigationExceptions.h`. |
| | The `this` pointer | Explicitly used in `GridMap3D.cpp` (`this->width = ...`, `this->allocateGrid()`) and `Vector3i.h` (`this->x = other.x`). |
| | Static Data Members & Static Methods | `AStarPathfinder::totalPathsCalculated` and `AStarPathfinder::getTotalPathsCalculated()`. |
| | Compile-Time Polymorphism (Method & Operator Overloading) | `Vector3i` overloads `operator+`, `operator-`, `operator==`, `operator!=`, `operator<`, and friend `operator<<`. |
| | Dynamic Memory Allocation (`new` / `delete`) | `GridMap3D::allocateGrid()` and `deallocateGrid()` manage dynamic heap memory (`Tile** grid = new Tile*[width]`). Dynamically allocated `PathNode` objects in `AStarPathfinder.cpp`. |
| | Friend Functions | `friend std::ostream& operator<<(std::ostream&, const Vector3i&)` in `Vector3i.h`, `friend operator<<` in `Tile.h` and `GridMap3D.h`. |
| **Unit 3: Constructors & Destructors** | Types of Constructors | Default Constructor, Parameterized Constructor with default arguments, and Copy Constructor in `Vector3i.h`, `Tile.h`, and `GridMap3D.h`. |
| | Deep Copy Constructor | `GridMap3D::GridMap3D(const GridMap3D& other)` allocates an independent dynamic 2D array on the heap, verified in `main.cpp` DEMO 3. |
| | Virtual Destructor | `virtual ~GridMap3D()` and `virtual ~IHeuristic()` ensure proper cleanup when deallocated through base pointers. |
| **Unit 4: Inheritance & Polymorphism** | Abstract Classes & Interfaces | [`IHeuristic.h`](cpp/IHeuristic.h) defines pure virtual functions: `virtual float calculate(...) = 0;` and `virtual std::string getName() const = 0;`. |
| | Hierarchical Inheritance | [`Heuristics.h`](cpp/Heuristics.h): `ManhattanHeuristic`, `EuclideanHeuristic`, and `ChebyshevHeuristic` derive from `IHeuristic`. |
| | Run-time Polymorphism (Dynamic Binding) | `AStarPathfinder` stores an `IHeuristic*` base pointer and dynamically dispatches heuristic calculations at runtime (`heuristic->calculate(start, goal)`). |
| **Unit 5: File Handling** | `std::ofstream`, `std::ifstream`, File Modes | [`MapSerializer.h`](cpp/MapSerializer.h) & [`MapSerializer.cpp`](cpp/MapSerializer.cpp): reads `.map` grid files with `std::ifstream`, writes with `std::ofstream`, and appends audit logs with `std::ios::app`. |
| **Unit 6: Exception Handling & Generics** | Custom Exception Hierarchy & Multiple Catch Blocks | [`NavigationExceptions.h`](cpp/NavigationExceptions.h): `OutOfBoundsException`, `TileBlockedException`, `PathNotFoundException`, and `InsufficientAPException` derived from `NavigationException`. Handled with multiple `catch` clauses in `main.cpp`. |
| | Generic Programming (Templates) | [`PriorityQueue.h`](cpp/PriorityQueue.h): `template <typename T, typename Comparator> class MinPriorityQueue` implementing a generic binary min-heap. |

---

## 🧭 Game Mechanics Implemented (GDD Task 1)

1. **3D Grid Coordinate System (`Vector3i`)**:
   - Represents discrete integer battlefield coordinates `(x, y, z)` on a 10×10 board ($y=0$ flat tactical plane).
   - Chebyshev, Manhattan, and Euclidean distance calculation.
   - 4-directional orthogonal neighbor expansion (North, South, East, West).

2. **A\* Pathfinding with Obstacle Avoidance & Team Occupancy Rules**:
   - Optimal path search avoiding static terrain walls/obstacles.
   - **Friendly Unit Pass-Through**: Units can walk through friendly tiles, but **cannot stop** on them.
   - **Enemy Unit Impassable Barrier**: Enemy operative tiles completely block path traversal.
   - Automatic detour path recalculation around occupied zones.

3. **Turn Economy (1 AP Per Tile)**:
   - Every tile traversed costs exactly **1 Action Point (AP)**.
   - Path AP Cost $= \text{path.size}() - 1$.
   - **Reachable Zone Flood-Fill**: Dijkstra/BFS algorithm calculates all tiles reachable within an operative's remaining AP (e.g. 3 AP budget).
   - Throws `InsufficientAPException` if a player attempts to execute a path exceeding their current AP.

4. **Godot 4 3D Visual Layer**:
   - **GridMap & Tile Palettes**: 10×10 tactical board with 2.0m × 2.0m tile spacing and 3D obstacle barricades.
   - **Mouse Raycasting (`MouseRaycaster.gd`)**: Projects 2D camera viewport clicks into 3D world ray intersections with the ground plane ($Y=0$) and maps them to grid coordinates.
   - **Visual Grid Highlights (`GridHighlighter.gd`)**:
     - 🟦 **Blue overlay**: Walkable tiles within operative's current AP.
     - 🟨 **Yellow trail**: Real-time calculated A* path preview from unit to cursor.
     - 🟥 **Red indicator**: Blocked obstacle, impassable enemy, or out-of-range tile.
   - **Visual Movement Lerping (`UnitMovementController.gd`)**:
     - Smoothly slides unit 3D models tile-by-tile using Godot `Tween`.
     - Smoothly rotates the model to face each waypoint before sliding.
     - Real-time step-by-step AP deduction and arrival signal emission.

---

## 🚀 How to Compile and Run the C++ CLI Testbed

Open a terminal in the `Member1_Navigator/cpp/` directory:

### Option A: Using `make`
```bash
cd Member1_Navigator/cpp
make run
```

### Option B: Direct `clang++` or `g++` compilation
```bash
cd Member1_Navigator/cpp
clang++ -std=c++17 -Wall -Wextra -O2 main.cpp GridMap3D.cpp AStarPathfinder.cpp MapSerializer.cpp -o navigator_engine
./navigator_engine
```

---

## 🎮 How to Run the Godot 4 Project

1. Open **Godot Engine 4.x** (e.g., Godot 4.7).
2. Click **Import** and select the [`project.godot`](project.godot) file located inside `Member1_Navigator/`.
3. Press **F5** (or click the **Play** button in the top right) to launch [`MainNavigatorScene.tscn`](scenes/MainNavigatorScene.tscn).
4. **Controls**:
   - **Hover Mouse**: Highlights hovered tile, showing path preview (yellow) or blocked indicator (red).
   - **Left Click**: Moves the player operative along the optimal path with smooth lerp movement.
   - **Reset Turn Button**: Restores Action Points back to 3 AP.

---

## 📂 Directory Structure

```
Member1_Navigator/
├── project.godot                     # Godot 4 project configuration
├── icon.svg                          # Project icon
├── README.md                         # Documentation & academic grading mapping
├── cpp/                              # Pure C++ Navigation Engine
│   ├── Vector3i.h                    # Coordinate class & operator overloads
│   ├── Tile.h                        # Cell properties, traversability & occupancy
│   ├── GridMap3D.h                   # 10x10 Board representation & deep copy
│   ├── GridMap3D.cpp
│   ├── IHeuristic.h                  # Abstract base class for heuristics
│   ├── Heuristics.h                  # Manhattan, Euclidean, Chebyshev strategies
│   ├── PriorityQueue.h               # Generic Min-Heap template
│   ├── NavigationExceptions.h        # Custom exception hierarchy
│   ├── AStarPathfinder.h             # A* algorithm & 1-AP-per-tile movement
│   ├── AStarPathfinder.cpp
│   ├── MapSerializer.h               # File I/O (ifstream, ofstream, ios::app)
│   ├── MapSerializer.cpp
│   ├── main.cpp                      # Complete CLI testbed with 10 demonstrations
│   ├── Makefile                      # Build automation
│   └── maps/
│       └── default_arena.map         # 10x10 ASCII level layout
├── scripts/                          # Godot 4 GDScript Integration
│   ├── AStarNavigator.gd             # GDScript mirror of the C++ engine
│   ├── MouseRaycaster.gd             # 2D Screen-to-3D Grid raycasting
│   ├── GridHighlighter.gd            # Blue AP range, yellow path & red indicators
│   ├── UnitMovementController.gd     # Smooth Tween lerp & orientation rotation
│   └── NavigatorController.gd        # Master coordinator & HUD
└── scenes/
    ├── Unit3D.tscn                   # Reusable 3D Unit scene with 3D label
    └── MainNavigatorScene.tscn       # Playable 3D tactical scene with lighting & UI
```
