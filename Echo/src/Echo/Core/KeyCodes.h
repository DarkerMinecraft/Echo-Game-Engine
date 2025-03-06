#pragma once


//glfw3
#define EC_KEY_SPACE              32
#define EC_KEY_APOSTROPHE         39  /* ' */
#define EC_KEY_COMMA              44  /* , */
#define EC_KEY_MINUS              45  /* - */
#define EC_KEY_PERIOD             46  /* . */
#define EC_KEY_SLASH              47  /* / */
#define EC_KEY_0                  48
#define EC_KEY_1                  49
#define EC_KEY_2                  50
#define EC_KEY_3                  51
#define EC_KEY_4                  52
#define EC_KEY_5                  53
#define EC_KEY_6                  54
#define EC_KEY_7                  55
#define EC_KEY_8                  56
#define EC_KEY_9                  57
#define EC_KEY_SEMICOLON          59  /* ; */
#define EC_KEY_EQUAL              61  /* = */
#define EC_KEY_A                  65
#define EC_KEY_B                  66
#define EC_KEY_C                  67
#define EC_KEY_D                  68
#define EC_KEY_E                  69
#define EC_KEY_F                  70
#define EC_KEY_G                  71
#define EC_KEY_H                  72
#define EC_KEY_I                  73
#define EC_KEY_J                  74
#define EC_KEY_K                  75
#define EC_KEY_L                  76
#define EC_KEY_M                  77
#define EC_KEY_N                  78
#define EC_KEY_O                  79
#define EC_KEY_P                  80
#define EC_KEY_Q                  81
#define EC_KEY_R                  82
#define EC_KEY_S                  83
#define EC_KEY_T                  84
#define EC_KEY_U                  85
#define EC_KEY_V                  86
#define EC_KEY_W                  87
#define EC_KEY_X                  88
#define EC_KEY_Y                  89
#define EC_KEY_Z                  90
#define EC_KEY_LEFT_BRACKET       91  /* [ */
#define EC_KEY_BACKSLASH          92  /* \ */
#define EC_KEY_RIGHT_BRACKET      93  /* ] */
#define EC_KEY_GRAVE_ACCENT       96  /* ` */
#define EC_KEY_WORLD_1            161 /* non-US #1 */
#define EC_KEY_WORLD_2            162 /* non-US #2 */

#define EC_KEY_ESCAPE             256
#define EC_KEY_ENTER              257
#define EC_KEY_TAB                258
#define EC_KEY_BACKSPACE          259
#define EC_KEY_INSERT             260
#define EC_KEY_DELETE             261
#define EC_KEY_RIGHT              262
#define EC_KEY_LEFT               263
#define EC_KEY_DOWN               264
#define EC_KEY_UP                 265
#define EC_KEY_PAGE_UP            266
#define EC_KEY_PAGE_DOWN          267
#define EC_KEY_HOME               268
#define EC_KEY_END                269
#define EC_KEY_CAPS_LOCK          280
#define EC_KEY_SCROLL_LOCK        281
#define EC_KEY_NUM_LOCK           282
#define EC_KEY_PRINT_SCREEN       283
#define EC_KEY_PAUSE              284
#define EC_KEY_F1                 290
#define EC_KEY_F2                 291
#define EC_KEY_F3                 292
#define EC_KEY_F4                 293
#define EC_KEY_F5                 294
#define EC_KEY_F6                 295
#define EC_KEY_F7                 296
#define EC_KEY_F8                 297
#define EC_KEY_F9                 298
#define EC_KEY_F10                299
#define EC_KEY_F11                300
#define EC_KEY_F12                301
#define EC_KEY_F13                302
#define EC_KEY_F14                303
#define EC_KEY_F15                304
#define EC_KEY_F16                305
#define EC_KEY_F17                306
#define EC_KEY_F18                307
#define EC_KEY_F19                308
#define EC_KEY_F20                309
#define EC_KEY_F21                310
#define EC_KEY_F22                311
#define EC_KEY_F23                312
#define EC_KEY_F24                313
#define EC_KEY_F25                314
#define EC_KEY_KP_0               320
#define EC_KEY_KP_1               321
#define EC_KEY_KP_2               322
#define EC_KEY_KP_3               323
#define EC_KEY_KP_4               324
#define EC_KEY_KP_5               325
#define EC_KEY_KP_6               326
#define EC_KEY_KP_7               327
#define EC_KEY_KP_8               328
#define EC_KEY_KP_9               329
#define EC_KEY_KP_DECIMAL         330
#define EC_KEY_KP_DIVIDE          331
#define EC_KEY_KP_MULTIPLY        332
#define EC_KEY_KP_SUBTRACT        333
#define EC_KEY_KP_ADD             334
#define EC_KEY_KP_ENTER           335
#define EC_KEY_KP_EQUAL           336
#define EC_KEY_LEFT_SHIFT         340
#define EC_KEY_LEFT_CONTROL       341
#define EC_KEY_LEFT_ALT           342
#define EC_KEY_LEFT_SUPER         343
#define EC_KEY_RIGHT_SHIFT        344
#define EC_KEY_RIGHT_CONTROL      345
#define EC_KEY_RIGHT_ALT          346
#define EC_KEY_RIGHT_SUPER        347
#define EC_KEY_MENU               348

#define EC_KEY_LAST               EC_KEY_MENU

/*! @} */

/*! @defgroup mods Modifier key flags
 *  @brief Modifier key flags.
 *
 *  See [key input](@ref input_key) for how these are used.
 *
 *  @ingroup input
 *  @{ */

 /*! @brief If this bit is set one or more Shift keys were held down.
  *
  *  If this bit is set one or more Shift keys were held down.
  */
#define EC_MOD_SHIFT           0x0001
  /*! @brief If this bit is set one or more Control keys were held down.
   *
   *  If this bit is set one or more Control keys were held down.
   */
#define EC_MOD_CONTROL         0x0002
   /*! @brief If this bit is set one or more Alt keys were held down.
	*
	*  If this bit is set one or more Alt keys were held down.
	*/
#define EC_MOD_ALT             0x0004
	/*! @brief If this bit is set one or more Super keys were held down.
	 *
	 *  If this bit is set one or more Super keys were held down.
	 */
#define EC_MOD_SUPER           0x0008
	 /*! @brief If this bit is set the Caps Lock key is enabled.
	  *
	  *  If this bit is set the Caps Lock key is enabled and the @ref
	  *  EC_LOCK_KEY_MODS input mode is set.
	  */
#define EC_MOD_CAPS_LOCK       0x0010
	  /*! @brief If this bit is set the Num Lock key is enabled.
	   *
	   *  If this bit is set the Num Lock key is enabled and the @ref
	   *  EC_LOCK_KEY_MODS input mode is set.
	   */
#define EC_MOD_NUM_LOCK        0x0020