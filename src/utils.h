
#include "game_context.h"

void static inline DrawInfiniteGrid(Camera2D cam, f32 spacing, Color color) {
  f32 window_width = GetScreenWidth();
  f32 window_height = GetScreenHeight();

  Vector2 top_left = GetScreenToWorld2D((Vector2){0, 0}, cam);
  Vector2 bottom_right =
      GetScreenToWorld2D((Vector2){window_width, window_height}, cam);

  i32 min_x = floor(top_left.x / spacing) - 1;
  i32 max_x = ceil(bottom_right.x / spacing) - 1;
  i32 min_y = floor(top_left.y / spacing) - 1;
  i32 max_y = ceil(bottom_right.y / spacing) - 1;

  for (i32 x = min_x; x <= max_x; x++) {
    f32 x_pos = (f32)x * spacing;
    DrawLineEx((Vector2){x_pos, top_left.y}, (Vector2){x_pos, bottom_right.y},
               1.5, color);
  }

  for (i32 y = min_y; y <= max_y; y++) {
    f32 y_pos = (f32)y * spacing;
    DrawLineEx((Vector2){top_left.x, y_pos}, (Vector2){bottom_right.x, y_pos},
               1.5, color);
  }
}

// Draw part of a texture (defined by a rectangle) with rotation and scale tiled
// into dest
static inline void DrawTextureTiled(Texture2D texture, Rectangle source,
                                    Rectangle dest, Vector2 origin,
                                    float rotation, float scale, Color tint) {
  if ((texture.id <= 0) || (scale <= 0.0f))
    return; // Wanna see a infinite loop?!...just delete this line!
  if ((source.width == 0) || (source.height == 0))
    return;

  int tileWidth = (int)(source.width * scale),
      tileHeight = (int)(source.height * scale);
  if ((dest.width < tileWidth) && (dest.height < tileHeight)) {
    // Can fit only one tile
    DrawTexturePro(
        texture,
        (Rectangle){source.x, source.y,
                    ((float)dest.width / tileWidth) * source.width,
                    ((float)dest.height / tileHeight) * source.height},
        (Rectangle){dest.x, dest.y, dest.width, dest.height}, origin, rotation,
        tint);
  } else if (dest.width <= tileWidth) {
    // Tiled vertically (one column)
    int dy = 0;
    for (; dy + tileHeight < dest.height; dy += tileHeight) {
      DrawTexturePro(
          texture,
          (Rectangle){source.x, source.y,
                      ((float)dest.width / tileWidth) * source.width,
                      source.height},
          (Rectangle){dest.x, dest.y + dy, dest.width, (float)tileHeight},
          origin, rotation, tint);
    }

    // Fit last tile
    if (dy < dest.height) {
      DrawTexturePro(
          texture,
          (Rectangle){source.x, source.y,
                      ((float)dest.width / tileWidth) * source.width,
                      ((float)(dest.height - dy) / tileHeight) * source.height},
          (Rectangle){dest.x, dest.y + dy, dest.width, dest.height - dy},
          origin, rotation, tint);
    }
  } else if (dest.height <= tileHeight) {
    // Tiled horizontally (one row)
    int dx = 0;
    for (; dx + tileWidth < dest.width; dx += tileWidth) {
      DrawTexturePro(
          texture,
          (Rectangle){source.x, source.y, source.width,
                      ((float)dest.height / tileHeight) * source.height},
          (Rectangle){dest.x + dx, dest.y, (float)tileWidth, dest.height},
          origin, rotation, tint);
    }

    // Fit last tile
    if (dx < dest.width) {
      DrawTexturePro(
          texture,
          (Rectangle){source.x, source.y,
                      ((float)(dest.width - dx) / tileWidth) * source.width,
                      ((float)dest.height / tileHeight) * source.height},
          (Rectangle){dest.x + dx, dest.y, dest.width - dx, dest.height},
          origin, rotation, tint);
    }
  } else {
    // Tiled both horizontally and vertically (rows and columns)
    int dx = 0;
    for (; dx + tileWidth < dest.width; dx += tileWidth) {
      int dy = 0;
      for (; dy + tileHeight < dest.height; dy += tileHeight) {
        DrawTexturePro(texture, source,
                       (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth,
                                   (float)tileHeight},
                       origin, rotation, tint);
      }

      if (dy < dest.height) {
        DrawTexturePro(texture,
                       (Rectangle){source.x, source.y, source.width,
                                   ((float)(dest.height - dy) / tileHeight) *
                                       source.height},
                       (Rectangle){dest.x + dx, dest.y + dy, (float)tileWidth,
                                   dest.height - dy},
                       origin, rotation, tint);
      }
    }

    // Fit last column of tiles
    if (dx < dest.width) {
      int dy = 0;
      for (; dy + tileHeight < dest.height; dy += tileHeight) {
        DrawTexturePro(
            texture,
            (Rectangle){source.x, source.y,
                        ((float)(dest.width - dx) / tileWidth) * source.width,
                        source.height},
            (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx,
                        (float)tileHeight},
            origin, rotation, tint);
      }

      // Draw final tile in the bottom right corner
      if (dy < dest.height) {
        DrawTexturePro(
            texture,
            (Rectangle){source.x, source.y,
                        ((float)(dest.width - dx) / tileWidth) * source.width,
                        ((float)(dest.height - dy) / tileHeight) *
                            source.height},
            (Rectangle){dest.x + dx, dest.y + dy, dest.width - dx,
                        dest.height - dy},
            origin, rotation, tint);
      }
    }
  }
}
