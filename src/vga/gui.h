#ifndef VGA_GUI_H
#define VGA_GUI_H

extern int tab;

int align_center_text(int start, int end, int text_len);
void draw_tab(int tab);
void draw_terminal();
void draw_scene(Figure_t *figures[], int quantity);

#endif
