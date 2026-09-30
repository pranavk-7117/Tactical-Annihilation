# NavigatorController.gd
# Main scene controller for Member 1: The Navigator
# Coordinates Board, A* Pathfinding, Raycasting, Highlights, and Smooth Movement.
# Role 1: The Navigator (Action Point Zero)
class_name NavigatorController
extends Node3D

@export var raycaster: MouseRaycaster
@export var highlighter: GridHighlighter
@export var active_unit: UnitMovementController

# Optional other units for demonstration
@export var ally_unit: UnitMovementController
@export var enemy_unit: UnitMovementController

# Visual Floor & Obstacle root
@export var grid_map: GridMap
@export var board_root: Node3D

# UI References
@export var lbl_unit_info: Label
@export var lbl_hover_info: Label
@export var lbl_action_log: Label
@export var btn_reset_turn: Button

var navigator: AStarNavigator
var current_reachable_tiles: Array[Vector3i] = []
var active_path_preview: Array[Vector3i] = []

func _ready() -> void:
	navigator = AStarNavigator.new()
	_build_visual_board()
	_setup_units()
	_setup_signals()
	_refresh_reachable_range()
	_update_ui()

func _setup_signals() -> void:
	if raycaster:
		raycaster.grid_tile_hovered.connect(_on_tile_hovered)
		raycaster.grid_tile_clicked.connect(_on_tile_clicked)

	if btn_reset_turn:
		btn_reset_turn.pressed.connect(_on_reset_turn_pressed)

	if active_unit:
		active_unit.movement_finished.connect(_on_unit_movement_finished)

func _build_visual_board() -> void:
	if not board_root:
		board_root = Node3D.new()
		board_root.name = "BoardRoot"
		add_child(board_root)

	# Setup floor material (Dark tactical grid)
	var floor_mat = StandardMaterial3D.new()
	floor_mat.albedo_color = Color(0.12, 0.15, 0.2)
	floor_mat.roughness = 0.8

	# Setup obstacle material (Orange/Rust barricade)
	var obstacle_mat = StandardMaterial3D.new()
	obstacle_mat.albedo_color = Color(0.75, 0.35, 0.15)
	obstacle_mat.metallic = 0.5
	obstacle_mat.roughness = 0.4

	# Define static obstacles (matching default_arena.map)
	var obstacle_positions: Array[Vector3i] = [
		Vector3i(3, 0, 1), Vector3i(4, 0, 1),
		Vector3i(3, 0, 2), Vector3i(4, 0, 2),
		Vector3i(1, 0, 4), Vector3i(2, 0, 4), Vector3i(3, 0, 4), Vector3i(4, 0, 4),
		Vector3i(4, 0, 5), Vector3i(4, 0, 6), Vector3i(4, 0, 7)
	]

	for obs in obstacle_positions:
		navigator.set_obstacle(obs, true)

	# If a GridMap node is present, populate it with FloorTile (0) and ObstacleWall (1)
	if grid_map:
		grid_map.clear()
		for x in range(AStarNavigator.GRID_WIDTH):
			for z in range(AStarNavigator.GRID_DEPTH):
				var tile_pos = Vector3i(x, 0, z)
				if navigator.is_walkable(tile_pos):
					grid_map.set_cell_item(tile_pos, 0) # FloorTile
				else:
					grid_map.set_cell_item(tile_pos, 1) # ObstacleWall

	# Generate 10x10 visual tiles
	for x in range(AStarNavigator.GRID_WIDTH):
		for z in range(AStarNavigator.GRID_DEPTH):
			var tile_pos = Vector3i(x, 0, z)
			var world_pos = AStarNavigator.grid_to_world(tile_pos)

			var floor_mesh = BoxMesh.new()
			floor_mesh.size = Vector3(AStarNavigator.TILE_SIZE * 0.96, 0.1, AStarNavigator.TILE_SIZE * 0.96)
			var floor_inst = MeshInstance3D.new()
			floor_inst.mesh = floor_mesh
			floor_inst.material_override = floor_mat
			floor_inst.position = Vector3(world_pos.x, -0.05, world_pos.z)
			board_root.add_child(floor_inst)

			# If obstacle, place a 3D barrier cube on top
			if not navigator.is_walkable(tile_pos):
				var obs_mesh = BoxMesh.new()
				obs_mesh.size = Vector3(AStarNavigator.TILE_SIZE * 0.88, 1.2, AStarNavigator.TILE_SIZE * 0.88)
				var obs_inst = MeshInstance3D.new()
				obs_inst.mesh = obs_mesh
				obs_inst.material_override = obstacle_mat
				obs_inst.position = Vector3(world_pos.x, 0.6, world_pos.z)
				board_root.add_child(obs_inst)

func _setup_units() -> void:
	# Active player unit: Team 1
	if active_unit:
		var start_pos = Vector3i(2, 0, 5)
		active_unit.snap_to_grid(start_pos)
		navigator.occupy_tile(start_pos, 1, 1)

	# Allied friendly unit: Team 1 (Placed at 3,0,5 to prove friendly pass-through)
	if ally_unit:
		var ally_pos = Vector3i(3, 0, 5)
		ally_unit.snap_to_grid(ally_pos)
		navigator.occupy_tile(ally_pos, 2, 1)

	# Enemy unit: Team 2 (Placed at 4,0,3 to prove enemy blocking)
	if enemy_unit:
		var enemy_pos = Vector3i(4, 0, 3)
		enemy_unit.snap_to_grid(enemy_pos)
		navigator.occupy_tile(enemy_pos, 3, 2)

func _refresh_reachable_range() -> void:
	if not active_unit or not highlighter:
		return

	highlighter.clear_all()
	current_reachable_tiles = navigator.get_reachable_tiles(
		active_unit.grid_position,
		active_unit.current_ap,
		active_unit.team_id
	)
	highlighter.show_reachable_range(current_reachable_tiles)

func _on_tile_hovered(tile: Vector3i) -> void:
	if not active_unit or active_unit.is_moving:
		return

	if not navigator.is_in_bounds(tile):
		highlighter.clear_path()
		highlighter.clear_indicator()
		_set_hover_text("Cursor outside board boundaries")
		return

	# Case 1: Hovering over valid reachable tile
	if current_reachable_tiles.has(tile):
		highlighter.clear_indicator()
		active_path_preview = navigator.find_path(active_unit.grid_position, tile, active_unit.team_id)
		highlighter.show_path_preview(active_path_preview)
		var ap_cost = navigator.calculate_ap_cost(active_path_preview)
		_set_hover_text("Tile (%d, %d) | Path Cost: %d AP (Click to Move)" % [tile.x, tile.z, ap_cost])

	# Case 2: Hovering over blocked tile, occupied tile, or out of AP range
	else:
		highlighter.clear_path()
		highlighter.show_blocked_indicator(tile)
		if not navigator.is_walkable(tile):
			_set_hover_text("Tile (%d, %d) [BLOCKED] Solid Obstacle Wall" % [tile.x, tile.z])
		elif navigator.is_occupied(tile):
			var is_ally = navigator.get_occupant_team(tile) == active_unit.team_id
			var occupant = "Friendly Ally (Pass-through allowed, cannot end turn here)" if is_ally else "Enemy Unit (Impassable Barrier)"
			_set_hover_text("Tile (%d, %d) [OCCUPIED] %s" % [tile.x, tile.z, occupant])
		else:
			_set_hover_text("Tile (%d, %d) [OUT OF RANGE] Requires more than %d AP" % [tile.x, tile.z, active_unit.current_ap])

func _on_tile_clicked(tile: Vector3i) -> void:
	if not active_unit or active_unit.is_moving:
		return

	if not current_reachable_tiles.has(tile):
		_log_event("Cannot move to (%d, %d): Tile is blocked or out of AP budget." % [tile.x, tile.z])
		return

	var path = navigator.find_path(active_unit.grid_position, tile, active_unit.team_id)
	if path.is_empty():
		_log_event("Pathfinding failed: No route found to (%d, %d)." % [tile.x, tile.z])
		return

	var ap_cost = navigator.calculate_ap_cost(path)
	_log_event("Executing move to (%d, %d)... Cost: %d AP" % [tile.x, tile.z, ap_cost])

	# Vacate old tile
	navigator.vacate_tile(active_unit.grid_position)
	highlighter.clear_all()

	# Start smooth lerping along path
	active_unit.move_along_path(path)

func _on_unit_movement_finished(final_tile: Vector3i, ap_spent: int) -> void:
	# Occupy new tile in navigation state
	navigator.occupy_tile(final_tile, 1, active_unit.team_id)
	_log_event("Operative arrived at (%d, %d). AP remaining: %d" % [final_tile.x, final_tile.z, active_unit.current_ap])
	_refresh_reachable_range()
	_update_ui()

func _on_reset_turn_pressed() -> void:
	if active_unit and not active_unit.is_moving:
		active_unit.reset_turn()
		_log_event("Turn reset: Action Points restored to %d AP." % active_unit.max_ap)
		_refresh_reachable_range()
		_update_ui()

func _update_ui() -> void:
	if lbl_unit_info and active_unit:
		lbl_unit_info.text = "Active: %s | AP: %d / %d" % [active_unit.unit_name, active_unit.current_ap, active_unit.max_ap]

func _set_hover_text(text: String) -> void:
	if lbl_hover_info:
		lbl_hover_info.text = text

func _log_event(msg: String) -> void:
	print("[Navigator] " + msg)
	if lbl_action_log:
		lbl_action_log.text = msg
