#include "stdio.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "keyboard.h"
#include "pic.h"
#include "vga/vga.h"
#include "vga/3d.h"
#include "vga/terminal.h"
#include "vga/gui.h"
#include "math.h"
#include "timer.h"
#include "memory.h"
#include "heap.h"
#include "common.h"
#include "string.h"
#include "shell.h"

#if defined(__linux__)
#error "This code must be compiled with a cross-compiler"
#elif !defined(__i386__)
#error "This code must be compiled with an x86-elf compiler"
#endif


void debug_point(Point_3d_t p) {
    char bufs[3][5];

    dbl_to_str(bufs[0], p.x);
    dbl_to_str(bufs[1], p.y);
    dbl_to_str(bufs[2], p.z);
    for (int i = 0; i < 3; i++) {
        //vga_terminal_print(bufs[i]);
        //vga_terminal_print(" ");
    }
}

void handle_keyboard() {
    if (!kbhit())
        return;

    char buffer[2];
    kbgets(buffer, sizeof(buffer));

    if (buffer[0] == '\t') {
        int cnt = 0;
        while (tab_stack[cnt] != INVALID_PTR) {
            cnt++;
        }
        tab = (tab + 1) % cnt;
        return;
    }

    if (tab_stack[tab]->type == TERMINAL) {
        StateTerminal_t *terminal = &tab_stack[tab]->state.terminal;
        if (buffer[0] == '\b') {
            vga_terminal_delete(terminal);
            return;
        }
        vga_terminal_print(terminal, buffer); //  print inputed char 
        if (buffer[0] == '\n') {
            handle_command(terminal);
        } else {
            *terminal->cmd_ptr++ = buffer[0];
        }
    }

    if (tab_stack[tab]->type == SCENE) { 
        if (buffer[0] == 'w') {
            cam = move_cam_by_yaw(cam, global_angle, 0.05);
            global_dz -= 0.05;
        }
        if (buffer[0] == 's') {
            cam = move_cam_by_yaw(cam, global_angle, -0.05);
            global_dz += 0.05;
        }
        if (buffer[0] == 'q') {
            global_angle -= PI * 0.05;
        }
        if (buffer[0] == 'e') {
            global_angle += PI * 0.05;
        }
        if (buffer[0] == 'z') {
            test_angle += PI * 0.05;
        }
    }
}


void kernel_main() {
	term_init();
    gdt_init();
    idt_init();
    pic_init();
    ISR_init();
    asm volatile("sti");
    KHeap_init();

    timer_init(50);

    set_vga_mode_320x200x256();

    // TODO figures are meant to be in tab state
    Figure_t cube1 = create_cube(0.3, (Point_3d_t){0, 0, 1}, 0.1);
    Figure_t cube2 = create_cube(0.3, (Point_3d_t){0, 1, -1}, 0.1);
    Figure_t tetrahedron1 = create_tetrahedron(
        1, (Point_3d_t){0.3, 0.5, 1}, 0.1
    );
    Figure_t *figures[] = {&cube1, &cube2, &tetrahedron1};

    draw_intro();
    render_screen();
    timer_sleep(2000);

    for (int i = 0; i < MAXTAB+1; i++) {
        tab_stack[i] = INVALID_PTR;
    }

    tab_stack[0] = create_tab(TERMINAL);
    // TODO tab closing and heap free
    while (1) {
        handle_keyboard();
        clear_screen(0);

        switch (tab_stack[tab]->type) {
            case TERMINAL:
                draw_terminal(&tab_stack[tab]->state.terminal);
                break;
            case SCENE:
                draw_scene(figures, 3);
                break;
            case INFO:
                draw_info();
                break;
            default:
                draw_string("wrong tab",
                    (PointScreen_t){.x=align_center_text(0, 320, 9*8), .y=96},
                    2);
        }
        draw_navbar();
        render_screen();
    }
}
