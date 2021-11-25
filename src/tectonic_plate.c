#include "tectonic_plate.h"

const uint32_t tp_str_len(const char *str)
{
        /*Does NOT account for NULL delimiter!*/
        const char *s = NULL;
        for (s = str; *s; ++s);
        return (s - str);
}

const uint32_t tp_str_cmp(const char* stra, const char* strb)
{
    const uint32_t len_stra = tp_str_len(stra) + 1;
    const uint32_t len_strb = tp_str_len(strb) + 1;

    if(len_stra == len_strb) {
        while(stra && *stra == *strb) {
            stra++;
            strb++;
            if(*stra == '\0' && *strb == '\0') {
                return 1;
            } else if(*stra != *strb) {
                return 0;
            }
            continue;
        }
    }

    return 0;
}

char* tp_str_cpy(char *dest, const char *src, size_t n)
{
    /* Copied from Linux man 3 strcpy */
    size_t i = 0;

    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for ( ; i < n; i++)
        dest[i] = '\0';

    return dest;
}

void tp_exit_on_failed_malloc(void* ptr, const char* msg, int32_t err_num)
{
    if(ptr == NULL) {
        printf("%s", msg);
        exit(err_num);
    }
}

void tp_exit_on_bad_vk_res(VkResult* p_vk_res, const char* msg, int32_t err_num)
{
    if(*p_vk_res != VK_SUCCESS) {
      printf("%s\n", msg);
      exit(err_num);
    }
}

VkApplicationInfo* tp_create_vk_app_inf(const char* app_nm, uint32_t maj, uint32_t min, uint32_t pat)
{
    VkApplicationInfo* p_vk_app_inf = malloc(sizeof(VkApplicationInfo));
    tp_exit_on_failed_malloc(p_vk_app_inf, "Mallocing of VkApplicationInfo* failed\n", -1);

    p_vk_app_inf->sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    p_vk_app_inf->pNext              = NULL;
    p_vk_app_inf->pApplicationName   = app_nm;
    p_vk_app_inf->applicationVersion = VK_MAKE_VERSION(maj, min, pat);
    p_vk_app_inf->pEngineName        = "TectonicPlate";
    p_vk_app_inf->engineVersion      = VK_MAKE_VERSION(0, 0, 1);
    p_vk_app_inf->apiVersion         = VK_API_VERSION_1_2;

    return p_vk_app_inf;
}

Tp_Lay_Props_Plmsone* tp_init_vk_inst_lay_props(uint32_t* p_lay_props_cntr)
{
     Tp_Lay_Props_Plmsone* p_prelim_stg_three_strct_lay_props_data = malloc(sizeof(Tp_Lay_Props_Plmsone));
     tp_exit_on_failed_malloc(p_prelim_stg_three_strct_lay_props_data, "Malloc of Tp_Lay_Props_Plmsone failed!\n", -23);

     uint32_t lay_props_cntr = *p_lay_props_cntr;
     vkEnumerateInstanceLayerProperties(&lay_props_cntr, NULL);

     /* pt_vk_lay_props is a pointer to an array */
     VkLayerProperties* p_vk_lay_props = malloc(lay_props_cntr * sizeof(VkLayerProperties));
     tp_exit_on_failed_malloc(p_vk_lay_props, "Malloc of VkLayerProperties failed!\n", -24);

     vkEnumerateInstanceLayerProperties(&lay_props_cntr, p_vk_lay_props);

     p_prelim_stg_three_strct_lay_props_data->p_vk_lay_props = p_vk_lay_props;
     p_prelim_stg_three_strct_lay_props_data->num_lay_props = lay_props_cntr;

     return p_prelim_stg_three_strct_lay_props_data;
}

void tp_print_vk_inst_lay_props(Tp_Init_Vk_Stg_One* stg_one_data, uint8_t debug)
{
    if(debug > 0) {
        for(uint32_t i = 0; i < stg_one_data->p_lay_props_data->num_lay_props; i++) {
            printf("==========\nPrinting layer properties of layer #%d:\n\n", i);
            printf("Layer name: %s\n", (stg_one_data->p_lay_props_data->p_vk_lay_props)[i].layerName);
            printf("Layer spec version: %u\n", (stg_one_data->p_lay_props_data->p_vk_lay_props)[i].specVersion);
            printf("Layer impl version: %u\n", (stg_one_data->p_lay_props_data->p_vk_lay_props)[i].implementationVersion);
            printf("Layer description: %s\n", (stg_one_data->p_lay_props_data->p_vk_lay_props)[i].description);
            printf("==========\n");
        }
    }
}

VkInstanceCreateInfo* tp_create_vk_inst_creat_inf(VkApplicationInfo* p_vk_app_inf, SDL_Window* p_sdl_window)
{
    VkInstanceCreateInfo* p_vk_inst_creat_inf = malloc(sizeof(VkInstanceCreateInfo));
    tp_exit_on_failed_malloc(p_vk_inst_creat_inf, "Mallocing of VkInstanceCreateInfo* failed!\n", -2);

    uint32_t sdl_ext_cntr               = 0;

    //const char** prelim_required_exts    = NULL;
    SDL_Vulkan_GetInstanceExtensions(p_sdl_window, &sdl_ext_cntr, NULL);
    const char** required_exts                 = malloc(sdl_ext_cntr * sizeof(char*));
    tp_exit_on_failed_malloc(required_exts, "Mallocing of string array for req. instance extension names failed!\n", -4);
    SDL_Vulkan_GetInstanceExtensions(p_sdl_window, &sdl_ext_cntr, required_exts);
    uint32_t len_cur_str_stack_arr       = 0;

    /**if(glfw_ext_cntr > 0) {
       for(uint32_t i = 0; i < glfw_ext_cntr; i++) {
            len_cur_str_stack_arr = tp_str_len(prelim_required_exts[i]) + 1;
            required_exts[i] = malloc(len_cur_str_stack_arr * sizeof(char));
            tp_exit_on_failed_malloc(required_exts[i], "Mallocing of string for req. instance extension names failed!\n", -5);
            required_exts[i] = tp_str_cpy(required_exts[i], prelim_required_exts[i], len_cur_str_stack_arr);
        }
    }**/

    char** inst_val_lay_nm_arr = malloc(sizeof(char*));
    tp_exit_on_failed_malloc(inst_val_lay_nm_arr, "Mallocing of string array for req. instance layer name failed!\n", -6);
    uint32_t len_val_lay_nm = tp_str_len("VK_LAYER_KHRONOS_validation") + 1;
    char* inst_val_lay_nm = malloc(len_val_lay_nm * sizeof(char));
    tp_exit_on_failed_malloc(inst_val_lay_nm, "Mallocing of string for req. instance layer name failed!\n", -7);
    inst_val_lay_nm_arr[0] = "VK_LAYER_KHRONOS_validation";

    p_vk_inst_creat_inf->sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    p_vk_inst_creat_inf->pNext                   = NULL;
    p_vk_inst_creat_inf->flags                   = 0;
    p_vk_inst_creat_inf->pApplicationInfo        = p_vk_app_inf;
    p_vk_inst_creat_inf->enabledLayerCount       = 1;
    p_vk_inst_creat_inf->ppEnabledLayerNames     = (const char* const*)inst_val_lay_nm_arr;
    p_vk_inst_creat_inf->enabledExtensionCount   = sdl_ext_cntr;

    if(sdl_ext_cntr > 0)
        p_vk_inst_creat_inf->ppEnabledExtensionNames = (const char* const*)required_exts;
    else
        p_vk_inst_creat_inf->ppEnabledExtensionNames = NULL;

    return p_vk_inst_creat_inf;
}

VkInstance* tp_create_vk_inst(VkInstanceCreateInfo* p_vk_inst_creat_inf)
{
    VkInstance* p_vk_inst = malloc(sizeof(VkInstance));

    /* Second argument NULL means Vulkan will use its own memory allocator */
    printf("\n\nInitializing Stage 1 ...\n\n");
    VkResult res = vkCreateInstance(p_vk_inst_creat_inf, NULL, p_vk_inst);
    tp_exit_on_bad_vk_res(&res, "Initialization of VkInstance failed!\n", -8);

    printf("\n\nVulkan instance initialized. Stage 1 milestone complete!\n\n");
    printf("Pointer to VkInstance has address: %p\n", p_vk_inst);

    return p_vk_inst;
}

Tp_Vk_Inst_Fam_Plmsone* tp_init_prelim_stg_one_strct_vk_inst_fam(const char* app_nm, uint32_t maj, uint32_t min, uint32_t pat, SDL_Window* p_sdl_window)
{
    VkApplicationInfo* p_vk_app_inf = tp_create_vk_app_inf(app_nm, maj, min, pat);
    VkInstanceCreateInfo* p_vk_inst_create_inf = tp_create_vk_inst_creat_inf(p_vk_app_inf, p_sdl_window);
    VkInstance* p_vk_inst = tp_create_vk_inst(p_vk_inst_create_inf);

    Tp_Vk_Inst_Fam_Plmsone* p_rs = malloc(sizeof(Tp_Vk_Inst_Fam_Plmsone));
    tp_exit_on_failed_malloc(p_rs, "Mallocing of Tp_Vk_Inst_Fam_Plmsone* failed!\n", -9);

    p_rs->p_vk_app_inf = p_vk_app_inf;
    p_rs->p_vk_inst_create_inf = p_vk_inst_create_inf;
    p_rs->p_vk_inst = p_vk_inst;

    return p_rs;
}

void tp_destroy_vk_inst(VkInstance vk_inst, VkApplicationInfo* p_vk_app_inf, VkInstanceCreateInfo* p_vk_inst_create_inf)
{
    /* Does one need to free p_vk_inst_creat_inf->ppEnabledLayerNames and */
    /* p_vk_inst_creat_inf->ppEnabledExtensionNames ?*/
    free(p_vk_inst_create_inf);
    free(p_vk_app_inf);
    vkDestroyInstance(vk_inst, NULL);
}

Tp_GPUs_Plmsone* tp_create_vk_phys_devs(VkInstance* p_vk_inst)
{
    uint32_t detected_gpus = 0;
    VkResult res = vkEnumeratePhysicalDevices(*p_vk_inst, &detected_gpus, NULL);
    tp_exit_on_bad_vk_res(&res, "Enumerating physical device(s) (GPUs) failed!\n", -10);
    VkPhysicalDevice* gpu_arr = malloc(detected_gpus * sizeof(VkPhysicalDevice));
    tp_exit_on_failed_malloc(gpu_arr, "Mallocing of VkPhysicalDevice array for detected GPUs failed!\n", -11);

    if(detected_gpus > 0) {
        VkResult res = vkEnumeratePhysicalDevices(*p_vk_inst, &detected_gpus, gpu_arr);
        tp_exit_on_bad_vk_res(&res, "Initializing array with physical device(s) aka GPU(s) failed\n", -12);
    }

    Tp_GPUs_Plmsone* p_rs = malloc(sizeof(Tp_GPUs_Plmsone));
    tp_exit_on_failed_malloc(p_rs, "Mallocing of Tp_GPUs_Plmsone failed!\n", -13);
    p_rs->gpu_arr = gpu_arr;
    p_rs->num_gpus = &detected_gpus;

    return p_rs;
}

Tp_Init_Vk_Stg_One* tp_super_init_vulkan_stg_one(const char* app_nm, uint32_t maj, uint32_t min, uint32_t pat, SDL_Window* p_sdl_window)
{
    /* Stage 1 Creates VkApplicationInfo, VkInstanceCreateInfo. From those the VkInstance is created. */
    /* Finally the GPUs in your system are detected and are stored in and array in a struct as well as the number of GPUs found */

    Tp_Init_Vk_Stg_One* p_tp_strct_stg_one         = malloc(sizeof(Tp_Init_Vk_Stg_One));
    tp_exit_on_failed_malloc(p_tp_strct_stg_one,   "Mallocing of Tp_Init_Vk_Stg_One failed!\n", -14);

    p_tp_strct_stg_one->app_nm                     = app_nm;
    p_tp_strct_stg_one->maj                        = maj;
    p_tp_strct_stg_one->min                        = min;
    p_tp_strct_stg_one->pat                        = pat;


    Tp_Vk_Inst_Fam_Plmsone* p_tp_vk_inst_fam       = tp_init_prelim_stg_one_strct_vk_inst_fam(p_tp_strct_stg_one->app_nm, p_tp_strct_stg_one->maj, p_tp_strct_stg_one->min, p_tp_strct_stg_one->pat, p_sdl_window);
    Tp_GPUs_Plmsone* p_gpus                        = tp_create_vk_phys_devs(p_tp_vk_inst_fam->p_vk_inst);
    p_tp_strct_stg_one->p_tp_vk_inst_fam           = p_tp_vk_inst_fam;
    p_tp_strct_stg_one->p_gpus                     = p_gpus;

    uint32_t p_lay_props_cntr                      = 0;
    p_tp_strct_stg_one->p_lay_props_data           = tp_init_vk_inst_lay_props(&p_lay_props_cntr);

    p_tp_strct_stg_one->is_initialized             = TP_TRUE;
    p_tp_strct_stg_one->last_stg                   = NULL;
    p_tp_strct_stg_one->p_goi                      = NULL;

    return p_tp_strct_stg_one;
}

void tp_print_vk_app_inf(Tp_Init_Vk_Stg_One* p_tp_strct_stg_one, uint8_t debug)
{
    if(debug) {
        VkApplicationInfo* p_vk_inst_app_inf = p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_app_inf;

        printf("Printing VkApplicationInfo and VkInstanceCreateInfo member values:\n==========\n");
        printf("VkApplicationInfo pointer address:\t\t\t\t%p\n", p_vk_inst_app_inf);
        printf("VkApplicationInfo->pEngineName:\t\t\t\t\t%s\n", p_vk_inst_app_inf->pEngineName);
        printf("VkApplicationInfo->pApplicationName:\t\t\t\t%s\n", p_vk_inst_app_inf->pApplicationName);
        uint32_t engine_ver = p_vk_inst_app_inf->engineVersion;
        printf("VkApplicationInfo->engineVersion:\t\t\t\t%d.%d.%d\n", VK_VERSION_MAJOR(engine_ver), VK_VERSION_MINOR(engine_ver), VK_VERSION_PATCH(engine_ver));
        uint32_t app_ver = p_vk_inst_app_inf->applicationVersion;
        printf("VkApplicationInfo->applicationVersion:\t\t\t\t%d.%d.%d\n", VK_VERSION_MAJOR(app_ver), VK_VERSION_MINOR(app_ver), VK_VERSION_PATCH(app_ver));
        printf("==========\n");
    }
}

void tp_print_inst_creat_inf(Tp_Init_Vk_Stg_One* p_tp_strct_stg_one, uint8_t debug)
{
    if(debug) {
        if (p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->sType != VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO) {
            printf("Will not be able to print VkInstanceCreateInfo member because struct not initialized!\n");
            exit(-14);
        }

        printf("Printing members of VkInstanceCreateInfo\n==========\n");
        printf("VkInstanceCreateInfo pointer address:\t\t\t\t%p\n", p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf);
        printf("VkInstanceCreateInfo->sType:\t\t\t\t\t%u\n", p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->sType);
        printf("VkInstanceCreateInfo->flags:\t\t\t\t\t%u\n", p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->flags);
        printf("VkInstanceCreateInfo->pApplicationInfo:\t\t\t\t%p\n", p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->pApplicationInfo);
        printf("VkInstanceCreateInfo->enabledLayerCount:\t\t\t%u\n", p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->enabledLayerCount);

        for (uint32_t i = 0; i < p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->enabledLayerCount; i++) {
            printf("VkInstanceCreateInfo->ppEnabledLayerNames #%u:\t\t\t%s\n", i, p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->ppEnabledLayerNames[i]);
        }

        printf("VkInstanceCreateInfo->enabledExtensionCount:\t\t\t%u\n", p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->enabledExtensionCount);
        for (uint32_t i = 0; i < p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->enabledExtensionCount; i++) {
            printf("VkInstanceCreateInfo->ppEnabledExtensionNames #%u:\t\t%s\n", i, p_tp_strct_stg_one->p_tp_vk_inst_fam->p_vk_inst_create_inf->ppEnabledExtensionNames[i]);
        }
        printf("==========\n");
    }
}

VkPhysicalDeviceProperties* tp_create_vk_phys_dev_props(VkPhysicalDevice* p_vk_phys_dev)
{
    VkPhysicalDeviceProperties* p_gpu_props = malloc(sizeof(VkPhysicalDeviceProperties));
    tp_exit_on_failed_malloc(p_gpu_props, "Mallocing of VkPhysicalDeviceProperties* array getting GPU properties!\n", -15);
    vkGetPhysicalDeviceProperties(*p_vk_phys_dev, p_gpu_props);

    return p_gpu_props;
}

void tp_print_vk_phys_dev_props(VkPhysicalDeviceProperties* p_gpu_props, uint8_t debug)
{
    if(debug) {
        printf("==========\nPrinting physical device properties:\n\n");
        printf("VkPhysicalDeviceProperties pointer address:\t\t\t%p\n", p_gpu_props);
        printf("Name of GPU:\t\t\t\t\t\t\t%s\n", p_gpu_props->deviceName);
        uint32_t apiVer = p_gpu_props->apiVersion;
        printf("Vulkan API version:\t\t\t\t\t\t%d.%d.%d\n", VK_VERSION_MAJOR(apiVer), VK_VERSION_MINOR(apiVer), VK_VERSION_PATCH(apiVer));
        printf("Driver version:\t\t\t\t\t\t\t%d\n", p_gpu_props->driverVersion);
        printf("Vendor ID:\t\t\t\t\t\t\t%d\n", p_gpu_props->vendorID);
        printf("Device ID:\t\t\t\t\t\t\t%d\n", p_gpu_props->deviceID);
        printf("Device type:\t\t\t\t\t\t\t%d\n", p_gpu_props->deviceType);
        printf("Explanation device type:\t\t\t\t\t 0 - unidentified\n\t\t\t\t\t\t\t\t 1 - integrated GPU\n\t\t\t\t\t\t\t\t 2 - discrete GPU\n\t\t\t\t\t\t\t\t 3 - virtual GPU\n\t\t\t\t\t\t\t\t 4 - APU (GPU inside the CPU)\n");
        printf("Pipeline cache UUID:\t\t\t\t\t\t%hhu\n", *(p_gpu_props->pipelineCacheUUID));
        printf("Discrete queue priorities:\t\t\t\t\t %d\n", p_gpu_props->limits.discreteQueuePriorities);
        printf("Sparse property residencyStandard2DBlockShape:\t\t\t%d\n", p_gpu_props->sparseProperties.residencyStandard2DBlockShape);
        printf("Sparse property residencyStandard2DMultisampleBlockShape:\t%d\n", p_gpu_props->sparseProperties.residencyStandard2DMultisampleBlockShape);
        printf("Sparse property residencyStandard3DBlockShape:\t\t\t%d\n", p_gpu_props->sparseProperties.residencyStandard3DBlockShape);
        printf("Sparse property residencyAlignedMipSize:\t\t\t%d\n", p_gpu_props->sparseProperties.residencyAlignedMipSize);
        printf("Sparse property residencyNonResidentStrict:\t\t\t%d\n", p_gpu_props->sparseProperties.residencyNonResidentStrict);
        printf("==========\n");
    }
}

VkPhysicalDeviceFeatures* tp_create_phys_dev_feats(VkPhysicalDevice* p_vk_phys_dev)
{
    VkPhysicalDeviceFeatures* p_gpu_feats = malloc(sizeof(VkPhysicalDeviceFeatures));
    tp_exit_on_failed_malloc(p_gpu_feats, "Mallocing of VkPhysicalDeviceFeatures* array getting GPU features!\n", -15);
    vkGetPhysicalDeviceFeatures(*p_vk_phys_dev, p_gpu_feats);

    return p_gpu_feats;
}

void tp_print_vk_phys_dev_feats(VkPhysicalDeviceFeatures* p_gpu_feats, uint8_t debug)
{
    if(debug > 0) {
        printf("==========\nPrinting physical device memory features:\n\n");
        printf("print_phys_dev_feats() not implemented, yet!\n");
        printf("==========\n");
    }
}

VkPhysicalDeviceMemoryProperties* tp_create_phys_dev_mem_props(VkPhysicalDevice* p_vk_phys_dev)
{
    VkPhysicalDeviceMemoryProperties* p_gpu_mem_props = malloc(sizeof(VkPhysicalDeviceMemoryProperties));
    tp_exit_on_failed_malloc(p_gpu_mem_props, "Mallocing of VkPhysicalDeviceMemoryProperties array getting GPU's memory properties!\n", -16);
    vkGetPhysicalDeviceMemoryProperties(*p_vk_phys_dev, p_gpu_mem_props);

    return p_gpu_mem_props;

}

void tp_print_vk_phys_dev_mem_props(VkPhysicalDeviceMemoryProperties* p_gpu_mem_props, uint8_t debug)
{

    if(debug) {
        printf("==========\nPrinting physical device memory properties:\n\n");
        printf("VkPhysicalDeviceMemoryProperties* pointer address:\t\t%p\n", p_gpu_mem_props);
        printf("Amount of memory types:\t\t\t\t\t\t%u\n", p_gpu_mem_props->memoryTypeCount);
        printf("Amount of heap memory:\t\t\t\t\t\t%u\n", p_gpu_mem_props->memoryHeapCount);

        for(uint32_t i = 0; i < p_gpu_mem_props->memoryTypeCount; i++) {
            printf("Property flags of memory type number #%u:\t\t\t%u\n", i+1, p_gpu_mem_props->memoryTypes[i].propertyFlags);
            printf("Heap index of memory type number #%u:\t\t\t\t%u\n", i+1, p_gpu_mem_props->memoryTypes[i].heapIndex);
        }

        for(uint32_t i = 0; i < p_gpu_mem_props->memoryHeapCount; i++) {
            printf("Size of memory heap number #%u:\t\t\t\t\t%lu\n", i+1, p_gpu_mem_props->memoryHeaps[i].size);
            printf("Flags of memory heap number #%u:\t\t\t\t\t%u\n", i+1, p_gpu_mem_props->memoryHeaps[i].flags);
        }
        printf("==========\n");
    }
}

Tp_GPU_Props_Feats_Plmstwo* tp_init_gpu_props_feats(VkPhysicalDevice* p_goi)
{
    Tp_GPU_Props_Feats_Plmstwo* p_tp_gpu_props_feats = malloc(sizeof(Tp_GPU_Props_Feats_Plmstwo));
    tp_exit_on_failed_malloc(p_tp_gpu_props_feats,   "Mallocing of Tp_GPU_Props_Feats_Plmstwo failed!\n", -14);

    VkPhysicalDeviceProperties* p_gpu_props_goi           = tp_create_vk_phys_dev_props(p_goi);
    VkPhysicalDeviceFeatures* p_gpu_feats_goi             = tp_create_phys_dev_feats(p_goi);
    VkPhysicalDeviceMemoryProperties* p_gpu_mem_props_goi = tp_create_phys_dev_mem_props(p_goi);

    p_tp_gpu_props_feats->gpu_props                       = p_gpu_props_goi;
    p_tp_gpu_props_feats->gpu_feats                       = p_gpu_feats_goi;
    p_tp_gpu_props_feats->gpu_mem_props                   = p_gpu_mem_props_goi;

   return p_tp_gpu_props_feats;
}

Tp_Init_Vk_Stg_Two* tp_super_init_vulkan_stg_two(Tp_Init_Vk_Stg_One* p_tp_init_vk_stg_one, VkPhysicalDevice* p_goi)
{
    printf("\n\nInitializing Stage 2 ...\n\n");

    Tp_Init_Vk_Stg_Two* p_tp_gpu_inf                      = malloc(sizeof(Tp_Init_Vk_Stg_Two));
    tp_exit_on_failed_malloc(p_tp_gpu_inf,                "Mallocing of Tp_Init_Vk_Stg_Two failed!\n", -14);

    Tp_GPU_Props_Feats_Plmstwo* p_tp_gpu_props_feats      = tp_init_gpu_props_feats(p_goi);

    p_tp_gpu_inf->basic_gpu_inf                           = p_tp_init_vk_stg_one->p_gpus;
    p_tp_gpu_inf->adv_gpu_inf                             = p_tp_gpu_props_feats;

    if(p_tp_init_vk_stg_one->is_initialized) {
        p_tp_gpu_inf->is_initialized                      = TP_TRUE;
        p_tp_gpu_inf->last_stg                            = p_tp_init_vk_stg_one;
    } else {
        printf("Stage two was not completely initialized because stage one isn't, yet!\n");
        exit(-14);
    }

    printf("\n\nVkPhysicalDeviceProperties, VkPhysicalDeviceFeatures, and VkPhysicalDeviceMemoryProperties initialized. Stage 2 milestone complete!\n\n");

    return p_tp_gpu_inf;
}

void tp_print_super_init_stg_two(Tp_Init_Vk_Stg_Two* p_gpu_inf, const char* what, uint8_t debug)
{
    if(debug) {
        if(tp_str_cmp(what, "all")) {
            tp_print_vk_phys_dev_props(p_gpu_inf->adv_gpu_inf->gpu_props, debug);
            tp_print_vk_phys_dev_feats(p_gpu_inf->adv_gpu_inf->gpu_feats, debug);
            tp_print_vk_phys_dev_mem_props(p_gpu_inf->adv_gpu_inf->gpu_mem_props, debug);
        } else if(tp_str_cmp(what, "GPU properties")) {
            tp_print_vk_phys_dev_props(p_gpu_inf->adv_gpu_inf->gpu_props, debug);
        } else if(tp_str_cmp(what, "GPU features")) {
            tp_print_vk_phys_dev_feats(p_gpu_inf->adv_gpu_inf->gpu_feats, debug);
        } else if(tp_str_cmp(what, "GPU memory properties")) {
            tp_print_vk_phys_dev_mem_props(p_gpu_inf->adv_gpu_inf->gpu_mem_props, debug);
        }
    }
}

VkQueueFamilyProperties* tp_create_q_fam_props_for_gpu(VkPhysicalDevice* p_goi)
{
    uint32_t q_fam_props_cntr = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*p_goi, &q_fam_props_cntr, NULL);

    if(q_fam_props_cntr > 0) {
        VkQueueFamilyProperties* p_gpu_q_fam_props_arr = malloc(q_fam_props_cntr * sizeof(VkQueueFamilyProperties));
        tp_exit_on_failed_malloc(p_gpu_q_fam_props_arr, "Mallocing of VkQueueFamilyProperties array to store GPU'q Queue family properties failed!\n", -17);

        printf("GPU has queue family properties: %u\n", q_fam_props_cntr);
        vkGetPhysicalDeviceQueueFamilyProperties(*p_goi, &q_fam_props_cntr, p_gpu_q_fam_props_arr);

        return p_gpu_q_fam_props_arr;
    }

    printf("Failed to detect VkQueueFamilyProperties!\n");
    exit(-18);
}

void tp_print_q_fam_props(VkQueueFamilyProperties* p_vk_q_fam_props, uint8_t debug)
{
    if(debug) {
        printf("==========\nPrinting queue family properties:\n\n");
        printf("VkQueueFamilyProperties* has address:\t\t\t\t%p\n", p_vk_q_fam_props);
        printf("Queue flags of queue family (Graphics bit):\t\t\t%u\n", (p_vk_q_fam_props->queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0);
        printf("Queue flags of queue family (Compute bit):\t\t\t%u\n", (p_vk_q_fam_props->queueFlags & VK_QUEUE_COMPUTE_BIT) != 0);
        printf("Queue flags of queue family (Transfer bit):\t\t\t%u\n", (p_vk_q_fam_props->queueFlags & VK_QUEUE_TRANSFER_BIT) != 0);
        printf("Queue flags of queue family (Sparse binding bit):\t\t%u\n", (p_vk_q_fam_props->queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0);
        printf("Queue count: %u\n", p_vk_q_fam_props->queueCount);
        printf("Timestamp valid bits: %u\n", p_vk_q_fam_props->timestampValidBits);
        printf("Min Image Timestamp Granularity width:\t\t\t\t%u\n", p_vk_q_fam_props->minImageTransferGranularity.width);
        printf("Min Image Timestamp Granularity height:\t\t\t\t%u\n", p_vk_q_fam_props->minImageTransferGranularity.height);
        printf("Min Image Timestamp Granularity depth:\t\t\t\t%u\n", p_vk_q_fam_props->minImageTransferGranularity.depth);
        printf("==========\n");
    }
}

VkDeviceQueueCreateInfo* tp_create_dev_q_creat_inf()
{
    VkDeviceQueueCreateInfo* p_vk_dev_q_create_info = malloc(sizeof(VkDeviceQueueCreateInfo));
    tp_exit_on_failed_malloc(p_vk_dev_q_create_info, "Mallocing of VkDeviceQueueCreateInfo failed!\n", -18);

    p_vk_dev_q_create_info->sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    p_vk_dev_q_create_info->pNext            = NULL;
    p_vk_dev_q_create_info->flags            = 0;
    p_vk_dev_q_create_info->queueFamilyIndex = 0;
    p_vk_dev_q_create_info->queueCount       = 1;

    float* q_prios = malloc(p_vk_dev_q_create_info->queueCount * sizeof(float));
    tp_exit_on_failed_malloc(p_vk_dev_q_create_info, "Mallocing of float array to store Queue priorities failed!\n", -19);

    printf("Queuecount: %u\n\n\n", p_vk_dev_q_create_info->queueCount);

    for(uint8_t i = 0; i < p_vk_dev_q_create_info->queueCount; i++) {
        q_prios[i] = 1.0f;
    }

    p_vk_dev_q_create_info->pQueuePriorities = (const float*)q_prios;

    return p_vk_dev_q_create_info;
}


void tp_print_dev_q_creat_inf(VkDeviceQueueCreateInfo* p_gpu_q_creat_inf, uint8_t debug)
{
    if(debug) {
        printf("==========\nPrinting VkDeviceQueueCreateInfo* :\n\n");
        printf("VkDeviceQueueCreateInfo* pointer has address:\t\t%p\n", p_gpu_q_creat_inf);
        printf("VkDeviceQueueCreateInfo->pNext:\t\t\t\t%p\n", p_gpu_q_creat_inf->pNext);
        printf("VkDeviceQueueCreateInfo->flags:\t\t\t\t%u\n", p_gpu_q_creat_inf->flags);
        printf("VkDeviceQueueCreateInfo->queueFamilyIndex:\t\t\t\t%u\n", p_gpu_q_creat_inf->queueFamilyIndex);
        printf("VkDeviceQueueCreateInfo->queueCount:\t\t\t\t%u\n", p_gpu_q_creat_inf->queueCount);

        for(uint8_t i = 0; i < p_gpu_q_creat_inf->queueCount; i++) {
            printf("Queue priority of Queue #%u:\t\t\t\t%f\n", i, (p_gpu_q_creat_inf->pQueuePriorities)[i]);
        }
        printf("==========\n");
    }
}

VkDeviceCreateInfo* tp_create_dev_creat_info(VkDeviceQueueCreateInfo* p_gpu_q_creat_inf, char** pp_en_lay_nms, const uint32_t en_lay_cnt, VkPhysicalDeviceFeatures* p_vk_gpu_feats, char** pp_en_ext_nms, const uint32_t en_ext_cnt)
{
    VkDeviceCreateInfo* p_vk_dev_creat_inf = malloc(sizeof(VkDeviceCreateInfo));
    tp_exit_on_failed_malloc(p_vk_dev_creat_inf, "Mallocing of VkDeviceCreateInfo failed\n", -20);

    p_vk_dev_creat_inf->sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    p_vk_dev_creat_inf->pNext                   = NULL;
    p_vk_dev_creat_inf->flags                   = 0;
    p_vk_dev_creat_inf->queueCreateInfoCount    = 1;
    p_vk_dev_creat_inf->pQueueCreateInfos       = p_gpu_q_creat_inf;
    p_vk_dev_creat_inf->enabledLayerCount       = en_lay_cnt;
    p_vk_dev_creat_inf->ppEnabledLayerNames     = (const char* const*)pp_en_lay_nms;
    p_vk_dev_creat_inf->enabledExtensionCount   = en_ext_cnt;
    p_vk_dev_creat_inf->ppEnabledExtensionNames = (const char* const*)pp_en_ext_nms;
    p_vk_dev_creat_inf->pEnabledFeatures        = p_vk_gpu_feats;

   return p_vk_dev_creat_inf;
}

void tp_print_create_dev_creat_info(VkDeviceCreateInfo* p_vk_dev_creat_inf, uint8_t debug)
{
    if(debug) {
        printf("Printing VkDeviceCreateInfo member values:\n==============\n");
        printf("VkDeviceCreateInfo pointer has address:\t\t\t\t%p\n", p_vk_dev_creat_inf);
        printf("VkDeviceCreateInfo->sType:\t\t\t\t\t%u\n", p_vk_dev_creat_inf->sType);
        printf("VkDeviceCreateInfo->pNext:\t\t\t\t\t%p\n", p_vk_dev_creat_inf->pNext);
        printf("VkDeviceCreateInfo->flags:\t\t\t\t\t%u\n", p_vk_dev_creat_inf->flags);
        printf("VkDeviceCreateInfo->queueCreateInfoCount:\t\t\t%u\n", p_vk_dev_creat_inf->queueCreateInfoCount);
        printf("VkDeviceCreateInfo->pQueueCreateInfos:\t\t\t\t%p\n", p_vk_dev_creat_inf->pQueueCreateInfos);
        printf("VkDeviceCreateInfo->enabledLayerCount:\t\t\t\t%u\n", p_vk_dev_creat_inf->enabledLayerCount);
        printf("VkDeviceCreateInfo->ppEnabledLayerNames:\t\t\t%p\n", p_vk_dev_creat_inf->ppEnabledLayerNames);
        printf("VkDeviceCreateInfo->enabledExtensionCount:\t\t\t%u\n", p_vk_dev_creat_inf->enabledExtensionCount);
        printf("VkDeviceCreateInfo->ppEnabledExtensionNames:\t\t\t%p\n", p_vk_dev_creat_inf->ppEnabledExtensionNames);
        printf("VkDeviceCreateInfo->pEnabledFeatures:\t\t\t\t%p\n", p_vk_dev_creat_inf->pEnabledFeatures);
        printf("==============\n");
   }
}

Tp_Init_Vk_Stg_Three* tp_super_init_vulkan_stg_three(Tp_Init_Vk_Stg_Two* p_tp_init_vk_stg_two, char** pp_en_lay_nms, const uint32_t en_lay_cnt, VkPhysicalDeviceFeatures* p_vk_gpu_feats, char** pp_en_ext_nms, const uint32_t en_ext_cnt)
{
    printf("\n\nInitializing Stage 3 ...\n\n");

    Tp_Init_Vk_Stg_Three* p_tp_stg_three_strct    = malloc(sizeof(Tp_Init_Vk_Stg_Three));
    tp_exit_on_failed_malloc(p_tp_stg_three_strct,"Mallocing of Tp_Init_Vk_Stg_Three failed\n", -14);

    if(p_tp_init_vk_stg_two->is_initialized) {
        p_tp_stg_three_strct->last_stg            = p_tp_init_vk_stg_two;
        p_tp_stg_three_strct->is_initialized      = TP_TRUE;
        Tp_Init_Vk_Stg_One* p_tp_stg_one_strct    = p_tp_stg_three_strct->last_stg->last_stg;

        if(p_tp_stg_one_strct->p_goi == NULL) {
            printf("You forgot to assign your GPU of interest to Tp_Init_Vk_Stg_One->p_goi in stage 2. Exiting...\n");
            exit(-14);
        }

    } else {
        printf("Stage three was not completely initialized because stage two isn't, yet!\n");
        exit(-14);
    }

    VkPhysicalDevice* p_goi                       = p_tp_init_vk_stg_two->last_stg->p_goi;
    VkQueueFamilyProperties* p_vk_q_fam_props     = tp_create_q_fam_props_for_gpu(p_goi);
    VkDeviceQueueCreateInfo* p_vk_dev_q_creat_inf = tp_create_dev_q_creat_inf();
    VkDeviceCreateInfo* p_vk_dev_creat_inf        = tp_create_dev_creat_info(p_vk_dev_q_creat_inf, pp_en_lay_nms, en_lay_cnt, p_vk_gpu_feats, pp_en_ext_nms, en_ext_cnt);

    p_tp_stg_three_strct->p_vk_q_fam_props        = p_vk_q_fam_props;
    p_tp_stg_three_strct->p_vk_dev_q_creat_inf    = p_vk_dev_q_creat_inf;
    p_tp_stg_three_strct->p_vk_dev_creat_inf      = p_vk_dev_creat_inf;

    printf("\n\nVkQueue and VkExtensionProperties initialized. Stage 3 milestone complete!\n\n");

    return p_tp_stg_three_strct;
}

VkDevice* tp_create_logical_gpu(Tp_Init_Vk_Stg_Three* p_tp_stg_three_strct)
{
    Tp_Init_Vk_Stg_One* p_tp_stg_one_strct        = p_tp_stg_three_strct->last_stg->last_stg;
    const VkDeviceCreateInfo* p_vk_dev_creat_inf  = p_tp_stg_three_strct->p_vk_dev_creat_inf;
    VkPhysicalDevice    gpu_of_choice             = *(p_tp_stg_one_strct->p_goi);

    VkDevice* p_vk_dev = malloc(sizeof(VkDevice));
    tp_exit_on_failed_malloc(p_vk_dev, "Mallocing of VkDevice failed\n", -21);

    /* Third argument NULL means Vulkan will use its own memory allocator */
    /* TODO: Pick best device instead of first device */
    VkResult res = vkCreateDevice(gpu_of_choice, p_vk_dev_creat_inf, NULL, p_vk_dev);
    tp_exit_on_bad_vk_res(&res, "Creating of VkDevice (logical GPU representation) failed!\n", -22);

    return p_vk_dev;
}

Tp_Init_Vk_Stg_Four* tp_super_init_vulkan_stg_four(Tp_Init_Vk_Stg_Three* p_tp_stg_three_strct)
{
    printf("\n\nInitializing Stage 4 ...\n\n");

    Tp_Init_Vk_Stg_Four* p_tp_stg_four_strct = malloc(sizeof(Tp_Init_Vk_Stg_Four));
    p_tp_stg_four_strct->p_vk_dev = tp_create_logical_gpu(p_tp_stg_three_strct);

    if(p_tp_stg_three_strct->is_initialized) {
        p_tp_stg_four_strct->is_initialized = TP_TRUE;
        p_tp_stg_four_strct->last_stg = p_tp_stg_three_strct;
    } else {
        printf("Stage four was not completely initialized because stage three isn't, yet!\n");
        exit(-14);
    }

    printf("\n\nVkInstance initialized!!! Stage 4 milestone complete!\n\n");

    return p_tp_stg_four_strct;
}

VkQueue* tp_create_q(Tp_Init_Vk_Stg_Four* p_tp_stg_four_strct)
{
    VkQueue* p_vk_q = malloc(sizeof(VkQueue));
    tp_exit_on_failed_malloc(p_vk_q, "Mallocing of VkQueue failed!\n", -25);
    vkGetDeviceQueue(*(p_tp_stg_four_strct->p_vk_dev), 0, 0, p_vk_q);

    return p_vk_q;
}

Tp_Q_Plmsfive* tp_init_vk_q(Tp_Init_Vk_Stg_Four* p_tp_stg_four_strct)
{
     Tp_Q_Plmsfive* p_prelim_strct_stg_five_vk_q_data = malloc(sizeof(Tp_Q_Plmsfive));
     tp_exit_on_failed_malloc(p_prelim_strct_stg_five_vk_q_data, "Mallocing of Tp_Q_Plmsfive failed!\n", -26);

     p_prelim_strct_stg_five_vk_q_data->p_vk_q = tp_create_q(p_tp_stg_four_strct);

     return p_prelim_strct_stg_five_vk_q_data;
}


VkExtensionProperties* tp_create_vk_ext_props(const char* lay_nm, uint32_t* p_ext_props_cntr)
{
    /**
    *  In contrast to vkEnumerateInstanceLayerProperties() vkEnumerateInstanceExtensionProperties()
    *  needs three arguments because Extensions can belong to layers.
    *  If first argument is NULL we say: This extension does not belong to a specific layer
    **/
    vkEnumerateInstanceExtensionProperties(lay_nm, p_ext_props_cntr, NULL);
    VkExtensionProperties* vk_ext_props_arr = malloc(*p_ext_props_cntr * sizeof(VkExtensionProperties));
    tp_exit_on_failed_malloc(vk_ext_props_arr, "Mallocing of VkExtensionProperties array failed!\n", -26);

    vkEnumerateInstanceExtensionProperties(lay_nm, p_ext_props_cntr, vk_ext_props_arr);

    return vk_ext_props_arr;
}

Tp_Ext_Props_Plmsfive* tp_init_ext_props(const char* lay_nm)
{
    uint32_t ext_props_cntr = 0;

    Tp_Ext_Props_Plmsfive* p_prelim_strct_stg_five_ext_props_data = malloc(sizeof(Tp_Ext_Props_Plmsfive));
    tp_exit_on_failed_malloc(p_prelim_strct_stg_five_ext_props_data, "Mallocing of Tp_Ext_Props_Plmsfive failed!\n", -27);

    p_prelim_strct_stg_five_ext_props_data->p_vk_ext_props = tp_create_vk_ext_props(lay_nm, &ext_props_cntr);
    p_prelim_strct_stg_five_ext_props_data->lay_nm = lay_nm;
    p_prelim_strct_stg_five_ext_props_data->num_ext_props = ext_props_cntr;

    return p_prelim_strct_stg_five_ext_props_data;
}

Tp_Init_Vk_Stg_Five* tp_super_init_vulkan_stg_five(Tp_Init_Vk_Stg_Four* p_tp_stg_four_strct, const char* lay_nm)
{
    printf("\n\nInitializing Stage 5 ...\n\n");

    Tp_Init_Vk_Stg_Five* p_tp_stg_five         = malloc(sizeof(Tp_Init_Vk_Stg_Five));
    tp_exit_on_failed_malloc(p_tp_stg_five,    "Mallocing of Tp_Init_Vk_Stg_Five failed!\n", -99);

    if(p_tp_stg_four_strct->is_initialized) {
        p_tp_stg_five->is_initialized = TP_TRUE;
        p_tp_stg_five->last_stg       = p_tp_stg_four_strct;
    } else {
        printf("Stage five was not completely initialized because stage four isn't, yet!\n");
        exit(-14);
    }

    printf("\n\nVkSurfaceKHR, VkSurfaceCapabilitiesKHR, VkSurfaceFormatKHR array, VkPresentModeKHR array and GLFWwindow initialized!!! Stage 5 milestone complete!\n\n");

    p_tp_stg_five->p_vk_q_data         = tp_init_vk_q(p_tp_stg_four_strct);
    p_tp_stg_five->p_vk_ext_props_data = tp_init_ext_props(lay_nm);

    return p_tp_stg_five;
}

void tp_print_vk_ext_props(VkExtensionProperties* vk_ext_props_arr, uint32_t ext_props_cntr)
{
    for(uint32_t i = 0; i < ext_props_cntr; i++) {
        printf("==========\nPrinting extension properties of extension #%d:\n\n", i);
        printf("Extension name:\t\t\t\t\t\t\t%s\n", vk_ext_props_arr[i].extensionName);
        printf("Extension spec version:\t\t\t\t\t\t%u\n", vk_ext_props_arr[i].specVersion);
        printf("==========\n");
    }
}

void tp_print_super_init_stg_five(VkExtensionProperties* vk_ext_props_arr, uint32_t ext_props_cntr)
{
    tp_print_vk_ext_props(vk_ext_props_arr, ext_props_cntr);
}

char** tp_get_avail_lay_nms(Tp_Init_Vk_Stg_One* tp_init_vk_stg_one_data)
{
    uint32_t detectd_lay_props_cntr = tp_init_vk_stg_one_data->p_lay_props_data->num_lay_props;
    char** lay_nms = malloc(detectd_lay_props_cntr * sizeof(char*));
    tp_exit_on_failed_malloc(lay_nms, "Mallocing of string array to store detected layer names failed!\n", -28);

    for(uint32_t i = 0; i < detectd_lay_props_cntr; i++) {
        lay_nms[i] = malloc(tp_str_len((tp_init_vk_stg_one_data->p_lay_props_data->p_vk_lay_props)[i].layerName) * sizeof(char));
        tp_exit_on_failed_malloc(lay_nms[i], "Mallocing of string for array to store detected layer names failed!\n", -28);
        lay_nms[i] = (tp_init_vk_stg_one_data->p_lay_props_data->p_vk_lay_props)[i].layerName;
    }

    return lay_nms;
}


char** tp_get_avail_ext_nms(Tp_Init_Vk_Stg_Five* tp_init_vk_stg_five_data)
{
    uint32_t detectd_ext_props_cntr = tp_init_vk_stg_five_data->p_vk_ext_props_data->num_ext_props;
    char** ext_nms = malloc(detectd_ext_props_cntr * sizeof(char*));
    tp_exit_on_failed_malloc(ext_nms, "Mallocing of string array to store detected extension names failed!\n", -29);

    for(uint32_t i = 0; i < detectd_ext_props_cntr; i++) {
        ext_nms[i] = malloc(tp_str_len((tp_init_vk_stg_five_data->p_vk_ext_props_data->p_vk_ext_props)[i].extensionName)  * sizeof(char));
        tp_exit_on_failed_malloc(ext_nms[i], "Mallocing of string for array to store detected extension names failed!\n", -30);
        ext_nms[i] = (tp_init_vk_stg_five_data->p_vk_ext_props_data->p_vk_ext_props)[i].extensionName;
    }

    return ext_nms;
}

GLFWwindow* tp_start_glfw(GLFWwindow* p_glfw_win, const uint32_t width, const uint32_t height, const char* title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Third argument is on what monitor to open window */
    /* Last argument we will never need because that's only for OpenGL */

    p_glfw_win = glfwCreateWindow(width, height, title, NULL, NULL);

    return p_glfw_win;
}

VkSurfaceKHR* tp_glfw_create_win_surface_wrapper(Tp_Init_Vk_Stg_One* tp_init_vk_stg_one_data, SDL_Window* p_sdl_window)
{
    /* We use GLFW here because it let's us create */
    /* a window surface platform independently */

    VkSurfaceKHR* p_vk_sur_khr = malloc(sizeof(VkSurfaceKHR));
    tp_exit_on_failed_malloc(p_vk_sur_khr, "Mallocing of VkSurfaceKHR* failed!\n", -31);

    //VkResult res = glfwCreateWindowSurface(*(tp_init_vk_stg_one_data->p_tp_vk_inst_fam->p_vk_inst), glfw_win, NULL, p_vk_sur_khr);
    //tp_exit_on_bad_vk_res(&res, "Creating window surface (via GLFW) failed!\n", -32);
    
    SDL_Vulkan_CreateSurface(p_sdl_window, *(tp_init_vk_stg_one_data->p_tp_vk_inst_fam->p_vk_inst), p_vk_sur_khr);

    return p_vk_sur_khr;
}

VkSurfaceCapabilitiesKHR* tp_create_vk_sur_caps_khr(VkPhysicalDevice* p_goi, VkSurfaceKHR* p_vk_sur_khr)
{
    VkSurfaceCapabilitiesKHR* p_vk_sur_caps_khr = malloc(sizeof(VkSurfaceCapabilitiesKHR));
    tp_exit_on_failed_malloc(p_vk_sur_caps_khr, "Mallocing of VkSurfaceCapabilitiesKHR failed!\n", -33);
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*p_goi, *p_vk_sur_khr, p_vk_sur_caps_khr);

    return p_vk_sur_caps_khr;
}

void tp_print_vk_surf_caps_khr(VkSurfaceCapabilitiesKHR* p_vk_sur_caps)
{
    printf("==========\nPrinting Vulkan surface capabilities:\n\n");
    printf("Pointer VkSurfaceCapabilitiesKHR* has address:\t\t\t%p\n", p_vk_sur_caps);
    printf("Surface capability min image count:\t\t\t\t%u\n", p_vk_sur_caps->minImageCount);
    printf("Surface capability max image count:\t\t\t\t%u\n", p_vk_sur_caps->maxImageCount);
    printf("Surface capability current extent of width:\t\t\t%u\n", p_vk_sur_caps->currentExtent.width);
    printf("Surface capability current extent of height:\t\t\t%u\n", p_vk_sur_caps->currentExtent.height);
    printf("Surface capability min image extent of width:\t\t\t%u\n", p_vk_sur_caps->minImageExtent.width);
    printf("Surface capability min image extent of height:\t\t\t%u\n", p_vk_sur_caps->minImageExtent.height);
    printf("Surface capability max image extent of width:\t\t\t%u\n", p_vk_sur_caps->maxImageExtent.width);
    printf("Surface capability max image extent of height:\t\t\t%u\n", p_vk_sur_caps->maxImageExtent.height);
    printf("Surface capability max image array layers:\t\t\t%u\n", p_vk_sur_caps->maxImageArrayLayers);
    printf("Surface capability supported transforms:\t\t\t%u\n", p_vk_sur_caps->supportedTransforms);
    printf("Surface capability current transform:\t\t\t\t%u\n", p_vk_sur_caps->currentTransform);
    printf("Surface capability supported composite alpha:\t\t\t%u\n", p_vk_sur_caps->supportedCompositeAlpha);
    printf("Surface capability supported usage flags:\t\t\t%u\n", p_vk_sur_caps->supportedUsageFlags);
    printf("==========\n");
}

VkSurfaceFormatKHR* tp_create_vk_sur_form(VkPhysicalDevice* p_goi, VkSurfaceKHR* p_vk_sur_khr, uint32_t* p_vk_sur_forms_cntr)
{

    vkGetPhysicalDeviceSurfaceFormatsKHR(*p_goi, *p_vk_sur_khr, p_vk_sur_forms_cntr, NULL);

    VkSurfaceFormatKHR* vk_sur_form_khr_arr = malloc(*p_vk_sur_forms_cntr * sizeof(VkSurfaceFormatKHR));
    tp_exit_on_failed_malloc(vk_sur_form_khr_arr, "Mallocing of VkSurfaceFormatKHR array failed!\n", -34);

    vkGetPhysicalDeviceSurfaceFormatsKHR(*p_goi, *p_vk_sur_khr, p_vk_sur_forms_cntr, vk_sur_form_khr_arr);

    return vk_sur_form_khr_arr;
}

void tp_print_vk_sur_forms(Tp_Init_Vk_Stg_Six* p_stg_six_strct)
{
    VkSurfaceFormatKHR* vk_sur_form_khr_arr = p_stg_six_strct->p_stg_six_vk_sur_data->p_vk_sur_form_khr;
    uint32_t size_vk_sur_form_khr_arr = p_stg_six_strct->p_stg_six_vk_sur_data->size_vk_sur_form_khr_arr;

    for(uint32_t i = 0; i < size_vk_sur_form_khr_arr; i++) {
        printf("Printing supported formats #%u\n==========\n", i);

        uint32_t format_enum_val = vk_sur_form_khr_arr[i].format;

        switch(format_enum_val) {
            case 0: printf("%u, %s\n", format_enum_val, "VK_FORMAT_UNDEFINED");
            break;
            case 1: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R4G4_UNORM_PACK8");
            break;
            case 2: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R4G4B4A4_UNORM_PACK16");
            break;
            case 3: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B4G4R4A4_UNORM_PACK16");
            break;
            case 4: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R5G6B5_UNORM_PACK16");
            break;
            case 5: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B5G6R5_UNORM_PACK16");
            break;
            case 6: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R5G5B5A1_UNORM_PACK16");
            break;
            case 7: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B5G5R5A1_UNORM_PACK16");
            break;
            case 8: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A1R5G5B5_UNORM_PACK16");
            break;
            case 9: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8_UNORM");
            break;
            case 10: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8_SNORM");
            break;
            case 11: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8_USCALED");
            break;
            case 12: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8_SSCALED");
            break;
            case 13: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8_UINT");
            break;
            case 14: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8_SINT");
            break;
            case 15: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8_SRGB");
            break;
            case 16: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8_UNORM");
            break;
            case 17: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8_SNORM");
            break;
            case 18: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8_USCALED");
            break;
            case 19: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8_SSCALED");
            break;
            case 20: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8_UINT");
            break;
            case 21: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8_SINT");
            break;
            case 22: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8_SRGB");
            break;
            case 23: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8_UNORM");
            break;
            case 24: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8_SNORM");
            break;
            case 25: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8_USCALED");
            break;
            case 26: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8_SSCALED");
            break;
            case 27: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8_UINT");
            break;
            case 28: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8_SINT");
            break;
            case 29: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8_SRGB");
            break;
            case 30: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8_UNORM");
            break;
            case 31: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8_SNORM");
            break;
            case 32: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8_USCALED");
            break;
            case 33: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8_SSCALED");
            break;
            case 34: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8_UINT");
            break;
            case 35: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8_SINT");
            break;
            case 36: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8_SRGB");
            break;
            case 37: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8A8_UNORM");
            break;
            case 38: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8A8_SNORM");
            break;
            case 39: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8A8_USCALED");
            break;
            case 40: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8A8_SSCALED");
            break;
            case 41: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8A8_UINT");
            break;
            case 42: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8A8_SINT");
            break;
            case 43: printf("%u, %s\n", format_enum_val, "VK_FORMAT_R8G8B8A8_SRGB");
            break;
            case 44: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8A8_UNORM");
            break;
            case 45: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8A8_SNORM");
            break;
            case 46: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8A8_USCALED");
            break;
            case 47: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8A8_SSCALED");
            break;
            case 48: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8A8_UINT");
            break;
            case 49: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8A8_SINT");
            break;
            case 50: printf("%u, %s\n", format_enum_val, "VK_FORMAT_B8G8R8A8_SRGB");
            break;
            case 51: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A8B8G8R8_UNORM_PACK32");
            break;
            case 52: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A8B8G8R8_SNORM_PACK32");
            break;
            case 53: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A8B8G8R8_USCALED_PACK32");
            break;
            case 54: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A8B8G8R8_SSCALED_PACK32");
            break;
            case 55: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A8B8G8R8_UINT_PACK32");
            break;
            case 56: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A8B8G8R8_SINT_PACK32");
            break;
            case 57: printf("%u, %s\n", format_enum_val, "VK_FORMAT_A8B8G8R8_SRGB_PACK32");
            break;
            default: printf("Unknown format detected!\n");
            /*TODO: Finish 200 remaining cases or so :/*/
        }
        printf("==========\n");
    }
}


VkPresentModeKHR* tp_create_vk_pres_mod_khr(VkPhysicalDevice* p_goi, VkSurfaceKHR* p_vk_sur_khr, uint32_t* p_pres_modes_cntr)
{
    vkGetPhysicalDeviceSurfacePresentModesKHR(*p_goi, *p_vk_sur_khr, p_pres_modes_cntr, NULL);

    VkPresentModeKHR* vk_pres_mod_arr = malloc(*p_pres_modes_cntr * sizeof(VkPresentModeKHR));
    tp_exit_on_failed_malloc(vk_pres_mod_arr, "Mallocing of VkPresentModeKHR array failed!\n", -35);

    vkGetPhysicalDeviceSurfacePresentModesKHR(*p_goi, *p_vk_sur_khr, p_pres_modes_cntr, vk_pres_mod_arr);

    return vk_pres_mod_arr;
}

Tp_Sur_Plmssix* tp_init_sur(Tp_Init_Vk_Stg_One* p_tp_init_vk_stg_one_data, const uint32_t width, const uint32_t height, const char* title, SDL_Window* win)
{
    /* Here the GLFWwindow* win is born */

    /** TODO: Find out why I need to return GLFWwindow* from tp_start_glfw
    * so why passing pointer is not sufficient. 
    * So I mean, when just passing the pointer and returning void.
    * The window will get initalized but when passing glfw_window to the "game loop"
    * GLFW throws NULL assertion failed. It thinks glfw_window is NULL again.
    **/

    //GLFWwindow *win                               = NULL;
    //win                                           = tp_start_glfw(win, width, height, title);
    VkSurfaceKHR* p_vk_sur_khr                  = tp_glfw_create_win_surface_wrapper(p_tp_init_vk_stg_one_data, win);
    VkSurfaceCapabilitiesKHR* p_vk_sur_caps_khr = tp_create_vk_sur_caps_khr(p_tp_init_vk_stg_one_data->p_goi, p_vk_sur_khr);
    uint32_t vk_sur_forms_cntr                    = 0;
    VkSurfaceFormatKHR* p_vk_form_khr           = tp_create_vk_sur_form(p_tp_init_vk_stg_one_data->p_goi, p_vk_sur_khr, &vk_sur_forms_cntr);
    uint32_t vk_pres_mods_cntr                    = 0;
    VkPresentModeKHR* p_vk_pres_mod_khr         = tp_create_vk_pres_mod_khr(p_tp_init_vk_stg_one_data->p_goi, p_vk_sur_khr, &vk_pres_mods_cntr);

    Tp_Sur_Plmssix* p_prelim_stg_six_strct_sur_data = malloc(sizeof(p_prelim_stg_six_strct_sur_data));
    tp_exit_on_failed_malloc(p_prelim_stg_six_strct_sur_data, "Mallocing of Tp_Sur_Plmssix failed!\n", -36);

    p_prelim_stg_six_strct_sur_data->size_vk_sur_form_khr_arr = (const uint32_t)vk_sur_forms_cntr;
    p_prelim_stg_six_strct_sur_data->size_vk_pres_mod_khr_arr = (const uint32_t)vk_pres_mods_cntr;
    p_prelim_stg_six_strct_sur_data->p_vk_sur_khr           = p_vk_sur_khr;
    p_prelim_stg_six_strct_sur_data->p_vk_sur_caps_khr      = p_vk_sur_caps_khr;
    p_prelim_stg_six_strct_sur_data->p_vk_sur_form_khr      = p_vk_form_khr;
    p_prelim_stg_six_strct_sur_data->p_vk_pres_mods_khr     = p_vk_pres_mod_khr;
    p_prelim_stg_six_strct_sur_data->width                  = width;
    p_prelim_stg_six_strct_sur_data->height                 = height;
    p_prelim_stg_six_strct_sur_data->title                  = title;
    p_prelim_stg_six_strct_sur_data->p_sdl_window           = win;
    
    int* w = (int*)&width;
    int* h = (int*)&height;
    
    SDL_Vulkan_GetDrawableSize(win, w, h);

    return p_prelim_stg_six_strct_sur_data;
}

VkSwapchainCreateInfoKHR* tp_create_vk_swc_creat_inf_khr(Tp_Sur_Plmssix* tp_prelim_strct_stg_six_sur_data, const uint32_t width, const uint32_t height)
{
    VkSwapchainCreateInfoKHR* p_vk_swc_creat_inf     = malloc(sizeof(VkSwapchainCreateInfoKHR));
    tp_exit_on_failed_malloc(p_vk_swc_creat_inf, "Mallocing of VkSwapchainCreateInfoKHR!\n", -202);

    VkSurfaceKHR* p_vk_sur_khr                       = tp_prelim_strct_stg_six_sur_data->p_vk_sur_khr;
    VkSurfaceCapabilitiesKHR* p_vk_sur_caps_khrt     = tp_prelim_strct_stg_six_sur_data->p_vk_sur_caps_khr;

    p_vk_swc_creat_inf->sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    p_vk_swc_creat_inf->pNext                 = NULL;
    p_vk_swc_creat_inf->flags                 = 0;
    p_vk_swc_creat_inf->surface               = *p_vk_sur_khr;
    p_vk_swc_creat_inf->minImageCount         = p_vk_sur_caps_khrt->minImageCount;
    p_vk_swc_creat_inf->imageFormat           = VK_FORMAT_B8G8R8A8_UNORM;
    p_vk_swc_creat_inf->imageColorSpace       = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    VkExtent2D vk_ext_2d                      = { width, height };

    p_vk_swc_creat_inf->imageExtent           = vk_ext_2d;
    p_vk_swc_creat_inf->imageArrayLayers      = 1;
    p_vk_swc_creat_inf->imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    p_vk_swc_creat_inf->imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    p_vk_swc_creat_inf->queueFamilyIndexCount = 0;
    p_vk_swc_creat_inf->pQueueFamilyIndices   = NULL;
    p_vk_swc_creat_inf->preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    p_vk_swc_creat_inf->compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    p_vk_swc_creat_inf->presentMode           = VK_PRESENT_MODE_MAILBOX_KHR;
    p_vk_swc_creat_inf->clipped               = VK_TRUE;
    p_vk_swc_creat_inf->oldSwapchain          = VK_NULL_HANDLE;

    return p_vk_swc_creat_inf;
}

Tp_Swc_Creat_Inf_Plmssix* tp_init_swc_creat_inf(Tp_Sur_Plmssix* p_prelim_stg_six_strct_sur_data, const uint32_t width, const uint32_t height)
{
    Tp_Swc_Creat_Inf_Plmssix* p_prelim_stg_six_strct_swc_creat_inf_data = malloc(sizeof(Tp_Swc_Creat_Inf_Plmssix));
    tp_exit_on_failed_malloc(p_prelim_stg_six_strct_swc_creat_inf_data, "Mallocing of Tp_Swc_Creat_Inf_Plmssix failed!\n", -37);
    p_prelim_stg_six_strct_swc_creat_inf_data->p_vk_swc_creat_inf_khr = tp_create_vk_swc_creat_inf_khr(p_prelim_stg_six_strct_sur_data, width, height);

    return p_prelim_stg_six_strct_swc_creat_inf_data;
}

Tp_Init_Vk_Stg_Six* tp_super_init_vulkan_stg_six(Tp_Init_Vk_Stg_Five* p_tp_stg_five_data, uint32_t width, uint32_t height, const char* title, SDL_Window* win)
{
    Tp_Init_Vk_Stg_Six* p_tp_stg_six_data = malloc(sizeof(Tp_Init_Vk_Stg_Six));
    tp_exit_on_failed_malloc(p_tp_stg_six_data, "Mallocing of Tp_Init_Vk_Stg_Six failed!\n", -101);

    if(p_tp_stg_five_data->is_initialized) {
        p_tp_stg_six_data->is_initialized = TP_TRUE;
        p_tp_stg_six_data->last_stg       = p_tp_stg_five_data;
    } else {
        printf("Stage six was not completely initialized because stage five isn't, yet!\n");
        exit(-14);
    }

    Tp_Init_Vk_Stg_One* p_tp_stg_one_data = p_tp_stg_five_data->last_stg->last_stg->last_stg->last_stg;

    Tp_Sur_Plmssix*           p_prelim_stg_six_strct_vk_sur_khr_data    = tp_init_sur(p_tp_stg_one_data, width, height, title, win);
    Tp_Swc_Creat_Inf_Plmssix* p_prelim_stg_six_strct_swc_creat_inf_data = tp_init_swc_creat_inf(p_prelim_stg_six_strct_vk_sur_khr_data, width, height);

    p_tp_stg_six_data->p_stg_six_vk_sur_data        = p_prelim_stg_six_strct_vk_sur_khr_data;
    p_tp_stg_six_data->p_stg_six_src_creat_inf_data = p_prelim_stg_six_strct_swc_creat_inf_data;

    printf("\n\n VkSurfaceKHR, VkSurfaceCapabilitiesKHR, VkSurfaceFormatKHR array, VkPresentModeKHR array and GLFWwindow initialized!!! Stage 6 milestone complete!\n\n");

    return p_tp_stg_six_data;
}

void tp_print_vk_pres_mods(Tp_Init_Vk_Stg_Six* p_stg_six_strct)
{
    VkPresentModeKHR* vk_pres_mod_khr_arr = p_stg_six_strct->p_stg_six_vk_sur_data->p_vk_pres_mods_khr;
    uint32_t size_vk_pres_mode_arr        = p_stg_six_strct->p_stg_six_vk_sur_data->size_vk_pres_mod_khr_arr;

    for(uint32_t i = 0; i < size_vk_pres_mode_arr; i++) {
        printf("Printing supported presentation mode #%u\n==========\n", i);

        switch(vk_pres_mod_khr_arr[i]) {
            case 0: printf("%u, %s\n", vk_pres_mod_khr_arr[i], "VK_PRESENT_MODE_IMMEDIATE_KHR");
            break;
            case 1: printf("%u, %s\n", vk_pres_mod_khr_arr[i], "VK_PRESENT_MODE_MAILBOX_KHR");
            break;
            case 2: printf("%u, %s\n", vk_pres_mod_khr_arr[i], "VK_PRESENT_MODE_FIFO_KHR");
            break;
            case 3: printf("%u, %s\n", vk_pres_mod_khr_arr[i], "VK_PRESENT_MODE_FIFO_RELAXED_KHR");
            break;
            case 1000111000: printf("%u, %s\n", vk_pres_mod_khr_arr[i], "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR");
            break;
            case 1000111001: printf("%u, %s\n", vk_pres_mod_khr_arr[i], "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR");
            default: printf("Unkown presentation mode detected.\n");
        }
        printf("==========\n");
    }
}

VkBool32* check_for_sur_support(Tp_Init_Vk_Stg_Six* p_stg_six_strct)
{
    VkBool32* p_system_has_surface_support = malloc(sizeof(VkBool32));
    tp_exit_on_failed_malloc(p_system_has_surface_support, "Mallocing of Tp_Swc_Creat_Inf_Plmssix failed!\n", -303);

    VkPhysicalDevice* p_goi = p_stg_six_strct->last_stg->last_stg->last_stg->last_stg->last_stg->p_goi;
    VkSurfaceKHR* p_vk_sur  = p_stg_six_strct->p_stg_six_vk_sur_data->p_vk_sur_khr;

    VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(*p_goi, 0, *p_vk_sur, p_system_has_surface_support);
    tp_exit_on_bad_vk_res(&res, "Checking if system has VkSurface support failed!\n", -585);

    return p_system_has_surface_support;
}

void act_on_sur_support_res(Tp_Init_Vk_Stg_Six* p_stg_six_strct, VkBool32* p_system_has_surface_support)
{
    if(*p_system_has_surface_support) {
        /** char* vk_swapchaon_ext_macr = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        * printf("Char pointer vk_swapchaon_ext_macr: %p\n", vk_swapchaon_ext_macr);
        * char* device_extensions_array[] = { vk_swapchaon_ext_macr };
        * char* vk_swapchaon_ext_macr[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME; } Segfaults because:
        * It allocates the string array on the stack. Both the char pointer pointer and the char pointer
        * are on the stack. Later on in main when functions are being returned and popped from the stack
        * it appears that the char* does not live as long as the char pointer pointer (array). 
        * Allocating the array and string on the heap deals with that. Because heap is not auto managed.**/

        char** dev_exts_arr   = malloc(sizeof(char*));
        tp_exit_on_failed_malloc(dev_exts_arr, "Mallocing of extension name array to pass to VkDeviceCreateInfo failed!\n", -221);
        char* dev_swc_ext     = malloc((tp_str_len(VK_KHR_SWAPCHAIN_EXTENSION_NAME) + 1) * sizeof(char));
        tp_exit_on_failed_malloc(dev_swc_ext, "Mallocing of tension name 'VK_KHR_swapchain' (string) to pass to extension name array (VkDeviceCreateInfo level) failed!\n", -222);
        dev_swc_ext           = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        dev_exts_arr[0]       = dev_swc_ext;

        VkDeviceCreateInfo* p_dev_creat_inf = p_stg_six_strct->last_stg->last_stg->last_stg->p_vk_dev_creat_inf;
        //VkDevice*           p_vk_dev        = p_stg_six_strct->last_stg->last_stg->p_vk_dev;
        /* Free old logical device before reinitializing with new extension(s) */
        //free(p_vk_dev);

        printf("Your GPU has VKSurfaceKHR support. Adding surface 'VK_KHR_swapchain' extension on device level.\n");
        printf("Setting up new VkDeviceCreateInfo. Reinitializing logical representation of GPU...\n");
        p_dev_creat_inf->ppEnabledExtensionNames = (const char* const*)dev_exts_arr;

        p_dev_creat_inf->enabledExtensionCount = 1;
        p_stg_six_strct->last_stg->last_stg->p_vk_dev = NULL;
        p_stg_six_strct->last_stg->last_stg->p_vk_dev = tp_create_logical_gpu(p_stg_six_strct->last_stg->last_stg->last_stg);
        free(dev_exts_arr);
    } else {
        printf("Your GPU does NOT HAVE VKSurfaceKHR support!. \n");
        printf("Don't know how to handle that, yet. I'm out. Good Luck!. \n");
        exit(10);
    }
}

VkSwapchainKHR* tp_create_vk_swc_khr(Tp_Init_Vk_Stg_Six* p_stg_six_strct)
{
    VkSwapchainKHR* p_vk_swc_khr = malloc(sizeof(VkSwapchainKHR));
    tp_exit_on_failed_malloc(p_vk_swc_khr, "Mallocing of VkSwapchainKHR failed!\n", -70);

    VkDevice* p_vk_dev                               = p_stg_six_strct->last_stg->last_stg->p_vk_dev;
    VkSwapchainCreateInfoKHR* p_vk_swc_creat_inf_khr = p_stg_six_strct->p_stg_six_src_creat_inf_data->p_vk_swc_creat_inf_khr;

    VkResult res = vkCreateSwapchainKHR(*p_vk_dev, p_vk_swc_creat_inf_khr, NULL, p_vk_swc_khr);
    tp_exit_on_bad_vk_res(&res, "Creating Swapchain failed!\n", -585);

    return p_vk_swc_khr;
}

void tp_destroy_vk_swc_khr(Tp_Init_Vk_Stg_Seven* p_stg_seven_strct)
{
    VkDevice*       p_vk_dev  = p_stg_seven_strct->last_stg->last_stg->last_stg->p_vk_dev;
    VkSwapchainKHR* p_vk_swc_khr = p_stg_seven_strct->p_vk_swc_khr;

    vkDestroySwapchainKHR(*p_vk_dev, *p_vk_swc_khr, NULL);
}

VkImage* tp_create_vk_img_arr(VkSwapchainKHR* p_vk_swc_khr, Tp_Init_Vk_Stg_Six* p_stg_six_strct, uint32_t* p_img_cntr)
{
    VkDevice*       p_vk_dev  = p_stg_six_strct->last_stg->last_stg->p_vk_dev;

    vkGetSwapchainImagesKHR(*p_vk_dev, *p_vk_swc_khr, p_img_cntr, NULL);

    if(*p_img_cntr > 0) {
        VkImage* p_vk_img_arr = malloc(*p_img_cntr * sizeof(VkImage));
        tp_exit_on_failed_malloc(p_vk_img_arr, "Mallocing of VkImage array failed!\n", -71);
        VkResult res = vkGetSwapchainImagesKHR(*p_vk_dev, *p_vk_swc_khr, p_img_cntr, p_vk_img_arr);
        tp_exit_on_bad_vk_res(&res, "Creating VkImage array failed!\n", -586);

        return p_vk_img_arr;
    }
    return NULL;
}

VkImageViewCreateInfo* tp_create_vk_ing_vu_creat_inf(VkImage* p_vk_img_arr)
{
    VkImageViewCreateInfo* p_img_vu_creat_inf = malloc(sizeof(VkImageCreateInfo));
    tp_exit_on_failed_malloc(p_img_vu_creat_inf, "Mallocing of VkImageViewCreateInfo failed!\n", -72);

    p_img_vu_creat_inf->sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    p_img_vu_creat_inf->pNext                           = NULL;
    p_img_vu_creat_inf->flags                           = 0;
    p_img_vu_creat_inf->image                           = p_vk_img_arr[0]; /*TODO: Make dynamic */
    p_img_vu_creat_inf->viewType                        = VK_IMAGE_VIEW_TYPE_2D; /*enum VkImageViewType: 1 = VK_IMAGE_VIEW_TYPE_2D*/;
    p_img_vu_creat_inf->format                          = VK_FORMAT_B8G8R8A8_UNORM; /* TODO: Check if valid */
    p_img_vu_creat_inf->components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY; /* struct VkComponentMapping */
    p_img_vu_creat_inf->components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY; /* struct VkComponentMapping */
    p_img_vu_creat_inf->components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY; /* struct VkComponentMapping */
    p_img_vu_creat_inf->components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY; /* struct VkComponentMapping. They are VkSwizzleComponents. Swizzling means to switch up */
    p_img_vu_creat_inf->subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT; /*struct VkImageSubresourceRange*/
    p_img_vu_creat_inf->subresourceRange.baseMipLevel   = 0; /*struct VkImageSubresourceRange Mip Levels help decide what Pixels to draw for far away textures in a 3D world. They are small representations of a texture that from a certain distance calculate their averagely most abundant pixels */
    p_img_vu_creat_inf->subresourceRange.levelCount     = 1; /*struct VkImageSubresourceRange*/
    p_img_vu_creat_inf->subresourceRange.baseArrayLayer = 0; /*struct VkImageSubresourceRange for steoreographic textures, so 3D VR-like stuff*/
    p_img_vu_creat_inf->subresourceRange.layerCount     = 1; /*struct VkImageSubresourceRange for steoreographic textures*/

   return p_img_vu_creat_inf;
}

int tp_posix_get_file_size(const char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

unsigned char* tp_posix_read_shader_file_bin(const char* filename)
{
    int fs = tp_posix_get_file_size(filename);
    unsigned char* buffer = malloc(fs * sizeof(unsigned char));
    FILE* fp = NULL;

    fp = fopen(filename, "rb");
    fread(buffer, sizeof(buffer), 1, fp);
    fclose(fp);

    return buffer;
}

Tp_Init_Vk_Stg_Seven* tp_super_init_vulkan_stg_seven(Tp_Init_Vk_Stg_Six* p_stg_six_strct)
{
    VkBool32* system_has_surface_support = check_for_sur_support(p_stg_six_strct);
    act_on_sur_support_res(p_stg_six_strct, system_has_surface_support);

    VkSwapchainKHR* p_vk_swc_khr = tp_create_vk_swc_khr(p_stg_six_strct);
    uint32_t vk_img_cntr = 0;
    VkImage* p_img_arr = tp_create_vk_img_arr(p_vk_swc_khr, p_stg_six_strct, &vk_img_cntr);
    VkImageViewCreateInfo* p_img_vu_creat_inf = tp_create_vk_ing_vu_creat_inf(p_img_arr);

    Tp_Init_Vk_Stg_Seven* p_stg_seven_strct = malloc(sizeof(Tp_Init_Vk_Stg_Seven));
    tp_exit_on_failed_malloc(p_stg_seven_strct, "Mallocing of Tp_Init_Vk_Stg_Seven failed!\n", -73);

    p_stg_seven_strct->surface_support = *system_has_surface_support;
    p_stg_seven_strct->p_vk_swc_khr    = p_vk_swc_khr;
    p_stg_seven_strct->p_img_vu_creat_inf = p_img_vu_creat_inf;
    p_stg_seven_strct->p_vk_img_arr    = p_img_arr;

    if(p_stg_six_strct->is_initialized) {
        p_stg_seven_strct->is_initialized = TP_TRUE;
        p_stg_seven_strct->last_stg       = p_stg_six_strct;
    } else {
        printf("Stage seven was not completely initialized because stage six isn't, yet!\n");
        exit(-14);
    }

    return p_stg_seven_strct;
}
