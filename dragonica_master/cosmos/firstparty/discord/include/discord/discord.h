#pragma once

namespace discord
{

bool init(__int64 client_id);
void activity_set_details(const char* details);
void activity_set_state(const char* state);
void activity_set_large_image(const char* image, const char* text);
void activity_set_small_image(const char* image, const char* text);
void update_activity();
bool tick();

} // namespace discord
