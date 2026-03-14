#ifndef VGA_GUI_H
#define VGA_GUI_H

#include "../stdint.h"
#include "3d.h"
#include "terminal.h"
#include "vga.h"

#define MAXTAB 4

extern int tab;

typedef enum {
    TERMINAL,
    SCENE,
    INFO,
} TabType;

union State {
    struct StateTerminal terminal;
    struct StateScene scene;
};

typedef struct Tab {
    TabType type;
    union State state;
} Tab_t;

extern Tab_t *tab_stack[MAXTAB+1];

int align_center_text(int start, int end, int text_len);
void draw_navbar();
void draw_terminal(StateTerminal_t *terminal);
void draw_scene(Figure_t *figures[], int quantity); 
void draw_nya_xpm(uint8_t scale_size);
Tab_t* create_tab(TabType type);
void draw_intro();
void draw_info();

#endif
