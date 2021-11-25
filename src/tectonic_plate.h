#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>

/** If #include glfw3.h with
*   #define GLFW_INCLUDE_VULKAN
*   is used, then
*   #include <vulkan/vulkan.h>
*   and
*   #include <vulkan/vulkan_core.h>
*   are obsolete because GLFW
*   includes them already
**/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define TP_FALSE 0
#define TP_TRUE  1
#define DEBUG    TP_TRUE

/** Glossar
*  Identifiers are heavily abbreviated to avoid long names as good as possible
*  Vk        = Vulkan
*  Tp        = TectonicPlate
*  advances  = adv
*  arr       = array
*  app       = application
*  cmp       = compare
*  cntr      = counter
*  creat     = create
*  dev       = device
*  devs      = devices
*  dptr      = pointer pointer (double pointer)
*  dsld      = disabled
*  enbld     = enabled
*  extn      = extension
*  extns     = extensions
*  fam       = family
*  feat      = feature
*  feats     = features
*  frag      = fragment
*  frags     = fragments
*  gpu       = phys_dev
*  goi       = gpu of interest (GPU user/engine chose to use)
*  idx       = index
*  inf       = info
*  inst      = instance
*  ivald     = invalid
*  lay       = layer
*  lays      = layers
*  len       = length
*  maj       = major (version)
*  min       = minor (version)
*  mem       = memory
*  mlsamp    = multisampling
*  mod       = mode
*  mods      = modes
*  modl      = module
*  modls     = modules
*  n         = and / placeholder for numeric value
*  nm        = name
*  nms       = names
*  num       = number
*  dev       = device
*  logic_dev = logical device aka logical abstraction of GPU
*  p         = pointer
*  pat       = patch (version)
*  phys      = physical
*  pipl      = pipeline
*  prelim    = preliminary
*  prep      = prepare
*  preptn    = preparation
*  prop      = property
*  props     = properties
*  plmsone   = preliminary work for stage 1 (right before init of stage, so like stage 0.5 or so)
*  plmstwo   = preliminary work for stage 2 (right before init of stage, so like stage 1.5 or so)
               and so on. You will figure out plmsthree, plsmfour, plsmfive yourself ;)
*  res       = result
*  scss      = scissor
*  sone      = stage one
*  stg       = stage
*  str       = string
*  stwo      = stage two
*  strct     = struct
*  tptr      = triple pointer
*  sur       = surface
*  swc       = swapchain
*  phys_dev  = physical device aka GPU
*  q         = queue
*  qptr      = quadruple pointer
*  val       = value
*  vals      = values
*  vald      = valid
*  valdtr    = validator
*  vcs       = verteces
*  vx        = vertex
*  w         = with
**/


/* Typedefs */
typedef uint8_t boolean;

typedef struct Tp_Vk_Inst_Fam_Plmsone {
    VkApplicationInfo*    p_vk_app_inf;
    VkInstanceCreateInfo* p_vk_inst_create_inf;
    VkInstance*           p_vk_inst;
}Tp_Vk_Inst_Fam_Plmsone;

typedef struct Tp_GPUs_Plmsone {
    VkPhysicalDevice* gpu_arr;
    uint32_t*         num_gpus;
}Tp_GPUs_Plmsone;

typedef struct Tp_Lay_Props_Plmsone {
    VkLayerProperties*      p_vk_lay_props;
    uint32_t                num_lay_props;
}Tp_Lay_Props_Plmsone;

typedef struct Tp_Init_Vk_Stg_One {
    boolean                 is_initialized;
    void*                   last_stg;
    const char*             app_nm;
    uint32_t                maj;
    uint32_t                min;
    uint32_t                pat;
    VkPhysicalDevice*       p_goi;
    Tp_Vk_Inst_Fam_Plmsone* p_tp_vk_inst_fam;
    Tp_GPUs_Plmsone*        p_gpus;
    Tp_Lay_Props_Plmsone*   p_lay_props_data;
}Tp_Init_Vk_Stg_One;

typedef struct Tp_GPU_Props_Feats_Plmstwo {
    VkPhysicalDeviceProperties*       gpu_props;
    VkPhysicalDeviceFeatures*         gpu_feats;
    VkPhysicalDeviceMemoryProperties* gpu_mem_props;
}Tp_GPU_Props_Feats_Plmstwo;

typedef struct Tp_Init_Vk_Stg_Two {
    boolean                     is_initialized;
    Tp_Init_Vk_Stg_One*         last_stg;
    Tp_GPUs_Plmsone*            basic_gpu_inf;
    Tp_GPU_Props_Feats_Plmstwo* adv_gpu_inf;
}Tp_Init_Vk_Stg_Two;

typedef struct Tp_Init_Vk_Stg_Three {
    boolean                  is_initialized;
    Tp_Init_Vk_Stg_Two*      last_stg;
    VkQueueFamilyProperties* p_vk_q_fam_props;
    VkDeviceQueueCreateInfo* p_vk_dev_q_creat_inf;
    VkDeviceCreateInfo*      p_vk_dev_creat_inf;
}Tp_Init_Vk_Stg_Three;

typedef struct Tp_Init_Vk_Stg_Four {
    boolean                  is_initialized;
    Tp_Init_Vk_Stg_Three*    last_stg;
    VkDevice*                p_vk_dev;
}Tp_Init_Vk_Stg_Four;

typedef struct Tp_Ext_Props_Plmsfive {
    const char*            lay_nm;
    uint32_t               num_ext_props;
    VkExtensionProperties* p_vk_ext_props;
}Tp_Ext_Props_Plmsfive;

typedef struct Tp_Q_Plmsfive {
    VkQueue*       p_vk_q;
}Tp_Q_Plmsfive;

typedef struct Tp_Init_Vk_Stg_Five {
    boolean                  is_initialized;
    Tp_Init_Vk_Stg_Four*     last_stg;
    Tp_Q_Plmsfive*           p_vk_q_data;
    Tp_Ext_Props_Plmsfive*   p_vk_ext_props_data;
}Tp_Init_Vk_Stg_Five;

typedef struct Tp_Sur_Plmssix {
    uint32_t                  size_vk_sur_form_khr_arr;
    uint32_t                  size_vk_pres_mod_khr_arr;
    VkSurfaceKHR*             p_vk_sur_khr;
    VkSurfaceCapabilitiesKHR* p_vk_sur_caps_khr;
    VkSurfaceFormatKHR*       p_vk_sur_form_khr;
    VkPresentModeKHR*         p_vk_pres_mods_khr;
    SDL_Window*               p_sdl_window;
    uint32_t                  width;
    uint32_t                  height;
    const char*               title;
}Tp_Sur_Plmssix;

typedef struct Tp_Swc_Creat_Inf_Plmssix {
     VkSwapchainCreateInfoKHR* p_vk_swc_creat_inf_khr;
}Tp_Swc_Creat_Inf_Plmssix;

typedef struct Tp_Init_Vk_Stg_Six {
    boolean                    is_initialized;
    Tp_Init_Vk_Stg_Five*       last_stg;
    Tp_Sur_Plmssix*            p_stg_six_vk_sur_data;
    Tp_Swc_Creat_Inf_Plmssix*  p_stg_six_src_creat_inf_data;
}Tp_Init_Vk_Stg_Six;

typedef struct Tp_Init_Vk_Stg_Seven {
    boolean                   is_initialized;
    Tp_Init_Vk_Stg_Six*       last_stg;
    boolean                   surface_support;
    VkSwapchainKHR*           p_vk_swc_khr;
    VkImageViewCreateInfo*    p_img_vu_creat_inf;
    VkImage*                  p_vk_img_arr;
}Tp_Init_Vk_Stg_Seven;

/* Helper functions */
const uint32_t tp_str_len(const char *str);
const uint32_t tp_str_cmp(const char* stra, const char* strb);
char*          tp_str_cpy(char *dest, const char *src, size_t n);
void           tp_exit_on_failed_malloc(void* ptr, const char* msg, int32_t err_num);
void           tp_exit_on_bad_vk_res(VkResult* p_vk_res, const char* msg, int32_t err_num);
GLFWwindow*    tp_start_glfw(GLFWwindow* p_glfw_win, const uint32_t width, const uint32_t height, const char* title);
VkBool32*      check_for_sur_support(Tp_Init_Vk_Stg_Six* p_stg_six_strct);
void           act_on_sur_support_res(Tp_Init_Vk_Stg_Six* p_stg_six_strct, VkBool32* p_system_has_surface_support);
int            tp_posix_get_file_size(const char* filename);
unsigned char* tp_posix_read_shader_file_bin(const char* filename);

/* Simple wrappers for creating Vk structs or Vk arrays */
/* have tp_create_ in their name */
VkApplicationInfo*                tp_create_vk_app_inf(const char* app_nm, uint32_t maj, uint32_t min, uint32_t pat);
VkInstanceCreateInfo*             tp_create_vk_inst_creat_inf(VkApplicationInfo* p_vk_app_inf, SDL_Window* p_sdl_window);
VkInstance*                       tp_create_vk_inst(VkInstanceCreateInfo* p_vk_inst_creat_inf);
void                              tp_destroy_vk_inst(VkInstance vk_inst, VkApplicationInfo* p_vk_app_inf, VkInstanceCreateInfo* p_vk_inst_create_inf);
Tp_GPUs_Plmsone*                  tp_create_vk_phys_devs(VkInstance* p_vk_inst);
VkPhysicalDeviceProperties*       tp_create_vk_phys_dev_props(VkPhysicalDevice* p_vk_phys_dev);
VkPhysicalDeviceFeatures*         tp_create_phys_dev_feats(VkPhysicalDevice* p_vk_phys_dev);
VkPhysicalDeviceMemoryProperties* tp_create_phys_dev_mem_props(VkPhysicalDevice* p_vk_phys_dev);
VkQueueFamilyProperties*          tp_create_q_fam_props_for_gpu(VkPhysicalDevice* p_goi);
VkDeviceQueueCreateInfo*          tp_create_dev_q_creat_inf();
VkDeviceCreateInfo*               tp_create_dev_creat_info(VkDeviceQueueCreateInfo* p_gpu_q_creat_inf, char** pp_en_lay_nms, const uint32_t en_lay_cnt, VkPhysicalDeviceFeatures* p_vk_gpu_feats, char** pp_en_ext_nms, const uint32_t en_ext_cnt);
VkDevice*                         tp_create_logical_gpu(Tp_Init_Vk_Stg_Three* p_tp_stg_three_strct);
VkQueue*                          tp_create_q(Tp_Init_Vk_Stg_Four* p_tp_stg_four_strct);
VkExtensionProperties*            tp_create_vk_ext_props(const char* lay_nm, uint32_t* p_ext_props_cntr);
char**                            tp_get_avail_lay_nms(Tp_Init_Vk_Stg_One* tp_init_vk_stg_one_data);
char**                            tp_get_avail_ext_nms(Tp_Init_Vk_Stg_Five* tp_init_vk_stg_five_data);
VkSurfaceKHR*                     tp_glfw_create_win_surface_wrapper(Tp_Init_Vk_Stg_One* tp_init_vk_stg_one_data, SDL_Window* p_sdl_window);
VkSurfaceCapabilitiesKHR*         tp_create_vk_sur_caps_khr(VkPhysicalDevice* p_goi, VkSurfaceKHR* p_vk_sur_khr);
VkSurfaceFormatKHR*               tp_create_vk_sur_form(VkPhysicalDevice* p_goi, VkSurfaceKHR* p_vk_sur_khr, uint32_t* p_vk_sur_forms_cntr);
VkPresentModeKHR*                 tp_create_vk_pres_mod_khr(VkPhysicalDevice* p_goi, VkSurfaceKHR* p_vk_sur_khr, uint32_t* p_pres_modes_cntr);
VkSwapchainCreateInfoKHR*         tp_create_vk_swc_creat_inf_khr(Tp_Sur_Plmssix* p_prelim_stg_six_strct_sur_data, const uint32_t width, const uint32_t height);
VkSwapchainKHR*                   tp_create_vk_swc_khr(Tp_Init_Vk_Stg_Six* p_stg_six_strct);
void                              tp_destroy_vk_swc_khr(Tp_Init_Vk_Stg_Seven* p_stg_seven_strct);
VkImage*                          tp_create_vk_img_arr(VkSwapchainKHR* p_vk_swc_khr, Tp_Init_Vk_Stg_Six* p_stg_six_strct, uint32_t* p_img_cntr);
VkImageViewCreateInfo*            tp_create_vk_ing_vu_creat_inf(VkImage* p_vk_img_arr);

/* Wrapper of wrappers: E.g. the user just have to use one function to initialize VkInstance */
/* have tp_init_ in their name */
Tp_Vk_Inst_Fam_Plmsone*       tp_init_prelim_stg_one_strct_vk_inst_fam(const char* app_nm, uint32_t maj, uint32_t min, uint32_t pat, SDL_Window* p_sdl_window);
Tp_Lay_Props_Plmsone*         tp_init_vk_inst_lay_props(uint32_t* p_lay_props_cntr);
Tp_GPU_Props_Feats_Plmstwo*   tp_init_gpu_props_feats(VkPhysicalDevice* p_goi);
Tp_Q_Plmsfive*                tp_init_vk_q(Tp_Init_Vk_Stg_Four* p_tp_stg_four_strct);
Tp_Ext_Props_Plmsfive*        tp_init_ext_props(const char* lay_nm);
Tp_Sur_Plmssix*               tp_init_sur(Tp_Init_Vk_Stg_One* p_tp_init_vk_stg_one_data, const uint32_t width, const uint32_t height, const char* title, SDL_Window* win);
Tp_Swc_Creat_Inf_Plmssix*     tp_init_swc_creat_inf(Tp_Sur_Plmssix* p_prelim_stg_six_strct_sur_data, const uint32_t width, const uint32_t height);

/** We must go deeper: Wrapper of wrapper of wrappers
*   Combines many of the above second level init functions
*   have tp_super_init_ in their name and are the final stage initializers **/
Tp_Init_Vk_Stg_One*   tp_super_init_vulkan_stg_one(const char* app_nm, uint32_t maj, uint32_t min, uint32_t pat, SDL_Window* p_sdl_window);
Tp_Init_Vk_Stg_Two*   tp_super_init_vulkan_stg_two(Tp_Init_Vk_Stg_One* p_tp_init_vk_stg_one, VkPhysicalDevice* p_goi);
Tp_Init_Vk_Stg_Three* tp_super_init_vulkan_stg_three(Tp_Init_Vk_Stg_Two* p_tp_init_vk_stg_two, char** pp_en_lay_nms, const uint32_t en_lay_cnt, VkPhysicalDeviceFeatures* p_vk_gpu_feats, char** pp_en_ext_nms, const uint32_t en_ext_cnt);
Tp_Init_Vk_Stg_Four*  tp_super_init_vulkan_stg_four(Tp_Init_Vk_Stg_Three* p_tp_init_vk_stg_three);
Tp_Init_Vk_Stg_Five*  tp_super_init_vulkan_stg_five(Tp_Init_Vk_Stg_Four* p_tp_stg_four_strct, const char* lay_nm);
Tp_Init_Vk_Stg_Six*   tp_super_init_vulkan_stg_six(Tp_Init_Vk_Stg_Five* p_tp_stg_five_data, uint32_t width, uint32_t height, const char* title, SDL_Window* win);
Tp_Init_Vk_Stg_Seven* tp_super_init_vulkan_stg_seven(Tp_Init_Vk_Stg_Six* p_stg_six_strct);

/* Functions that print helpful info for the user to the screen */
/* have tp_print_<function name without tp_ prefix> in their name */
void tp_print_vk_app_inf(Tp_Init_Vk_Stg_One* p_tp_vk_init, uint8_t debug);
void tp_print_inst_creat_inf(Tp_Init_Vk_Stg_One* p_tp_strct_stg_one, uint8_t debug);
void tp_print_vk_inst_lay_props(Tp_Init_Vk_Stg_One* stg_one_data, uint8_t debug);
void tp_print_vk_phys_dev_props(VkPhysicalDeviceProperties* p_gpu_props, uint8_t debug);
void tp_print_vk_phys_dev_feats(VkPhysicalDeviceFeatures* p_gpu_feats, uint8_t debug);
void tp_print_vk_phys_dev_mem_props(VkPhysicalDeviceMemoryProperties* p_gpu_mem_props, uint8_t debug);
void tp_print_super_init_stg_two(Tp_Init_Vk_Stg_Two* p_gpu_inf, const char* what, uint8_t debug);
void tp_print_q_fam_props(VkQueueFamilyProperties* p_vk_q_fam_props, uint8_t debug);
void tp_print_dev_q_creat_inf(VkDeviceQueueCreateInfo* p_gpu_q_creat_inf, uint8_t debug);
void tp_print_create_dev_creat_info(VkDeviceCreateInfo* p_vk_dev_creat_inf, uint8_t debug);
void tp_print_vk_ext_props(VkExtensionProperties* vk_ext_props_arr, uint32_t ext_props_cntr);
void tp_print_super_init_stg_five(VkExtensionProperties* vk_ext_props_arr, uint32_t ext_props_cntr);
void tp_print_vk_surf_caps_khr(VkSurfaceCapabilitiesKHR* p_vk_sur_caps);
void tp_print_vk_sur_forms(Tp_Init_Vk_Stg_Six* p_stg_six_strct);
void tp_print_vk_pres_mods(Tp_Init_Vk_Stg_Six* p_stg_six_strct);
