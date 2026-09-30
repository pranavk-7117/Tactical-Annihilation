# AStarNavigator.gd
# GDScript Mirror of the C++ AStarPathfinder & GridMap3D Engine
# Role 1: The Navigator (Action Point Zero)
class_name AStarNavigator
extends RefCounted

const GRID_WIDTH: int = 10
const GRID_DEPTH: int = 10
const TILE_SIZE: float = 2.0 # 2.0m x 2.0m per tile

# Grid data structures
# tile_walkable[x][z] -> bool
# tile_occupant[x][z] -> { "unit_id": int, "team_id": int }
var _walkable: Array = []
var _occupants: Dictionary = {}

func _init() -> void:
	reset_grid()

func reset_grid() -> void:
	_walkable.clear()
	_occupants.clear()
	for x in range(GRID_WIDTH):
		var column: Array = []
		for z in range(GRID_DEPTH):
			column.append(true)
		_walkable.append(column)

func is_in_bounds(pos: Vector3i) -> bool:
	return pos.x >= 0 and pos.x < GRID_WIDTH and pos.z >= 0 and pos.z < GRID_DEPTH

func set_obstacle(pos: Vector3i, blocked: bool) -> void:
	if is_in_bounds(pos):
		_walkable[pos.x][pos.z] = !blocked

func is_walkable(pos: Vector3i) -> bool:
	if not is_in_bounds(pos):
		return false
	return _walkable[pos.x][pos.z]

func occupy_tile(pos: Vector3i, unit_id: int, team_id: int) -> void:
	if is_in_bounds(pos):
		_occupants[Vector2i(pos.x, pos.z)] = { "unit_id": unit_id, "team_id": team_id }

func vacate_tile(pos: Vector3i) -> void:
	_occupants.erase(Vector2i(pos.x, pos.z))

func is_occupied(pos: Vector3i) -> bool:
	return _occupants.has(Vector2i(pos.x, pos.z))

func get_occupant_team(pos: Vector3i) -> int:
	var key = Vector2i(pos.x, pos.z)
	if _occupants.has(key):
		return _occupants[key]["team_id"]
	return 0

# Traversal rule: open or friendly units can be stepped through,
# enemies and static obstacles block completely.
func can_traverse(pos: Vector3i, moving_team_id: int) -> bool:
	if not is_walkable(pos):
		return false
	if not is_occupied(pos):
		return true
	return get_occupant_team(pos) == moving_team_id

# Stop rule: can only stop on walkable, unoccupied tiles
func can_stop(pos: Vector3i) -> bool:
	return is_walkable(pos) and not is_occupied(pos)

# 4-directional orthogonal neighbors (North, South, East, West)
func get_neighbors(pos: Vector3i) -> Array[Vector3i]:
	var result: Array[Vector3i] = []
	var dirs: Array[Vector3i] = [
		Vector3i(0, 0, -1), # North
		Vector3i(0, 0, 1),  # South
		Vector3i(1, 0, 0),  # East
		Vector3i(-1, 0, 0)  # West
	]
	for d in dirs:
		var n = pos + d
		if is_in_bounds(n):
			result.append(n)
	return result

func manhattan_distance(a: Vector3i, b: Vector3i) -> int:
	return abs(a.x - b.x) + abs(a.z - b.z)

# Calculate 1-AP-per-tile movement cost
func calculate_ap_cost(path: Array[Vector3i]) -> int:
	if path.is_empty():
		return 0
	return maxi(0, path.size() - 1)

# A* Pathfinding Algorithm
func find_path(start: Vector3i, goal: Vector3i, moving_team_id: int) -> Array[Vector3i]:
	if not is_in_bounds(start) or not is_in_bounds(goal):
		return []
	if not can_stop(goal):
		return []
	if start == goal:
		return [start]

	# Priority Queue simulated with an Array sorted by fCost
	var open_set: Array[Dictionary] = []
	var g_score: Dictionary = {}
	var came_from: Dictionary = {}
	var closed_set: Dictionary = {}

	var start_key = Vector2i(start.x, start.z)
	var goal_key = Vector2i(goal.x, goal.z)

	g_score[start_key] = 0
	open_set.append({
		"pos": start,
		"f": float(manhattan_distance(start, goal)),
		"g": 0.0
	})

	var found_goal: bool = false

	while not open_set.is_empty():
		# Sort ascending by f score
		open_set.sort_custom(func(a, b): return a["f"] < b["f"])
		var current_node = open_set.pop_front()
		var current_pos: Vector3i = current_node["pos"]
		var current_key = Vector2i(current_pos.x, current_pos.z)

		if current_key == goal_key:
			found_goal = true
			break

		closed_set[current_key] = true

		for neighbor in get_neighbors(current_pos):
			var n_key = Vector2i(neighbor.x, neighbor.z)
			if closed_set.has(n_key):
				continue

			# Must be traversable (open or friendly unit)
			if not can_traverse(neighbor, moving_team_id):
				continue

			var tentative_g = current_node["g"] + 1.0 # 1 AP cost per step

			var prev_g = g_score.get(n_key, 999999.0)
			if tentative_g < prev_g:
				came_from[n_key] = current_pos
				g_score[n_key] = tentative_g
				var f = tentative_g + float(manhattan_distance(neighbor, goal))
				open_set.append({
					"pos": neighbor,
					"f": f,
					"g": tentative_g
				})

	if not found_goal:
		return []

	# Reconstruct path from goal back to start
	var path: Array[Vector3i] = []
	var curr = goal
	while curr != start:
		path.append(curr)
		var k = Vector2i(curr.x, curr.z)
		curr = came_from[k]
	path.append(start)
	path.reverse()
	return path

# Reachable tiles within current AP budget (Dijkstra / BFS flood-fill)
func get_reachable_tiles(start: Vector3i, current_ap: int, moving_team_id: int) -> Array[Vector3i]:
	var reachable: Array[Vector3i] = []
	if current_ap <= 0 or not is_in_bounds(start):
		return reachable

	var frontier: Array[Dictionary] = [{ "pos": start, "cost": 0 }]
	var cost_so_far: Dictionary = { Vector2i(start.x, start.z): 0 }

	while not frontier.is_empty():
		var current = frontier.pop_front()
		var curr_pos: Vector3i = current["pos"]
		var curr_cost: int = current["cost"]

		if curr_cost > 0 and can_stop(curr_pos):
			reachable.append(curr_pos)

		if curr_cost >= current_ap:
			continue

		for neighbor in get_neighbors(curr_pos):
			if not can_traverse(neighbor, moving_team_id):
				continue

			var new_cost = curr_cost + 1
			var n_key = Vector2i(neighbor.x, neighbor.z)

			if not cost_so_far.has(n_key) or new_cost < cost_so_far[n_key]:
				cost_so_far[n_key] = new_cost
				frontier.append({ "pos": neighbor, "cost": new_cost })

	return reachable

# Convert grid coordinate to 3D world coordinate (centered on tile)
static func grid_to_world(pos: Vector3i) -> Vector3:
	return Vector3(
		pos.x * TILE_SIZE + (TILE_SIZE * 0.5),
		0.0,
		pos.z * TILE_SIZE + (TILE_SIZE * 0.5)
	)

# Convert 3D world coordinate to grid coordinate
static func world_to_grid(world_pos: Vector3) -> Vector3i:
	return Vector3i(
		int(floor(world_pos.x / TILE_SIZE)),
		0,
		int(floor(world_pos.z / TILE_SIZE))
	)
