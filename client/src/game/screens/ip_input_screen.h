#ifndef IP_INPUT_SCREEN_H
#define IP_INPUT_SCREEN_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Display IP input screen and get user input
 * @param ip_buffer Buffer to store the IP address (must be at least 16 bytes)
 * @param buffer_size Size of the buffer
 * @param error_message Error message to display, or NULL for no error.
 *                      Pass "" (empty string) to show default "Could not connect"
 * @return true if user provided IP and pressed ENTER, false if window was closed
 */
bool show_ip_input_screen(char* ip_buffer, size_t buffer_size, const char* error_message);

#endif