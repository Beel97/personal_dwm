/*
bread's dwm build

patches applied (incl. some slight changes):

dwm-bartoggle-keybinds-6.4.diff ----- toggle pieces of the bar
dwm-bulkill-20231029-9f88553.diff ----- kill all except current; kill all in tag
dwm-colorbar-6.3.diff ----- fine control over statusbar colors
dwm-fixmultimon-6.4.diff ----- pretty much a bug fix
dwm-focusfullscreen-20211121-95e7342.diff ----- proper fullscreen
dwm-focusmaster-return-6.2.diff ----- switch to master from anywhere in the stack
dwm-focusmonmouse-6.2.diff ----- move mouse when you switch monitors via keybind
dwm-hide_vacant_tags-6.4.diff ----- hide tags with no windows
dwm-preventfocusshift-20240831-6.5.diff ----- automatically exit fullscreen when a window is spawned
dwm-restartsig-20180523-6.2.diff ----- refresh dwm after recompile, without quitting
dwm-spawntag-6.2.diff ----- spawn an application when tag is middle-clicked
dwm-stacker-6.2.diff ----- more utilities to manage the stack
dwm-statuscmd-20210405-67d76bd.diff ----- execute cmds when statusbar is clicked (used for dwmblocks)
dwm-sticky-6.5.diff ----- make a window follow from tag to tag
dwm-swallow-6.3.diff ----- terminal swallows launched programs
dwm-vanitygaps-6.2.diff ----- gaps are functional: your eye is less inclined to drift while working.
dwm-xrdb-6.4.diff - xresource database colors
*/

/* appearance */
static unsigned int borderpx        = 2;        /* border pixel of windows */
static unsigned int snap            = 32;       /* snap pixel */
static const unsigned int gappih    = 5;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 5;       /* vert inner gap between windows */
static const unsigned int gappoh    = 5;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 5;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar                  = 1;        /* 0 means no bar */
static const int showtitle          = 1;        /* 0 means no title */
static const int showtags           = 1;        /* 0 means no tags */
static const int showlayout         = 1;        /* 0 means no layout indicator */
static const int showstatus         = 1;        /* 0 means no status bar */
static const int showfloating       = 0;        /* 0 means no floating indicator */
static int topbar                   = 1;        /* 0 means bottom bar */

static const char *fonts[]          = { "monospace:size=10","FiraCode Nerd Font:size=10", "Hack Nerd Font Mono:size=16", "NotoColorEmoji:pixelsize=14:antialias=true:autohint=true"  };

/* default colors used if xrdb is not loaded */
static char normbgcolor[]           = "#2e3440";
static char normbordercolor[]       = "#4c566a";
static char normfgcolor[]           = "#d8dee9";
static char selfgcolor[]            = "#eceff4";
static char selbordercolor[]        = "#a3be8c";
static char selbgcolor[]            = "#b48ead";

static char *colors[][3] = {
       /*               fg           bg           border   */
		[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor},
		[SchemeSel]  = { selbgcolor,  selfgcolor,  selbordercolor  },
		/* for bar --> {text, background, null} */
		[SchemeStatus]  = { normfgcolor, normbgcolor,  normbgcolor  }, /* status R */
		[SchemeTagsSel]  = { normfgcolor, normbgcolor,  normbgcolor  }, /* tag L selected */
		[SchemeTagsNorm]  = { selbordercolor, normbgcolor,  normbgcolor  }, /* tag L unselected */
		[SchemeInfoSel]  = { normfgcolor, normbgcolor,  normbgcolor  }, /* info M selected */
		[SchemeInfoNorm]  = { normfgcolor, normbgcolor,  normbgcolor  }, /* info M unselected */
};


/* tagging */
static const char *tags[] = {
    "一", // ichi - 1
    "二", // ni   - 2
    "三", // san  - 3
    "四", // yon  - 4
    "五", // go   - 5
    "六", // roku - 6
    "七", // nana - 7
    "八", // hachi- 8
    "九"  // kyuu - 9
};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
  { NULL, NULL, "float-alacritty", 0, 1, -1 },
};

#include "vanitygaps.c"


/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 0; /* 1 will force focus on the fullscreen window */


static const Layout layouts[] = { /* alt glyphs: 󱡗 󱏋 */
	/* symbol     arrange function */
  { "󰫣",      spiral }, /* first entry is default */
	{ "󰊠",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
	{ "󰙀",      tile },
	{ "󰫥",      dwindle },
};

/* key definitions */
#define MODKEY Mod4Mask // windows key
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
  //Move to specific stack position uncomment if you want to use it
	/*{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
	{ MOD, XK_q,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_x,     ACTION##stack, {.i = -1 } }, */

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* helper for launching gtk application */
#define GTKCMD(cmd) { .v = (const char*[]){ "/usr/bin/gtk-launch", cmd, NULL } }

#define STATUSBAR "dwmblocks"
#define BROWSER "qutebrowser"

/* commands */
static const char *termcmd[]  = { "alacritty", NULL }; //my term change to your favorite terminal
static const char *spfcmd[] = { "alacritty", "-e", "spf", NULL };
//Volume control commands
static const char *mutevol[] = { "sh", "-c", "amixer set Master toggle && pkill -RTMIN+10 dwmblocks", NULL };
static const char *downvol[] = { "sh", "-c", "amixer set Master 5%- unmute && pkill -RTMIN+10 dwmblocks", NULL };
static const char *upvol[]   = { "sh", "-c", "amixer set Master 5%+ unmute && pkill -RTMIN+10 dwmblocks", NULL };
// Brillo
static const char *upbright[]   = { "sh", "-c", "brightnessctl set +5% && pkill -RTMIN+11 dwmblocks", NULL };
static const char *downbright[] = { "sh", "-c", "brightnessctl set 5%- && pkill -RTMIN+11 dwmblocks", NULL };
//screenshots maim
static const char *screenshot[] = { "sh", "-c", "maim -u -i $(xdotool getactivewindow) ~/Pictures/Screenshots/$(date +%Y-%m-%d_%H:%M:%S).png && notify-send 'Captura guardada'", NULL };
static const char *screenshotfull[] = { "sh", "-c", "maim ~/Pictures/Screenshots/$(date +%Y-%m-%d_%H:%M:%S).png && notify-send 'Captura guardada'", NULL };



static const Arg tagexec[] = { /* spawn application when tag is middle-clicked */
	{ .v = termcmd }, /* 1 */
	{ .v = termcmd }, /* 2 */
	{ .v = termcmd }, /* 3 */
	{ .v = termcmd }, /* 4 */
	{ .v = termcmd }, /* 5 */
	{ .v = termcmd }, /* 6 */
	{ .v = termcmd }, /* 7 */
	{ .v = termcmd }, /* 8 */
	{ .v = termcmd }, /* 9 */
    /* GTKCMD("gtkapplication") */
};

static const Key keys[] = {
	/* modifier                     key        function        argument */
  { 0, XF86XK_AudioMute,        spawn, {.v = mutevol } },
  { 0, XF86XK_AudioLowerVolume, spawn, {.v = downvol } },
  { 0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol } },
  { 0, XF86XK_MonBrightnessUp,   spawn, {.v = upbright } },
  { 0, XF86XK_MonBrightnessDown, spawn, {.v = downbright } },
  { ControlMask|Mod1Mask,         XK_w,      togglebar,      {0} },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} }, 
  { MODKEY,                       XK_c,      spawn,         {.v = (const char*[]){"dwm_quick_nvim_open.sh",NULL} } },
  { MODKEY|ShiftMask,             XK_c,      quit,     {0} },
	{ MODKEY,                       XK_t,      spawn,      {.v = termcmd} },
	{ MODKEY,                       XK_s,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_w,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY,                       XK_q,      killclient,           {0} },
  { MODKEY,                       XK_a,       spawn,        {.v = (const char*[]){"dwm_applauncher.sh",NULL} } },
  { MODKEY,                       XK_b,       spawn,        {.v =  (const char*[]){"dwm_browser_launcher.sh"}} },
  { MODKEY,                       XK_e,       spawn,        {.v = spfcmd} },
  { MODKEY|ShiftMask,             XK_w,       spawn,        {.v =  (const char*[]){"dwm_wallpaper.sh"}} },
  { MODKEY|ShiftMask,             XK_m,       spawn,        {.v =  (const char*[]){"dwm_monitors.sh"}} },
  { MODKEY,                       XK_BackSpace,       spawn,        {.v =  (const char*[]){"dwm_logout_options.sh"}} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = -1 } },

	{ MODKEY|ShiftMask,				XK_q,		killclient,     {.ui = 1} },  /* kill all windows besides current */
  { MODKEY|ShiftMask|ControlMask,  XK_q,	killclient,     {.ui = 2} },
	{ MODKEY|ControlMask,			XK_backslash, xrdb,			{.v = NULL } }, /* refresh xrdb; run this when setting new colors */


	{ MODKEY|ShiftMask,						XK_f,	   togglefullscreen, {0} }, /* focus fullscreen patch */
	{ MODKEY|ControlMask,           XK_space,  focusmaster,    {0} },
	{ MODKEY|ShiftMask,             XK_s,      togglesticky,   {0} },


/*// multi-monitor control */
	{ MODKEY,			XK_bracketright,       focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,		XK_bracketright,   tagmon,         {.i = -1 } },
	{ MODKEY,			XK_bracketleft,        focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,		XK_bracketleft,    tagmon,         {.i = +1 } },

/*// gaps control */
	{ MODKEY,					XK_minus,  incrgaps,       {.i = -3 } }, /* all */
	{ MODKEY,					XK_equal,  incrgaps,       {.i = +3 } },
	{ MODKEY|Mod1Mask,              XK_i,      incrigaps,      {.i = +1 } }, /* inner */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_o,      incrogaps,      {.i = +1 } }, /* outer */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_6,      incrihgaps,     {.i = +1 } }, /* inner horiz */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_7,      incrivgaps,     {.i = +1 } }, /* inner vert */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_8,      incrohgaps,     {.i = +1 } }, /* outer horiz */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_9,      incrovgaps,     {.i = +1 } }, /* outer vert */
	{ MODKEY|Mod1Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_equal,  togglegaps,     {0} },
	{ MODKEY|ShiftMask,             XK_minus,  defaultgaps,    {0} },


/*// tag keys */
	//TAGKEYS(                        XK_1,                      0)
	//TAGKEYS(                        XK_2,                      1)
	//TAGKEYS(                        XK_3,                      2)
	//TAGKEYS(                        XK_4,                      3)
	//TAGKEYS(                        XK_5,                      4)
	//TAGKEYS(                        XK_6,                      5)
	//TAGKEYS(                        XK_7,                      6)
	//TAGKEYS(                        XK_8,                      7)
	//TAGKEYS(                        XK_9,                      8)


/*// toggle statusbar pieces individually */
  { MODKEY|ControlMask,			XK_w,      togglebarcolor, {0} }, /* swaps fg/bg for tag+win */

  //screenshot
  { MODKEY,                       XK_p,  spawn,          {.v = screenshot } }, /* screenshot active window */
  { MODKEY|ShiftMask,             XK_p,  spawn,          {.v = screenshotfull } }, /* screenshot full screen */
};


/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask           button          function        argument */
#ifndef __OpenBSD__
	{ ClkWinTitle,          0,			Button2,	zoom,           {0} },
	{ ClkStatusText,        0,			Button1,	sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,			Button2,	sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,			Button3,	sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,			Button4,	sigstatusbar,   {.i = 4} },
	{ ClkStatusText,        0,			Button5,	sigstatusbar,   {.i = 5} },
	{ ClkStatusText,		ShiftMask,	Button1,	sigstatusbar,	{.i = 6} },
#endif

	{ ClkStatusText,        ShiftMask,	Button3,	spawn,          SHCMD("alacritty -e nvim ~/.local/src/dwmblocks/blocks.h") },
	{ ClkClientWin,			MODKEY,		Button1,	movemouse,      {0} }, /* left click */
	{ ClkClientWin,			MODKEY,		Button2,	defaultgaps,    {0} }, /* middle click */
	{ ClkClientWin,			MODKEY,		Button3,	resizemouse,    {0} }, /* right click */
	{ ClkClientWin,			MODKEY,		Button4,	incrgaps,       {.i = +1} }, /* scroll up */
	{ ClkClientWin,			MODKEY,		Button5,	incrgaps,       {.i = -1} }, /* scroll down */
	{ ClkTagBar,			0,			Button1,	view,           {0} },
	{ ClkTagBar,			0,			Button3,	toggleview,     {0} },
	{ ClkTagBar,			MODKEY,		Button1,	tag,            {0} },
	{ ClkTagBar,			MODKEY,		Button3,	toggletag,      {0} },
	{ ClkRootWin,			0,			Button2,	togglebar,      {0} }, /* hide bar */
};

