#include "sjr.h"
#include "sdl.h"
#include "tectonic_plate.h"

/* #################################################################### */
/* ========================== START MAIN ============================== */
/* #################################################################### */
int main(int argc, char* argv[])
{
    /* =========================== START SETUP SDL BASICS ============================ */
    
    init_sdl();
    
    bool game_is_running = false;
    
    SDL_Vulkan_LoadLibrary("/usr/lib/libvulkan.so");

    SDL_Window* top_win = init_window("SJR", WIDTH, HEIGHT);
    
    int window_refresh_rate = get_refresh_rate(top_win);
    printf("Your monitor's refresh rate is %d Hz.\n", window_refresh_rate);
    
    SDL_Renderer* renderer = NULL;
    renderer = init_renderer(-1, top_win);
    
    /* ================================================================================================= */
    /**
    *  STAGE ONE: Creates VkApplicationInfo, VkInstanceCreateInfo. From those the VkInstance is created!
    *  Finally the GPUs in your system are detected and are stored in and array
    *  in a struct as well as the number of GPUs found
    **/
    /* ================================================================================================= */
    if(!glfwInit()) {
        printf("Initializing GLFW failed. Needed to get required instance extensions\n");
        exit(-3);
    }

    /* Prepare Stage 1*/
    const char* app_nm   = "TectonicPlate Example Triangle";
    uint32_t app_maj_ver = 0;
    uint32_t app_min_ver = 0;
    uint32_t app_pat_ver = 1;

    /* Init Stage 1 */
    Tp_Init_Vk_Stg_One* tp_strct_stg_one = tp_super_init_vulkan_stg_one(app_nm, app_maj_ver, app_min_ver, app_pat_ver, top_win);

    char** detectd_avail_lay_nms = tp_get_avail_lay_nms(tp_strct_stg_one);
    /* Print = test Stage 1 info */
    tp_print_vk_app_inf(tp_strct_stg_one, DEBUG);
    tp_print_inst_creat_inf(tp_strct_stg_one, DEBUG);

    /* ================================================================================================= */
    /**
    *  STAGE TWO: Choose your GPU. From that GPU fill VkPhysicalDeviceProperties,
    *  VkPhysicalDeviceFeatures, and VkPhysicalDeviceMemoryProperties
    *  Combine basic GPU info from stage one with the advanced one in a single struct Tp_Init_Vk_Stg_Two
    **/
    /* ================================================================================================= */

    /* Prepare Stage 2 */
    /* Your basic GPU info and number of GPUs are stored in */
    /* tp_strct_stg_one->p_gpus->gpu_arr and tp_strct_stg_one->p_gpus->num_gpus, respectively */
    uint32_t num_gpus_this_system = *(tp_strct_stg_one->p_gpus->num_gpus);

    /* For now I don't know how to detect the best GPU for some puposes, yet */
    /* I am on a PC with an AMD Radeon RX570. It is my only activated and thus detected GPU */
    VkPhysicalDevice* gpu_array    = tp_strct_stg_one->p_gpus->gpu_arr;
    VkPhysicalDevice gpu_of_choice = gpu_array[0];
    tp_strct_stg_one->p_goi = &gpu_of_choice;

    /* Init Stage 2 */
    Tp_Init_Vk_Stg_Two* complete_gpu_info = tp_super_init_vulkan_stg_two(tp_strct_stg_one, &gpu_of_choice);

    /* Print = test Stage 2 Info*/
    tp_print_super_init_stg_two(complete_gpu_info, "all", DEBUG);

    /* ================================================================================================= */
    /**
    *  STAGE THREE: From chosen GPU VkQueueFamilyProperties will be created.
    *  then VkDeviceQueueCreateInfo and VkDeviceCreateInfo follow
    *  Until our stage milestone, the initialization of the logical representation of our GPU.
    **/
    /* ================================================================================================= */

    Tp_Init_Vk_Stg_Three* p_tp_stg_three_strct = tp_super_init_vulkan_stg_three(complete_gpu_info, NULL, 0, NULL, NULL, 0);
    tp_print_dev_q_creat_inf(p_tp_stg_three_strct->p_vk_dev_q_creat_inf, DEBUG);
    tp_print_q_fam_props(p_tp_stg_three_strct->p_vk_q_fam_props, DEBUG);
    tp_print_create_dev_creat_info(p_tp_stg_three_strct->p_vk_dev_creat_inf, DEBUG);

    /* ================================================================================================= */
    /**
    *  STAGE FOUR: "Just" creates VkDevice, the logical representation of the GPU. Vulkan will only
    *  directly talk with logical devices, not the physical ones. Stage three was basically already a lot
    *  of preliminary work for stage four. That's why this stage seems so "small".
    **/
    /* ================================================================================================= */

    Tp_Init_Vk_Stg_Four* p_stg_four_strct = tp_super_init_vulkan_stg_four(p_tp_stg_three_strct);

    /* ================================================================================================= */
    /**
    *  STAGE FIVE: Contains VkExtensionProperties and VkQueue
    **/
    /* ================================================================================================= */

    Tp_Init_Vk_Stg_Five* p_stg_five_strct = tp_super_init_vulkan_stg_five(p_stg_four_strct, NULL);
    tp_print_super_init_stg_five(p_stg_five_strct->p_vk_ext_props_data->p_vk_ext_props, p_stg_five_strct->p_vk_ext_props_data->num_ext_props);
    char** detectd_avail_ext_nms = tp_get_avail_ext_nms(p_stg_five_strct);

    /* ================================================================================================= */
    /**
    *  STAGE SIX: Creates GLFWwindow VkSurfaceKHR, VkSurfaceCapabilitiesKHR,
    *  VkSurfaceFormatKHR, VkSwapchainCreateInfo
    **/
    /* ================================================================================================= */

    Tp_Init_Vk_Stg_Six* p_stg_six_strct = tp_super_init_vulkan_stg_six(p_stg_five_strct, WIDTH, HEIGHT, tp_strct_stg_one->app_nm, top_win);
    tp_print_vk_surf_caps_khr(p_stg_six_strct->p_stg_six_vk_sur_data->p_vk_sur_caps_khr);
    tp_print_vk_sur_forms(p_stg_six_strct);
    tp_print_vk_pres_mods(p_stg_six_strct);

    /* ================================================================================================= */
    /**
    *  STAGE Seven: Creates VkSwapchainKHR, VkImage Array, VkImageViewCreateInfo
    *  and stores boolean that indicates Surface support (Will always be true since
    *  program would have crashed and stage seven would have never finished
    **/
    /* ================================================================================================= */

    //Tp_Init_Vk_Stg_Seven* p_stg_seven_strct = tp_super_init_vulkan_stg_seven(p_stg_six_strct);

    //while(!glfwWindowShouldClose(p_stg_six_strct->p_stg_six_vk_sur_data->p_glfw_window)) {
    //    glfwPollEvents();
    //}

    /* ================================================================================================= */
    /**
    *  free memory
    **/
    /* ================================================================================================= */
    /* =========================== END SETUP SDL BASICS ============================ */

    /* =========================== START SETUP INITIAL GAME STATES (PRE GAME-LOOP) ============================ */
    game_is_running = true;
    const float time_step = FRAMECAP;
    float accumulator = 0.0f;
    float current_time = get_runtime_in_seconds();
    SDL_Event event;

    /* In the hierarchy this is higher than a scene. This could be seen as the game world or even the real world/memory */
    /* It keeps all entities until explicitely deleted. These entities must not necessarily be shown in a scene! */
    super_world* super_world     = create_super_world();

    /* I want to try and keep one scene that is always up-to-date, so just containing what is important for the current frame */
    scene* scene = create_scene();

    connect_super_world_w_scene(super_world, scene);

    // Start setup level 1 screen
    SDL_Texture* level_one_tex_ptr = NULL;
    level_one_tex_ptr = load_texture(level_one_tex_ptr, "../assets/images/backgrounds/FlatNatureArt.png", top_win, renderer);
    render_component_data* rcd_level_one = create_render_component_data(level_one_tex_ptr, NULL, NULL);
    entity*                ent_level_one = register_entity(RENDER, rcd_level_one, super_world, scene);
    component*             static_component   = malloc(1 * sizeof(component));
    static_component->type = STATIC;
    add_component_to_entity(1, static_component, scene);

    if(!level_one_tex_ptr) {
        printf("Initializing level 1 SDL_Texture* failed!\n");
        SDL_GetError();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(top_win);
        SDL_Quit();
    }
    
    // End setup level 1 screen

    // Start setup player character
    SDL_Texture* player_one_tex_ptr = NULL;
    player_one_tex_ptr = load_texture(player_one_tex_ptr, "../assets/images/chars/players/1/idle0_right_64x64.png", top_win, renderer);

    SDL_Rect* player_one_src_sdl_rect_ptr = create_sdl_rect(0, 0, 64, 64);
    SDL_Rect* player_one_dst_sdl_rect_ptr = create_sdl_rect(500, 523, 64, 64);
    render_component_data* rcd_player_one = create_render_component_data(player_one_tex_ptr, player_one_src_sdl_rect_ptr, player_one_dst_sdl_rect_ptr);
    entity*                ent_player_one = register_entity(RENDER, rcd_player_one, super_world, scene);
    component*             dynamic_comp   = malloc(1 * sizeof(component));
    dynamic_comp->type = DYNAMIC;
    add_component_to_entity(2, dynamic_comp, scene);

    // End setup player character
    /* =========================== END SETUP INITIAL GAME STATES (PRE GAME-LOOP)============================ */

    /* ============================== START GAME LOOP =================================== */
    while(game_is_running){
        int start_ticks = SDL_GetTicks();
        float new_time = get_runtime_in_seconds();
        float frame_time = new_time - current_time;

        current_time = new_time;
        accumulator += frame_time;

        while(accumulator >= time_step) {
            while(SDL_PollEvent(&event)) {
                switch(event.type) {
					case SDL_QUIT:
					    game_is_running = false;
					    break;
					case SDL_KEYDOWN:
					    switch(event.key.keysym.sym) {
							case SDLK_a:
							case SDLK_LEFT:
							    scene->entities[1]->components[0]->data.render_component_data->dst_rect_ptr->x -= 12;
							    break;
							case SDLK_d:
							case SDLK_RIGHT:
							    scene->entities[1]->components[0]->data.render_component_data->dst_rect_ptr->x += 12;
							    break;
						}
                }   break;
            }
            accumulator -= time_step;
        }

        const float alpha = accumulator / time_step;
        /* ============================== START EVENT LOOP =================================== */

        /* ============================== END EVENT LOOP ==================================== */        
        int frame_ticks = SDL_GetTicks() - start_ticks;
        
        /* Render entities */        
        render_system(scene, renderer);
        
        if(frame_ticks < (1000 / window_refresh_rate)) {
            SDL_Delay(1000 / (window_refresh_rate - frame_ticks));
        }
    }
    /* ============================== END GAME LOOP ==================================== */

    /* ============================== START FREE MEMORY AND END GAME =================== */
    deinit_sdl(super_world, top_win, renderer);
    /* ============================== END FREE MEMORY AND END GAME =================== */

    return true;
}
/* #################################################################### */
/* ========================== END MAIN ================================ */
/* #################################################################### */
