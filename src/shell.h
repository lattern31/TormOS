#ifndef SHELL_H
#define SHELL_H

#include "vga/terminal.h"

#define HELP_MSG "available commands\ncalc {statement}\nsleep {time}\ntabnew {tabtype} - TERMINAL SCENE INFO\nto switch tab press tab"

char* get_args(char *command);
void handle_calc_command(StateTerminal_t *terminal, char *args_ptr);
void handle_sleep_command(StateTerminal_t *terminal, char *args_ptr);
void handle_tabnew_command(StateTerminal_t *terminal, char *args_ptr);
void handle_command(StateTerminal_t *terminal);

#endif
