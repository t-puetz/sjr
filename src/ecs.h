#ifndef ECS_H
#define ECS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_vulkan.h>

/* #################################################################### */
/* ============ START DECLARATIONS AND TYPE DEFINITONS ================ */
/* #################################################################### */
enum component_type {
    META,
    REAL,
    STATIC,
    DYNAMIC,
    PLAYER,
    NPC,
    RENDER,
    ROTATION,
    POSITION,
    SPEED,
    HEALTH,
    DAMAGE,
    COLLISION
};

typedef enum component_type comp_type;

typedef struct render_component_data {
    SDL_Texture* sdl_tex_ptr;
    SDL_Rect*    src_rect_ptr;
    SDL_Rect*    dst_rect_ptr;
    bool         visible;
    bool         has_been_rendered;
    /* Yes, a render component is allowed to tell the system 
     * whether or not we need to clear the screen and start 
     * rendering anew. */
    bool         clear_renderer;
}render_component_data;

typedef struct component {
    /**
    * A component represents the behaviours/the features/the properties
    * of an entity. Based on what type it is the appropriate system will act upon it
    * If i am "visible", the rendering system will draw me, if I am health_data
    * the appropriate system will take care of calculating my health data etc etc
    * I also store the data that the system needs to manipulate accordingly
    **/

    comp_type type;
    union data {
        render_component_data* render_component_data;
    }data;

}component;

typedef struct entity {
    /**
    *  I am an object in the game world/per scene (not an object in the OOP sense necessarily)
    *  I usually just need to be made of a GUID and an array of components
    *  We need entities to associate components with each other.
    *  Thus the component array + the ID = entity. The ID is a very fast way to identify
    *  an entity. The industry uses integers instead of pointers because here pointers
    *  would not be that much faster but way more error prone.
    **/
    u_int64_t   guid;
    component** components;
    u_int8_t    num_components;
}entity;

typedef struct scene {
    entity**    entities;
    u_int32_t   num_entities;
}scene;

typedef struct super_world {
    entity**    entities;
    u_int32_t   num_entities;
    scene*      scene;
}super_world;
/* #################################################################### */
/* ============== END DECLARATIONS AND TYPE DEFINITONS ================ */
/* #################################################################### */

/* #################################################################### */
/* ================ START FUNCTION PROTOTYPES ========================= */
/* #################################################################### */
scene*        create_scene();
super_world*  create_super_world();
void          connect_super_world_w_scene(super_world* super_world, scene* scene);
void          add_entity_to_scene(scene* scene, entity* ent);
void          add_entity_to_super_world(super_world* sw, entity* ent);

render_component_data* create_render_component_data(SDL_Texture* sdl_tex_ptr, SDL_Rect* sdl_rect_ptr_src, SDL_Rect* sdl_rect_ptr_dst);

u_int16_t     get_size_null_terminated_component_array(component** comp_array);
u_int32_t     get_size_null_terminated_entity_array(entity** entity_array);
component*    create_render_component(render_component_data* rcd);
entity*       create_entity(component* comp, u_int8_t num_components);
entity*       register_entity(comp_type component_type, void* component_data, super_world* super_world, scene* scene);
entity*       get_entity_by_guid(u_int32_t guid, super_world* ent_reg);
entity*       get_entity_from_scene(u_int32_t guid, scene* scene);
void          add_component_data_component(component* component, void* component_data);
void          add_component_to_entity(u_int32_t guid, component* comp, scene* scene);
void          free_entity_components(entity* entity);
entity**      remove_entity_from_entity_array(entity** ents, entity* ent);
component**   remove_component_from_component_array(component** comps, component* comp);
void          deregister_entity(entity* ent, scene* scn, super_world* sw);
entity**      remove_entity_from(const char* scene_or_super_world_chooser, entity* ent, void* scene_or_super_world_struct_ptr);
bool          get_entity_has_static_component(entity* ent);
bool          get_entity_has_dynamic_component(entity* ent);
void          free_ecs(super_world* super_world);
void          render_system(scene* scene, SDL_Renderer* renderer);
/* #################################################################### */
/* ================ END FUNCTION PROTOTYPES =========================== */
/* #################################################################### */

#endif
