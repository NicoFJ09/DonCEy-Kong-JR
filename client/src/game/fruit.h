#ifndef FRUIT_H
#define FRUIT_H

#include <stdbool.h>
#include "level.h"  // Now we CAN include level.h because Fruit/PointsPopup are defined there
#include "player.h"  // Need Player typedef for collision checking
#include "../network/connection.h"  // Need Connection for event sending

// Note: Fruit and PointsPopup typedefs are now defined in level.h
// This avoids circular dependency issues

// ============================================================
// FRUIT FUNCTIONS
// ============================================================

/**
 * Initialize fruits for the level
 * @param level Level to initialize fruits for
 */
void fruit_initialize(Level* level);

/**
 * Initialize points popup system
 * @param level Level to initialize popups for
 */
void fruit_initialize_popups(Level* level);

/**
 * Render all fruits in the level
 * @param level Level containing fruits to render
 */
void fruit_render(Level* level);

/**
 * Render all active points popups
 * @param level Level containing popups to render
 */
void fruit_render_popups(Level* level);

/**
 * Update points popups (decrease lifetime)
 * @param level Level containing popups to update
 * @param deltaTime Time elapsed since last frame
 */
void fruit_update_popups(Level* level, float deltaTime);

/**
 * Remove fruit by ID (FASE 4: Admin panel deletion)
 * @param level Level containing fruits
 * @param fruit_id ID of fruit to remove
 * @return true if fruit was found and removed, false otherwise
 */
bool fruit_remove_by_id(Level* level, int fruit_id);

/**
 * Check collision between player and fruits
 * @param player Player to check collision with
 * @param level Level containing fruits
 */
void fruit_check_collision(Player* player, Level* level, Connection* conn);

/**
 * Collect a fruit and award points to player
 * @param level Level containing the fruit
 * @param fruit_index Index of fruit to collect
 * @param player Player who collected the fruit (to update score)
 * @return Points awarded
 */
int fruit_collect(Level* level, int fruit_index);

/**
 * Cleanup fruit resources
 * @param level Level to cleanup fruits from
 */
void fruit_destroy(Level* level);

// ============================================================
// ADMIN PANEL SPAWN FUNCTIONS (FASE 3)
// ============================================================

/**
 * Spawn fruit at specific vine and position (admin controlled)
 * @param level Level to spawn fruit in
 * @param fruit_id Unique fruit ID from server
 * @param vine_id Vine ID (1-11)
 * @param position_y Vertical position on vine (0-24 depending on vine)
 * @param type_str Fruit type string ("Mango- 800 pts", etc.)
 * @return true if spawn successful, false if failed
 */
bool fruit_spawn_admin(Level* level, int fruit_id, int vine_id, int position_y, const char* type_str);

#endif
