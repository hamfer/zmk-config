#pragma once

#include <zephyr/kernel.h>

// Forward declarations to avoid including LVGL in header
struct _lv_obj_t;
typedef struct _lv_obj_t lv_obj_t;

struct zmk_widget_modifier_status {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *modifier_label;
    lv_obj_t *layer_label;
};

int zmk_widget_modifier_status_init(struct zmk_widget_modifier_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_modifier_status_obj(struct zmk_widget_modifier_status *widget);
