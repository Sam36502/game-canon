#include "../include/sprite.h"


static Texture2D __spritesheet;


void SPR_load_spritesheet(Image img) {
	if (IsTextureValid(__spritesheet)) {
		TraceLog(LOG_WARNING, "Loaded a new spritesheet before the old one was unloaded");
		SPR_unload_spritesheet();
	}

	__spritesheet = LoadTextureFromImage(img);
}

void SPR_unload_spritesheet() {
	if (!IsTextureValid(__spritesheet)) return;

	UnloadTexture(__spritesheet);
}

void SPR_draw(SPR_Sprite sprite) {
	if (!IsTextureValid(__spritesheet)) {
		TraceLog(LOG_ERROR, "Trying to draw a sprite without any spritesheet loaded!");
		return;
	}

	DrawTexturePro(__spritesheet,
		(Rectangle){
			sprite.spritesheet_origin.x + (sprite.frame_index * sprite.rect.width),
			sprite.spritesheet_origin.y, 
			sprite.rect.width, sprite.rect.height
		},
		(Rectangle){
			sprite.rect.x, sprite.rect.y, 
			sprite.rect.width * PIXEL_SIZE, sprite.rect.height * PIXEL_SIZE
		},
		(Vector2){ 0, 0 },
		0.0f, sprite.clr
	);
}
