# MouseRaycaster.gd
# Translates 2D screen mouse positions into 3D Grid coordinates on the tactical board.
# Role 1: The Navigator (Action Point Zero)
class_name MouseRaycaster
extends Node

signal grid_tile_hovered(grid_pos: Vector3i)
signal grid_tile_clicked(grid_pos: Vector3i)

@export var camera: Camera3D
var last_hovered_tile: Vector3i = Vector3i(-1, -1, -1)
var ground_plane: Plane = Plane(Vector3.UP, 0.0)

func _ready() -> void:
	if not camera:
		camera = get_viewport().get_camera_3d()

func _input(event: InputEvent) -> void:
	if not camera:
		return

	if event is InputEventMouseMotion:
		var tile = get_grid_pos_at_mouse(event.position)
		if tile != last_hovered_tile:
			last_hovered_tile = tile
			grid_tile_hovered.emit(tile)

	elif event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
		var tile = get_grid_pos_at_mouse(event.position)
		grid_tile_clicked.emit(tile)

func get_grid_pos_at_mouse(mouse_screen_pos: Vector2) -> Vector3i:
	if not camera:
		return Vector3i(-1, -1, -1)

	var ray_origin = camera.project_ray_origin(mouse_screen_pos)
	var ray_normal = camera.project_ray_normal(mouse_screen_pos)

	var hit = ground_plane.intersects_ray(ray_origin, ray_normal)
	if hit != null:
		return AStarNavigator.world_to_grid(hit)

	return Vector3i(-1, -1, -1)
