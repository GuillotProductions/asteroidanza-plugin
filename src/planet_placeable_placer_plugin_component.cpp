#include "planet_placeable_placer_plugin_component.hpp"
#include "godot_cpp/classes/node2d.hpp"
#include "godot_cpp/classes/random_number_generator.hpp"
#include "godot_cpp/classes/tile_data.hpp"
#include "godot_cpp/classes/tile_map_layer.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector2i.hpp"

#include <cstddef>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/print_string.hpp>

using namespace godot;

int PlanetPlaceablePlacerPluginComponent::add_planet_placeable_to_layer_randomly(TileMapLayer *tile_map_layer, Object *planet_placeable) {
  if(!tile_map_layer) {
    return 0;
  }

  if(!planet_placeable) {
    return 0;
  }

  if(!planet_placeable->is_class("PlanetPlaceable")) {
    return 0;
  }

  Node2D *node_planet_placeable = Object::cast_to<Node2D>(planet_placeable);

  if(!node_planet_placeable) {
    return 0;
  }

  Variant tmp_variant_planet_placeable_dimension_tile_map = node_planet_placeable->get("dimension_tile_map");

  if(!tmp_variant_planet_placeable_dimension_tile_map || tmp_variant_planet_placeable_dimension_tile_map.get_type() != Variant::OBJECT) {
    return 0;
  }

  Object *tmp_object_planet_placeable_dimension_tile_map = tmp_variant_planet_placeable_dimension_tile_map;

  if(!tmp_object_planet_placeable_dimension_tile_map || tmp_object_planet_placeable_dimension_tile_map->is_class("TileMapLayer")) {
    return 0;
  }

  TileMapLayer *planet_placeable_dimension_tile_map = Object::cast_to<TileMapLayer>(tmp_object_planet_placeable_dimension_tile_map);
  TypedArray<Vector2i> planet_placeable_used_cells = planet_placeable_dimension_tile_map->get_used_cells();

  if(planet_placeable_used_cells.size() == 0) {
    return 0;
  }

  Vector2 tile_map_layer_tile_size = tile_map_layer->get_tile_set()->get_tile_size();
  TypedArray<Vector2i> tile_map_layer_available_cells = tile_map_layer->get_used_cells();
  int available_cells_count = tile_map_layer_available_cells.size();

  RandomNumberGenerator rng;
  rng.randomize();

  int random_index = rng.randi_range(0, available_cells_count - 1);
  int max_iterations = Math::max(available_cells_count - random_index, random_index);
  bool successfuly_placed = false;

  for(int i = 0; i < max_iterations; i++) {
    int forwards_index = random_index + i;
    int backwards_index = random_index - 1 - i;

    TypedArray<int> indices = {};

    if(forwards_index < available_cells_count) {
      indices.append(forwards_index);
    }

    if(backwards_index >= 0) {
      indices.append(backwards_index);
    }

    for(int j = 0; j < indices.size(); j++) {
      int index = indices[j];

      Vector2i tile_map_layer_available_cell = tile_map_layer_available_cells[index];
      bool is_placeable_at_position = true;
      TypedArray<Vector2i> used_cells_local_layer_coords = {};

      for(int k = 0; k < planet_placeable_used_cells.size(); k++) {
        Vector2i used_cell = planet_placeable_used_cells[k];
        Vector2i used_cell_local_layer_coords = Vector2i(tile_map_layer_available_cell.x + 1 + used_cell.x, tile_map_layer_available_cell.y + 1 + used_cell.y);

        TileData *layer_tile_data = tile_map_layer->get_cell_tile_data(used_cell_local_layer_coords);
        if(!layer_tile_data) {
          is_placeable_at_position = false;
          break;
        }

        used_cells_local_layer_coords.append(used_cell_local_layer_coords);
      }

      if(!is_placeable_at_position) {
        continue;
      }

      for(int k = 0; k < used_cells_local_layer_coords.size(); k++) {
        tile_map_layer->set_cell(used_cells_local_layer_coords[k]);
      }

      tile_map_layer->add_sibling(node_planet_placeable);
      node_planet_placeable->set_position(tile_map_layer_tile_size * Vector2(1.0f + tile_map_layer_available_cell.x, 1.0f + tile_map_layer_available_cell.y));
      successfuly_placed = true;
      break;
    }

    if(successfuly_placed) {
      break;
    }
  }

  if(!successfuly_placed) {
    return 0;
  }

  if(planet_placeable->has_method("apply_randomness")) {
    planet_placeable->call("apply_randomness");
  }

  planet_placeable_dimension_tile_map->queue_free();

  return 1;
}

void PlanetPlaceablePlacerPluginComponent::_bind_methods() {
  ClassDB::bind_method(D_METHOD("add_planet_placeable_to_layer_randomly", "tile_map_layer", "planet_placeable"), &PlanetPlaceablePlacerPluginComponent::add_planet_placeable_to_layer_randomly);
}
