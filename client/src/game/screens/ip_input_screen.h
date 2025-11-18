#ifndef IP_INPUT_SCREEN_H
#define IP_INPUT_SCREEN_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Display IP input screen and get user input
 * @param ip_buffer Buffer to store the IP address (must be at least 16 bytes)
 * @param buffer_size Size of the buffer
 * @param show_error If true, shows "Could not connect" error message
 * @return true if user provided IP and pressed ENTER, false if window was closed
 */
bool show_ip_input_screen(char* ip_buffer, size_t buffer_size, bool show_error);

#endif