#include "clap/clap.h"
#include <stdlib.h>
#include <string.h>

// Declare your assembly function!
extern void asm_process(const float* input, float* output, uint32_t frames);

// 1. The Audio Process Block
static clap_process_status my_process(const clap_plugin_t *plugin, const clap_process_t *process) {
    uint32_t frames = process->frames_count;
    if (frames == 0) return CLAP_PROCESS_CONTINUE;

    // Safety: Ensure host provided buffers
    if (process->audio_inputs_count == 0 || process->audio_outputs_count == 0) return CLAP_PROCESS_CONTINUE;
    if (!process->audio_inputs[0].data32 || !process->audio_outputs[0].data32) return CLAP_PROCESS_CONTINUE;

    // Grab the Left Channel
    const float* in_l = process->audio_inputs[0].data32[0];
    float* out_l = process->audio_outputs[0].data32[0];
    if (in_l && out_l) asm_process(in_l, out_l, frames);

    // Grab the Right Channel (if stereo)
    if (process->audio_inputs[0].channel_count > 1 && process->audio_outputs[0].channel_count > 1) {
        const float* in_r = process->audio_inputs[0].data32[1];
        float* out_r = process->audio_outputs[0].data32[1];
        if (in_r && out_r) asm_process(in_r, out_r, frames);
    }

    return CLAP_PROCESS_CONTINUE;
}

// 2. Boilerplate
static bool my_init(const struct clap_plugin *plugin) { return true; }
static void my_destroy(const struct clap_plugin *plugin) { free((void*)plugin); }
static bool my_activate(const struct clap_plugin *plugin, double sample_rate, uint32_t min_frames, uint32_t max_frames) { return true; }
static void my_deactivate(const struct clap_plugin *plugin) {}
static bool my_start_processing(const struct clap_plugin *plugin) { return true; }
static void my_stop_processing(const struct clap_plugin *plugin) {}
static void my_reset(const struct clap_plugin *plugin) {}
static void my_on_main_thread(const struct clap_plugin *plugin) {}

// 3. Audio Port Extension (Tells Reaper "I handle stereo audio")
static uint32_t audio_ports_count(const clap_plugin_t *plugin, bool is_input) {
    return 1; // 1 input port, 1 output port
}

static bool audio_ports_get(const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_audio_port_info_t *info) {
    if (index > 0) return false;
    
    info->id = is_input ? 100 : 200;
    info->channel_count = 2; // Stereo
    info->in_place_pair = is_input ? 200 : 100; // Link input/output for in-place processing
    strncpy(info->name, is_input ? "Stereo Input" : "Stereo Output", CLAP_NAME_SIZE);
    info->name[CLAP_NAME_SIZE - 1] = '\0';
    return true;
}

static const clap_plugin_audio_ports_t audio_ports_impl = {
    .count = audio_ports_count,
    .get = audio_ports_get
};

static const void* my_get_extension(const struct clap_plugin *plugin, const char *id) {
    if (strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0) return &audio_ports_impl;
    return NULL;
}

// 4. Plugin Metadata & Factory
static const clap_plugin_descriptor_t my_descriptor = {
    .clap_version = CLAP_VERSION_INIT,
    .id = "critter.asm.halfgain",
    .name = "ASM Half-Gain",
    .vendor = "Terminal Critter",
    .url = "", .manual_url = "", .support_url = "",
    .version = "1.0",
    .description = "Pure Assembly Audio Processor",
    .features = (const char *[]){ CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, NULL }
};

static uint32_t plugin_factory_get_plugin_count(const struct clap_plugin_factory *factory) { return 1; }
static const clap_plugin_descriptor_t* plugin_factory_get_plugin_descriptor(const struct clap_plugin_factory *factory, uint32_t index) {
    return (index == 0) ? &my_descriptor : NULL;
}

static const clap_plugin_t* plugin_factory_create_plugin(const struct clap_plugin_factory *factory, const clap_host_t *host, const char *plugin_id) {
    if (strcmp(plugin_id, my_descriptor.id) != 0) return NULL;

    clap_plugin_t *p = (clap_plugin_t*)calloc(1, sizeof(clap_plugin_t));
    if (!p) return NULL;

    p->desc = &my_descriptor;
    p->plugin_data = p;
    p->init = my_init;
    p->destroy = my_destroy;
    p->activate = my_activate;
    p->deactivate = my_deactivate;
    p->start_processing = my_start_processing;
    p->stop_processing = my_stop_processing;
    p->reset = my_reset;
    p->process = my_process;
    p->get_extension = my_get_extension;
    p->on_main_thread = my_on_main_thread;

    return p;
}

static const clap_plugin_factory_t my_factory = {
    .get_plugin_count = plugin_factory_get_plugin_count,
    .get_plugin_descriptor = plugin_factory_get_plugin_descriptor,
    .create_plugin = plugin_factory_create_plugin
};

// 5. Entry Point
static bool entry_init(const char *plugin_path) { return true; }
static void entry_deinit(void) {}
static const void* entry_get_factory(const char *factory_id) {
    return (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0) ? &my_factory : NULL;
}

CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
    .clap_version = CLAP_VERSION_INIT,
    .init = entry_init,
    .deinit = entry_deinit,
    .get_factory = entry_get_factory
};
