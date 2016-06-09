/****************************************************************************
 * Loadiine resource files.
 * This file is generated automatically.
 * Includes 15 files.
 *
 * NOTE:
 * Any manual modification of this file will be overwriten by the generation.
 ****************************************************************************/
#ifndef _FILELIST_H_
#define _FILELIST_H_

#include <gctypes.h>

typedef struct _RecourceFile
{
	const char *filename;
	const u8   *DefaultFile;
	const u32  &DefaultFileSize;
	u8		   *CustomFile;
	u32		    CustomFileSize;
} RecourceFile;

extern const u8 button_png[];
extern const u32 button_png_size;

extern const u8 button_click_mp3[];
extern const u32 button_click_mp3_size;

extern const u8 font_ttf[];
extern const u32 font_ttf_size;

extern const u8 GET_png[];
extern const u32 GET_png_size;

extern const u8 INSTALLED_png[];
extern const u32 INSTALLED_png_size;

extern const u8 launchMenuBox_png[];
extern const u32 launchMenuBox_png_size;

extern const u8 LOCAL_png[];
extern const u32 LOCAL_png_size;

extern const u8 player1_point_png[];
extern const u32 player1_point_png_size;

extern const u8 player2_point_png[];
extern const u32 player2_point_png_size;

extern const u8 player3_point_png[];
extern const u32 player3_point_png_size;

extern const u8 player4_point_png[];
extern const u32 player4_point_png_size;

extern const u8 progressWindow_png[];
extern const u32 progressWindow_png_size;

extern const u8 slimers_mp3[];
extern const u32 slimers_mp3_size;

extern const u8 titlehbas2_png[];
extern const u32 titlehbas2_png_size;

extern const u8 UPDATE_png[];
extern const u32 UPDATE_png_size;

static RecourceFile RecourceList[] =
{
	{"button.png", button_png, button_png_size, NULL, 0},
	{"button_click.mp3", button_click_mp3, button_click_mp3_size, NULL, 0},
	{"font.ttf", font_ttf, font_ttf_size, NULL, 0},
	{"GET.png", GET_png, GET_png_size, NULL, 0},
	{"INSTALLED.png", INSTALLED_png, INSTALLED_png_size, NULL, 0},
	{"launchMenuBox.png", launchMenuBox_png, launchMenuBox_png_size, NULL, 0},
	{"LOCAL.png", LOCAL_png, LOCAL_png_size, NULL, 0},
	{"player1_point.png", player1_point_png, player1_point_png_size, NULL, 0},
	{"player2_point.png", player2_point_png, player2_point_png_size, NULL, 0},
	{"player3_point.png", player3_point_png, player3_point_png_size, NULL, 0},
	{"player4_point.png", player4_point_png, player4_point_png_size, NULL, 0},
	{"progressWindow.png", progressWindow_png, progressWindow_png_size, NULL, 0},
	{"slimers.mp3", slimers_mp3, slimers_mp3_size, NULL, 0},
	{"titlehbas2.png", titlehbas2_png, titlehbas2_png_size, NULL, 0},
	{"UPDATE.png", UPDATE_png, UPDATE_png_size, NULL, 0},
	{NULL, NULL, 0, NULL, 0}
};

#endif
