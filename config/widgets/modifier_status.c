#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/modifiers_state_changed.h>
#include <zmk/keymap.h>
#include <zmk/hid.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <lvgl.h>

#if IS_ENABLED(CONFIG_ZMK_DISPLAY)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct modifier_status_state {
    zmk_mod_flags_t modifiers;
    uint8_t layer;
};

struct zmk_widget_modifier_status {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *modifier_label;
    lv_obj_t *layer_label;
};

static struct modifier_status_state get_modifier_status_state() {
    return (struct modifier_status_state){
        .modifiers = zmk_hid_get_explicit_mods(),
        .layer = zmk_keymap_highest_layer_active()
    };
}

static void set_modifier_symbol(lv_obj_t *label, zmk_mod_flags_t modifiers) {
    char text[16] = {0};
    
    // Build modifier string with + separators
    bool has_mod = false;
    
    if (modifiers & (MOD_LCTL | MOD_RCTL)) {
        strcat(text, "CTRL");
        has_mod = true;
    }
    if (modifiers & (MOD_LSFT | MOD_RSFT)) {
        if (has_mod) strcat(text, "+");
        strcat(text, "SFT");
        has_mod = true;
    }
    if (modifiers & (MOD_LALT | MOD_RALT)) {
        if (has_mod) strcat(text, "+");
        strcat(text, "ALT");
        has_mod = true;
    }
    if (modifiers & (MOD_LGUI | MOD_RGUI)) {
        if (has_mod) strcat(text, "+");
        strcat(text, "GUI");
        has_mod = true;
    }
    
    // Show "Ready" when no modifiers active
    if (!has_mod) {
        strcpy(text, "Ready");
    }
    
    lv_label_set_text(label, text);
}

static void set_layer_symbol(lv_obj_t *label, uint8_t layer) {
    char text[16];
    snprintf(text, sizeof(text), "Layer %d", layer);
    lv_label_set_text(label, text);
}

static void modifier_status_update_cb(struct modifier_status_state state) {
    struct zmk_widget_modifier_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_modifier_symbol(widget->modifier_label, state.modifiers);
        set_layer_symbol(widget->layer_label, state.layer);
    }
}

static int modifier_status_listener(const zmk_event_t *eh) {
    modifier_status_update_cb(get_modifier_status_state());
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(widget_modifier_status, modifier_status_listener);
ZMK_SUBSCRIPTION(widget_modifier_status, zmk_modifiers_state_changed);
ZMK_SUBSCRIPTION(widget_modifier_status, zmk_layer_state_changed);

int zmk_widget_modifier_status_init(struct zmk_widget_modifier_status *widget,
                                   lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    
    // Configure main container for 128x32 OLED
    lv_obj_set_size(widget->obj, 128, 32);
    lv_obj_clear_flag(widget->obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 2, LV_PART_MAIN);
    
    // Create modifier status label (top, larger font)
    widget->modifier_label = lv_label_create(widget->obj);
    lv_obj_align(widget->modifier_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(widget->modifier_label, "Ready");
    lv_obj_set_style_text_font(widget->modifier_label, 
                               &lv_font_montserrat_12, LV_PART_MAIN);
    
    // Create layer label (bottom, smaller font)  
    widget->layer_label = lv_label_create(widget->obj);
    lv_obj_align(widget->layer_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text(widget->layer_label, "Layer 0");
    lv_obj_set_style_text_font(widget->layer_label, 
                               &lv_font_montserrat_10, LV_PART_MAIN);
    
    sys_slist_append(&widgets, &widget->node);
    
    // Initialize with current state
    modifier_status_update_cb(get_modifier_status_state());
    
    return 0;
}

lv_obj_t *zmk_widget_modifier_status_obj(struct zmk_widget_modifier_status *widget) {
    return widget->obj;
}

#endif // IS_ENABLED(CONFIG_ZMK_DISPLAY)
