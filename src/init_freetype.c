#include <ft2build.h>
#include FT_FREETYPE_H

#include "render_text.h"
#include "config.h"

FT_Context* init_freetype() {
    FT_Context *ctx = malloc(sizeof(FT_Context));
    if (!ctx) return NULL;

    if (FT_Init_FreeType(&ctx->library)) {
        free(ctx);
        return NULL;
    }

    if (FT_New_Face(ctx->library, FONT, 0, &ctx->face)) {
        FT_Done_FreeType(ctx->library);
        free(ctx);
        return NULL;
    }

    if (FT_Set_Pixel_Sizes(ctx->face, 0, FONT_SIZE)) {
        FT_Done_Face(ctx->face);
        FT_Done_FreeType(ctx->library);
        free(ctx);
        return NULL;
    }

    ctx->set_pixel = 1;
    return ctx;
}

