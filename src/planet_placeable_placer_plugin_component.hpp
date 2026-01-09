#pragma once

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/classes/tile_map_layer.hpp"
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/classes/node.hpp>

class PlanetPlaceablePlacerPluginComponent: public godot::Node {
  GDCLASS(PlanetPlaceablePlacerPluginComponent, godot::Node)

  public:
    int add_planet_placeable_to_layer_randomly(godot::TileMapLayer *tile_map_layer, godot::Object *planet_placeable);

  protected:
    static void _bind_methods();

  private:
    void say_hello();
};
