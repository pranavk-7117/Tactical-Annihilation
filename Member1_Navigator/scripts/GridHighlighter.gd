# GridHighlighter.gd
# Visual Grid Highlights for Reachable AP Range (Blue), Path Preview (Yellow), and Blocked (Red).
# Role 1: The Navigator (Action Point Zero)
class_name GridHighlighter
extends Node3D

# Materials
var mat_range_blue: StandardMaterial3D
var mat_path_yellow: StandardMaterial3D
var mat_blocked_red: StandardMaterial3D

# Visual mesh container nodes
var range_container: Node3D
var path_container: Node3D
var indicator_container: Node3D

func _ready() -> void:
	_setup_materials()
	
	range_container = Node3D.new()
	range_container.name = "RangeHighlights"
	add_child(range_container)

	path_container = Node3D.new()
	path_container.name = "PathHighlights"
	add_child(path_container)

	indicator_container = Node3D.new()
	indicator_container.name = "IndicatorHighlights"
	add_child(indicator_container)

func _setup_materials() -> void:
	# Blue overlay for reachable AP movement range
	mat_range_blue = StandardMaterial3D.new()
	mat_range_blue.albedo_color = Color(0.2, 0.5, 1.0, 0.4)
	mat_range_blue.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mat_range_blue.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat_range_blue.cull_mode = BaseMaterial3D.CULL_DISABLED

	# Yellow/Cyan trail for path waypoints preview
	mat_path_yellow = StandardMaterial3D.new()
	mat_path_yellow.albedo_color = Color(1.0, 0.85, 0.15, 0.75)
	mat_path_yellow.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mat_path_yellow.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat_path_yellow.cull_mode = BaseMaterial3D.CULL_DISABLED

	# Red indicator for blocked or invalid tiles
	mat_blocked_red = StandardMaterial3D.new()
	mat_blocked_red.albedo_color = Color(0.95, 0.2, 0.2, 0.6)
	mat_blocked_red.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mat_blocked_red.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat_blocked_red.cull_mode = BaseMaterial3D.CULL_DISABLED

func _create_tile_quad(pos: Vector3i, material: Material, scale_factor: float = 0.92, y_offset: float = 0.02) -> MeshInstance3D:
	var quad = QuadMesh.new()
	var size = AStarNavigator.TILE_SIZE * scale_factor
	quad.size = Vector2(size, size)

	var mesh_inst = MeshInstance3D.new()
	mesh_inst.mesh = quad
	mesh_inst.material_override = material
	
	# Quad default faces Z, rotate to lie flat on X-Z plane
	mesh_inst.rotation_degrees = Vector3(-90, 0, 0)
	
	var world_center = AStarNavigator.grid_to_world(pos)
	mesh_inst.position = Vector3(world_center.x, y_offset, world_center.z)
	return mesh_inst

# Show all tiles reachable within operative AP budget in translucent blue
func show_reachable_range(tiles: Array[Vector3i]) -> void:
	clear_range()
	for tile in tiles:
		var quad = _create_tile_quad(tile, mat_range_blue, 0.92, 0.02)
		range_container.add_child(quad)

# Show active path preview in yellow/amber
func show_path_preview(path: Array[Vector3i]) -> void:
	clear_path()
	if path.is_empty():
		return
	for i in range(path.size()):
		var tile = path[i]
		# Start tile is where unit already is, so draw waypoints slightly smaller
		var quad = _create_tile_quad(tile, mat_path_yellow, 0.70, 0.04)
		path_container.add_child(quad)

# Show blocked cursor indicator in red
func show_blocked_indicator(pos: Vector3i) -> void:
	clear_indicator()
	var quad = _create_tile_quad(pos, mat_blocked_red, 0.95, 0.05)
	indicator_container.add_child(quad)

func clear_range() -> void:
	for child in range_container.get_children():
		child.queue_free()

func clear_path() -> void:
	for child in path_container.get_children():
		child.queue_free()

func clear_indicator() -> void:
	for child in indicator_container.get_children():
		child.queue_free()

func clear_all() -> void:
	clear_range()
	clear_path()
	clear_indicator()
