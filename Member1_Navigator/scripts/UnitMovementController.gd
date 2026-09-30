# UnitMovementController.gd
# Smoothly lerps/slides 3D unit models along the path calculated by the Navigator.
# Role 1: The Navigator (Action Point Zero)
class_name UnitMovementController
extends Node3D

signal movement_started
signal movement_step(tile: Vector3i, remaining_ap: int)
signal movement_finished(final_tile: Vector3i, ap_spent: int)

@export var unit_name: String = "Vanguard Operative"
@export var team_id: int = 1 # 1 = Allied Player, 2 = Enemy
@export var max_ap: int = 3
var current_ap: int = 3

var grid_position: Vector3i = Vector3i.ZERO
var is_moving: bool = false

# Visual nodes
@export var model_mesh: Node3D
@export var info_label: Label3D

func _ready() -> void:
	current_ap = max_ap
	update_info_display()

func reset_turn() -> void:
	current_ap = max_ap
	update_info_display()

func update_info_display() -> void:
	if info_label:
		var team_color = "#3a86ff" if team_id == 1 else "#ff006e"
		info_label.text = "%s\n[AP: %d / %d]" % [unit_name, current_ap, max_ap]
		info_label.modulate = Color(team_color)

func snap_to_grid(coord: Vector3i) -> void:
	grid_position = coord
	var world_target = AStarNavigator.grid_to_world(coord)
	global_position = Vector3(world_target.x, 0.0, world_target.z)

# Smoothly lerp / slide model along calculated path waypoints
func move_along_path(path: Array[Vector3i], on_complete: Callable = Callable()) -> void:
	if is_moving or path.size() <= 1:
		return

	is_moving = true
	movement_started.emit()

	var tween = create_tween().set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_IN_OUT)
	var ap_spent = path.size() - 1

	# Iterate from step 1 (step 0 is the starting tile)
	for i in range(1, path.size()):
		var next_tile = path[i]
		var world_target = AStarNavigator.grid_to_world(next_tile)

		# 1. Turn towards next tile
		tween.tween_callback(func():
			var look_pos = Vector3(world_target.x, global_position.y, world_target.z)
			if global_position.distance_squared_to(look_pos) > 0.01:
				look_at(look_pos, Vector3.UP)
		)

		# 2. Smoothly slide / lerp to next tile (0.2s per tile)
		tween.tween_property(self, "global_position", world_target, 0.2)

		# 3. Deduct AP and update UI step by step
		tween.tween_callback(func():
			current_ap = maxi(0, current_ap - 1)
			grid_position = next_tile
			update_info_display()
			movement_step.emit(next_tile, current_ap)
		)

	# Final completion callback
	tween.tween_callback(func():
		is_moving = false
		movement_finished.emit(grid_position, ap_spent)
		if on_complete.is_valid():
			on_complete.call()
	)
