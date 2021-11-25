#include "ecs.h"
#include "c_hlprfncts.h"

/* #################################################################### */
/* ================ START FUNCTION SIGNATURES ========================= */
/* #################################################################### */
u_int16_t get_size_null_terminated_component_array(component** comp_array)
{
    u_int16_t size = 0;
    u_int16_t i    = 0;

    while(comp_array[i]) {
        printf("Iterating component array until NULL to get size: I am at component %p, address of array is: %p\n", &comp_array[i], comp_array);
        size++;
        i++;
    }

    return size;
}

u_int32_t get_size_null_terminated_entity_array(entity** entity_array)
{
    u_int32_t size = 0;
    u_int32_t i    = 0;

    while(entity_array[i]) {
        printf("Iterating entity array until NULL to get size: I am at entity %p, address of array is: %p\n", &entity_array[i], entity_array);
        size++;
        i++;
    }

    return size;
}

super_world* create_super_world()
{
	printf("Creating super world...\n");
	
    super_world* sw = malloc(1 * sizeof(super_world));
    printf("Allocing world entity struct... %p\n", sw);
    exit_w_one_on_failed_xalloc(sw);

    sw->entities     = malloc(2 * sizeof(entity*));
    printf("Allocing initial space (2 x entity) for one entity to world->entities + NULL terminator... %p\n", sw->entities);
    exit_w_one_on_failed_xalloc(sw->entities);
    sw->entities[1] = NULL;
    sw->num_entities = 0;
    
    printf("Done creating super world!\n");

    return sw;
}

scene* create_scene()
{
	printf("Creating scene...\n");
	
    scene* scn = malloc(1 * sizeof(scn));
    printf("Allocing scene entity struct... %p\n", scn);
    exit_w_one_on_failed_xalloc(scn);

    scn->entities     = malloc(2 * sizeof(entity*));
    printf("Allocing initial space (2 x entity) for one entity to scene->entities + NULL terminator... %p\n", scn->entities);
    exit_w_one_on_failed_xalloc(scn->entities);
    scn->entities[1] = NULL;
    scn->num_entities = 0;
    
    printf("Done creating scene!\n");

    return scn;
}

void connect_super_world_w_scene(super_world* sw, scene* scn)
{
    sw->scene = scn;
}

void add_entity_to_scene(scene* scn, entity* ent)
{
    if(!ent) {
        printf("Provided null pointer for entity!\n Exiting with 1.");
        exit(1);
    }

    if(!scn) {
        printf("Provided null pointer for scene!\n Exiting with 1.");
        exit(1);
    }

u_int32_t num_cur_ents_in_scene = scn->num_entities;
    scn->entities = realloc(scn->entities, (num_cur_ents_in_scene + 1));
    exit_w_one_on_failed_xalloc(scn->entities);
    scn->entities[num_cur_ents_in_scene] = ent;
    scn->entities[num_cur_ents_in_scene + 1] = NULL;
    scn->num_entities++;
}

void add_entity_to_super_world(super_world* sw, entity* ent)
{
    if(!ent) {
        printf("Provided null pointer for entity!\n Exiting with 1.");
        exit(1);
    }

    if(!sw) {
        printf("Provided null pointer for super_world!\n Exiting with 1.");
        exit(1);
    }

    u_int32_t num_cur_ents_in_sw = sw->num_entities;
    sw->entities = realloc(sw->entities, (num_cur_ents_in_sw + 1));
    exit_w_one_on_failed_xalloc(sw->entities);
    sw->entities[num_cur_ents_in_sw] = ent;
    sw->entities[num_cur_ents_in_sw + 1] = NULL;
    sw->num_entities++;
}

entity* register_entity(comp_type component_type, void* component_data, super_world* super_world, scene* scene)
{
    component* component = NULL;

    switch(component_type) {
        case RENDER:
            component = create_render_component((render_component_data*)component_data);
            break;
        default:
            printf("Unknown component. Exiting with 1!\n");
            exit(1);
    }

    if(!component) {
        printf("Did not create component properly. Assigning to entity impossible! Exiting with 1!\n");
        exit(1);
    }

    entity* ent = create_entity(component, 1);

    add_entity_to_super_world(super_world, ent);
    add_entity_to_scene(scene, ent);

    return ent;
}

render_component_data* create_render_component_data(SDL_Texture* sdl_tex_ptr, SDL_Rect* sdl_rect_ptr_src, SDL_Rect* sdl_rect_ptr_dst)
{
    render_component_data* rcd = malloc(1 * sizeof(render_component_data));
    printf("Allocing render component data... %p\n", rcd);
    exit_w_one_on_failed_xalloc(rcd);

    rcd->sdl_tex_ptr  = sdl_tex_ptr; //TODO: Account for more shapes than just rect
    rcd->src_rect_ptr = sdl_rect_ptr_src; 
    rcd->dst_rect_ptr = sdl_rect_ptr_dst; 
    rcd->visible      = false;
    rcd->clear_renderer = false;

    return rcd;
}

component* create_render_component(render_component_data* rcd)
{
    component* render_component = malloc(1 * sizeof(component));
    printf("Allocing component... %p \n", render_component);
    exit_w_one_on_failed_xalloc(render_component);

    render_component->type = RENDER;

    printf("Associating data with RENDER component...\n");
    add_component_data_component(render_component, rcd);

    return render_component;
}

entity* create_entity(component* comp, u_int8_t num_components) {
    entity* ent = malloc(1 * sizeof(entity));
    printf("Allocing entity... %p\n", ent);
    exit_w_one_on_failed_xalloc(ent);

    static u_int64_t guid = 0;
    guid++;

    ent->components = malloc((num_components + 1) * sizeof(component));
    printf("Allocing components array to add first component %p to entity %p\n", ent->components, ent);
    exit_w_one_on_failed_xalloc(ent->components);

    /* Our entities start at GUID #1 */
    ent->guid            = guid;

    ent->components[0]   = comp;
    ent->components[1]   = NULL;
    ent->num_components  = num_components;

    return ent;
}

void add_component_data_component(component* comp, void* comp_data)
{
    switch(comp->type) {
        case RENDER:
            comp->data.render_component_data = (render_component_data*)comp_data;
            printf("Adding render_component_data %p to component %p...\n", comp->data.render_component_data, comp);
            break;
        default:
            printf("Unknown component. Exiting with 1!\n");
            exit(1);
    }
}

entity* get_entity_by_guid(u_int32_t guid, super_world* ent_reg) {
    for(u_int32_t i = 0; i < ent_reg->num_entities; i++) {
        if(ent_reg->entities[i]->guid == guid)
            return ent_reg->entities[i];
    }
    return NULL;
}

entity* get_entity_from_scene(u_int32_t guid, scene* scene) {
   for(u_int32_t i = 0; i < scene->num_entities; i++) {
        if(scene->entities[i]->guid == guid)
            return scene->entities[i];
    }
    return NULL;
}

void add_component_to_entity(u_int32_t guid, component* comp, scene* scene) {
    entity* ent = get_entity_from_scene(guid, scene);
    u_int16_t new_size = (ent->num_components + 1) * sizeof(component);
    
    ent->components = realloc(ent->components, new_size);
    printf("Resizing component array to %u to add component %p to entity %p\n", new_size, comp, ent);
    exit_w_one_on_failed_xalloc(ent->components);

    ent->components[ent->num_components] = comp;
    ent->components[(ent->num_components) + 1] = NULL;
    ent->num_components++;
}

void free_entity_components(entity* entity)
{
    u_int16_t num_components = entity->num_components;
    u_int16_t reverse_index  = 0;

    /* We start with i at 1 because although we want to free the array in reverse */
    /* i == 0 will cause the reverse_index to be the last element of the array */
    /* which is NULL since our arrays are null terminated. And that will fail. */

    for(u_int16_t i = 1; i <= num_components; i++) {
        reverse_index = ((i - num_components) * -1);
        free_valid_ptr(entity->components[reverse_index]);
    }

}

entity** remove_entity_from_entity_array(entity** ents, entity* ent)
{
    u_int32_t size_orig_entities_array = get_size_null_terminated_entity_array(ents);
    printf("remove_entity_from_entity_array(): Entity array has size: %u\n", size_orig_entities_array);
    u_int32_t size_new_entity_array  = size_orig_entities_array - 1;

    entity** new_entity_array = malloc(size_new_entity_array * sizeof(entity*));
    exit_w_one_on_failed_xalloc(new_entity_array);
    printf("remove_entity_from_entity_array(): Alloced new entity array with size: %u\n", size_new_entity_array);
    
    for(u_int16_t i = 0; i < size_orig_entities_array; i++) {
		if(ent == ents[i]) {
		    printf("remove_entity_from_entity_array(): Skipping to be deleted entity %p. Not assigned to new array. Freeing instead ...\n", ent);
		    free_valid_ptr(ent);
		    printf("remove_entity_from_entity_array(): Freeing entity SUCCESSFUL!\n");
		} else {
			printf("remove_entity_from_entity_array(): Assigned entity %p to new array!\n", ent);
			new_entity_array[i] = ents[i];
		}
	}
	
	printf("remove_entity_from_entity_array(): Gonna iterate NEWLY created component array after element deletion: \n");
        for(u_int16_t i = 0; i < size_new_entity_array; i++) {
		    printf("remove_entity_from_entity_array(): Component #%u at address: %p\n", i, new_entity_array[i]);
	}
	
    return new_entity_array;
}

component** remove_component_from_component_array(component** comps, component* comp)
{
    u_int32_t size_orig_components_array = get_size_null_terminated_component_array(comps);
    printf("remove_component_from_entity_array(): Component array has size: %u\n", size_orig_components_array);
    u_int32_t i = 0;

    printf("remove_component_from_component_array(): Size of component array: %u\n", size_orig_components_array);

    while(comps[i]) {
        printf("remove_component_from_component_array(): searching for candidate component: #%u, address: %p\n", i, comps[i]);
        if(comps[i] == comp) {
            if(comps[i]->type == RENDER)
                SDL_DestroyTexture(comps[i]->data.render_component_data->sdl_tex_ptr);
			printf("remove_component_from_component_array(): Found component to free. Freeing %p ...\n", comp);
            free_valid_ptr(comp);
            printf("remove_component_from_component_array(): Freeing SUCCESS...\n");
            break;
        } else {
            i++;
            continue;
        }
    }

    u_int32_t index_before_removed_component  = i - 1;
    u_int32_t size_new_component_array        = size_orig_components_array - 1;

    component** new_component_array = malloc(size_new_component_array * sizeof(component*));
    exit_w_one_on_failed_xalloc(new_component_array);

    for(u_int32_t j = 0; j < index_before_removed_component + 1; j++) {
        new_component_array[j] = comps[j];
    }

    for(u_int32_t k = index_before_removed_component + 1; k < size_new_component_array; k++) {
        new_component_array[k] = comps[k];
    }

    free_valid_ptr(comps);
    
    printf("remove_component_from_component_array(): Gonna iterate NEWLY created component array after element deletion: \n");
    for(u_int16_t i = 0; i < size_new_component_array; i++) {
		printf("remove_component_from_component_array(): Component #%u at address: %p\n", i, new_component_array[i]);
	}

    return new_component_array;
}

void deregister_entity(entity* ent, scene* scn, super_world* sw)
{
    printf("deregister_entity()... \n");
    u_int16_t reverse_index = 0;
    u_int16_t num_components = ent->num_components;
    u_int32_t num_entitites  = sw->num_entities;

    printf("deregister_entity(): Number of components in entity: %u\n", num_components);

    /* We start with i at 1 because although we want to free the array in reverse */
    /* i == 0 will cause the reverse_index to be the last element of the array */
    /* which is NULL since our arrays are null terminated. And that will fail. */

    for(u_int16_t i = 1; i <= num_components; i++) {
        reverse_index = ((i - num_components) * -1) - 1;
        printf("deregister_entity(): reverse_index calculated for freeing component from component array: %u\n", reverse_index);
        printf("deregister_entity(): Will remove component from entity's component array now: %p\n", ent->components[reverse_index]);

        ent->components = remove_component_from_component_array(ent->components, ent->components[reverse_index]);
    }

    //Just taking care of the super_world entity array is enough
    //Just synchronize it with scene entity array by also assigning it to the scene entity array
    //And free old pointers of old (obsolete) entity arrays
    entity** obsolete_entity_array = sw->entities;
    sw->entities  = remove_entity_from("super_world", ent, sw);
    free_valid_ptr(obsolete_entity_array);
    obsolete_entity_array = scn->entities;
    scn->entities = sw->entities;
    free_valid_ptr(obsolete_entity_array);
    sw->num_entities = num_entitites - 1;
    scn->num_entities = num_entitites - 1;
    printf("DEBUG!\n");

    printf("deregister_entity(): done deregistering entity!\n");
}

entity** remove_entity_from(const char* scene_or_super_world_chooser, entity* ent, void* scene_or_super_world_struct_ptr)
{   
	entity** new_entity_array = NULL;
	scene* scn_ptr            = NULL;
	super_world* sw_ptr       = NULL;
	
	printf("remove_entity_from(): Will remove entity %p from %s\n", ent, scene_or_super_world_chooser);
    if(strcmp(scene_or_super_world_chooser, "scene") == 0) {
        scn_ptr            = (scene*)scene_or_super_world_struct_ptr;
        new_entity_array   = remove_entity_from_entity_array(scn_ptr->entities, ent);
    } else if(strcmp(scene_or_super_world_chooser, "super_world") == 0) {
        sw_ptr             = (super_world*)scene_or_super_world_struct_ptr;
        new_entity_array   =  remove_entity_from_entity_array(sw_ptr->entities, ent);
	}
        
    return new_entity_array;
}

bool get_entity_has_static_component(entity* ent) {
	for(u_int16_t i = 0; i < ent->num_components; i++) {
		if(ent->components[i]->type == STATIC) {
			return true;
		}
	}
	return false;
}

bool get_entity_has_dynamic_component(entity* ent) {
	for(u_int16_t i = 0; i < ent->num_components; i++) {
		if(ent->components[i]->type == DYNAMIC) {
			return true;
		}
	}
	return false;
}

void free_ecs(super_world* super_world)
{   
    printf("free_ecs(): Will destroy Entity Component System now!\n");
    for(u_int32_t i = 0; i < super_world->num_entities; i++) {
        for(u_int8_t j = 0; j < super_world->entities[i]->num_components; j++) {
            if(super_world->entities[i]->components[j]->type == RENDER) {
                printf("free_ecs(): Found RENDER component. Free SDL_Texture* first...\n");
                free_valid_ptr(super_world->entities[i]->components[j]->data.render_component_data->src_rect_ptr);
                free_valid_ptr(super_world->entities[i]->components[j]->data.render_component_data->dst_rect_ptr);
                SDL_DestroyTexture(super_world->entities[i]->components[j]->data.render_component_data->sdl_tex_ptr);
            }
        }
        printf("free_ecs(): Will deregister entity %p now!\n", super_world->entities[i]);
        deregister_entity(super_world->entities[i], super_world->scene, super_world);
    }
    
    printf("Freeing scene...\n");
    free_valid_ptr(super_world->scene);
    printf("Scene free'd successfully!\n");
    printf("Freeing super_world...\n");
    free_valid_ptr(super_world);
    printf("super_world free'd successfully!\n");
    printf("Entity component system and thus game is deleted...Bye cruel world!\n");
}

void render_system(scene* scene, SDL_Renderer* renderer) {
    //printf("Render system is running()...\n");
    for(u_int32_t i = 0; i < scene->num_entities; i++) {
        for(u_int8_t j = 0; j < scene->entities[i]->num_components; j++) {
            if(scene->entities[i]->components[j]->type == RENDER) {
				entity*      scene_ent  = scene->entities[i];
				component*   scene_comp = scene_ent->components[j];
				SDL_Texture* tex_ptr    = scene_comp->data.render_component_data->sdl_tex_ptr;
				SDL_Rect*    src_rect   = scene_comp->data.render_component_data->src_rect_ptr;
				SDL_Rect*    dst_rect   = scene_comp->data.render_component_data->dst_rect_ptr;
				bool         is_visible = scene_comp->data.render_component_data->visible;
				bool         ent_has_static_comp  = get_entity_has_static_component(scene_ent);
				bool         ent_has_dynamic_comp = get_entity_has_dynamic_component(scene_ent);
				bool         has_been_rendered    = scene_comp->data.render_component_data->has_been_rendered;
                                
                if(scene_comp->data.render_component_data->clear_renderer)
					    SDL_RenderClear(renderer);
					    
			    SDL_RenderCopy(renderer, tex_ptr, src_rect, dst_rect);
                
                if(!is_visible && ent_has_static_comp && !has_been_rendered) {					    
                    printf("Rendering a static entity such as a background: %p!\n", tex_ptr);
                    scene_comp->data.render_component_data->visible = true;
                    scene_comp->data.render_component_data->has_been_rendered = true;
                    SDL_RenderCopy(renderer, tex_ptr, src_rect, dst_rect);                    
                } else if(!is_visible && ent_has_dynamic_comp) {
					//printf("Rendering a dynamic entity such as a player: %p!\n", tex_ptr);
					scene_comp->data.render_component_data->visible = false;
					scene_comp->data.render_component_data->has_been_rendered = true;
				}                
			    if(scene_comp->data.render_component_data->clear_renderer)
                        SDL_RenderPresent(renderer);
            }
        }
    }
    SDL_RenderPresent(renderer);
}
/* #################################################################### */
/* ================ END FUNCTION SIGNATURES =========================== */
/* #################################################################### */
