/*
** globals.c
*/

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240
#define SCREEN_BPP      16

int screen_width = SCREEN_WIDTH;	/* physical width */
int screen_height = SCREEN_HEIGHT;	/* physical height */
int screen_bpp = SCREEN_BPP;		/* bits per pixel */
int tiawidth = 320;		/* # of pixels per scanline generated by TIA engine */
int pixelspread = 1;	/* how many times to replicate the tia pixels */
int scanlinespread = 1;	/* how many display lines per scanline */
int lines2draw = 1;		/* how many display lines to draw */

/* controller code definitions */
#define JS 0	/* joystick */
#define PC 1	/* paddle controller */
#define KP 2	/* keypad */
#define DC 3	/* driving controller */
#define LG 4	/* lightgun */
#define CM 5	/* Compumate keyboard */
#define KV 6	/* Kid Vid tape player */
#define ML 7	/* Mindlink */
#define ST 8	/* ST mouse / CX-80 TRAK Ball */
#define TB 9	/* CX-22 TRAK Ball */
#define AM 10	/* Amiga mouse */
#define NC 11	/* no controller connected */

int MessageCode=0;		/* gets message code for final output */
char msg[1000];			/* message buffer */
char stat_msg[100];		/* status message buffer */
int status_timer = -1;	/* Counts down once per frame while status is shown,
						   status disappears when it's -1 again. */

int CartSize=0;			/* gets cart size */

db CartRom[0x8000f];
db PCXPalette[384];		/* palette information goes here */

db *ScreenBuffer;			/* pointer to screen buffer (set to RealScreenBuffer 1-4) */
db *ScreenBufferPrev;		/* pointer to screen buffer for previous frame (RSB 1-4)*/
db *PrevScreenBuffer;		/* previous pointer to screen buffer */
db *PrevScreenBufferPrev; /* previous pointer to previous screen buffer */

dd LinesInFrame;		/* # of lines in last frame */
dd PrevLinesInFrame;	/* # of lines in frame before that */
dd VBlankOff;			/* a valid VBlankOff was detected here */
dd VBlankOn;			/* a valid VBlankOn was detected here */
dd MaxLines;			/* # of lines in this video mode */
dd BailoutLine;			/* auto-sync occurs if game goes past this line */

dd SC_StartAddress;		/* supercharger start address */
db SC_ControlByte;		/* supercharger control byte */

dd crc;					/* holds accumulated CRC */

/* Masks (used for disabling graphics objects) */
db p0_mask = 0xff;
db p1_mask = 0xff;

db m0_mask = 0xff;
db m1_mask = 0xff;
db bl_mask = 0xff;

db pf_mask = 0xff;


#define KEYTABLESIZE 512

db KeyTable[KEYTABLESIZE];	/* event handler should tell z26 about keystrokes here */
key_data_t keys;

/* SDL event handler should tell z26 about joystick states here */
int JoystickAxis[16][6];
db JoystickButton[16][32];
db Stelladaptor[16];	/* is joystick a Stealladaptor */

db ShowFPS;
db SoundQ[65537];		/* sound queue */
dd SQ_Max = 4096;		/* 3072 */

/* make the output buffers big enough for 500 scanlines with tiawidth of 320 */

db RealScreenBuffer1[320*501];
/*
db RealScreenBuffer2[320*501];
db RealScreenBuffer3[320*501];
db RealScreenBuffer4[320*501];

dd texture_buffer[1024*1024];
*/

db srv_screen_fb[SCREEN_WIDTH * SCREEN_HEIGHT];
gfx_loadimg_t srv_screen;
unsigned short srv_screen_pal[256];
unsigned char srv_screen_handle;

/* paddle charges (can be 1-240) */

#define TRIGMIN 1
#define TRIGMAX 240

dd ChargeTrigger0[4] = {TRIGMAX/2, TRIGMAX/2, TRIGMAX/2, TRIGMAX/2};

#define CHARGEMAX 0x800000;	/* don't charge capacitors over this value (MAIN.ASM)*/
dd ChargeCounter = 0;		/* # of lines capacitors have been charging */

db InputLatch[2] = {0, 0};	/* INPT4 + INPT5 */

db CM_Collumn = 0;			/* Compumate key collumn to read - gets set by bankswitching */

int ExitEmulator;			/* leave the emulator when ESC or backslash are pressed */
int GamePaused;				/* toggle to pause emulator */
int GameReallyPaused;		/* ... it's a little bit complicated by the GUI */

int ResetEmulator = 0;
int StartInGUI = 0;
int LaunchedFromCommandline = 0;

//time_t MyTime;				/* structure used by the timer function to return 'Seconds' */
int Seconds;				/* seconds since 1.1.1970 - used to randomize RIOT timer */


/*
** The following variables are typically set by command line switches.
** They are reinitialized at startup and from the GUI.
** The GUI reinitializes them before reading command line parms.
*/

int UserLeftController;		/* -)  user specified left controller */
int LeftController;			/* controller in left port */

int UserRightController;		/* -(  user specified right controller */
int RightController;			/* controller in Right port */

int UserBankswitch = 0;			/* -gN user specified bankswitch type */
int BSType = 0;					/* override bankswitching type *EST* */

int UserPaletteNumber;		/* -cN user specified palette number */
int GamePaletteNumber;		/* game specific palette */
int PaletteNumber;			/* 0 = NTSC, 1 = PAL */

int UserDepth;				/* user specified Depthescence */
int Depth;					/* Depthescence */

int UserAllowAll4;			/* user specified allow all 4 directions on the joystick simultaniously */
int AllowAll4;				/* allow all 4 directions on the joystick simultaniously */

int UserSwapPortsFlag;		/* user specified port 0 to be swapped with port 1 */
int SwapPortsFlag;			/* controller port 0 to be swapped with port 1 */

int VideoMode;				/* default video mode */
int FullScreen;				/* run game full screen */
int Narrow;					/* width adjustment */
int Tall;					/* height adjustment */
int SimColourLoss;			/* simulate PAL colour loss *EST* */
int DoInterlace;			    /* simulate interlaced display *EST* */
int DoScanline;				/* do scanlines effect */

int GameOffset;				/* game specific vertical offset */
int CFirst;					/* first game line to display (zero has VBlank trigger a new frame) */
int OldCFirst;				/* remember original CFirst (for homing the display) */

db quiet;					/* set if we want no sound */
int dsp;						/* do digital signal processing */

int UserP0Diff;				/* user specified player zero difficulty */
int UserP1Diff;				/* user specified player one difficulty */
int UserConsoleBW;			/* user specified console forced to B/W */

db IOPortA;					/* IO Port A (joystick) */
db IOPortA_Controllers;		/* remember controller data throughout frame */
db IOPortA_UnusedBits;		/* bits unused by selected controller = 1 */
db IOPortA_Write;			/* value written to SWCHA */
db IOPortB;					/* IO Port B (switches) */

int UserPaddleSensitivity;
int PaddleSensitivity;		/* keyboard sensitivity for paddle emulation */

int UserTraceCount;
int TraceCount;				/* gets trace count */
int OldTraceCount;			/* for saving trace count if we turn off tracing */

int UserTraceEnabled;
int TraceEnabled;			/* if trace is enabled */

int UserMouseBaseX;
int UserMouseBaseY;
int MouseBaseX;				/* paddle to emulate with mouse X axis (0ffH = joystick) *EST* */
int MouseBaseY;				/* paddle to emulate with mouse Y axis (0ffH = joystick) *EST* */

char UserLightgun;
char UserLGadjust;
int Lightgun;				/* emulate lightgun and adjust horizontally *EST* */
int LGadjust;				/* adjust lightgun vertically *EST* */

int ShowLineCount;			/* display stats on game exit */
int theme;					/* color theme for GUI */
db KidVid;					/* ID byte on tapes for KidVid game *EST* */
db KidVidTape;				/* tape number to be played *EST* */
int MouseRude;				/* warp the mouse when running in a window */
int GrabInput; 				/* Grab keyboard/mouse? */
int PaddleAdjust;			/* wait for N lines before starting to evaluate paddle charge */

int UserJoystickEnabled;		/* user wants the joystick enabled */
int JoystickEnabled;			/* allow PC joystick input for VCS controllers with multiple PC inputs */
int MouseEnabled;			/* allow PC mouse input for VCS controllers with multiple PC inputs */
int KeyboardEnabled;			/* allow PC keyboard input for VCS controllers with multiple PC inputs */
int StelladaptorEnabled;		/* allow PC Stelladaptor input for VCS controllers with multiple PC inputs */

/*
** reinitialize the above variables
*/

void def_LoadDefaults(void)
{
	UserBankswitch = 0xff;
	BSType = 0;

	PaletteNumber = 0xff;		// actual palette
	GamePaletteNumber = 0xff;	// game specified palette override
	UserPaletteNumber = 0xff;	// user specified palette override

	UserLeftController = 0xff;
	LeftController=0xff;

	UserRightController = 0xff;
	RightController=0xff;

	UserAllowAll4 = 0xff;
	AllowAll4 = 0;

	UserSwapPortsFlag = 0xff;
	SwapPortsFlag = 0;
	
	UserP0Diff = 0;			// default difficulty is easy
	UserP1Diff = 0;
	UserConsoleBW = 0;		// default to color console

	GameOffset = 0;
	theme = 0x50;			// aqua
	VideoMode = 0xff;
	FullScreen = 1;
	Narrow = 0;
	Tall = 0;
	CFirst = 0xffff;
	quiet = 0;
	IOPortA = 0xff;
	IOPortA_Controllers = 0xff;
	IOPortA_UnusedBits = 0xff;
	IOPortB = 0x0b;			// 0000 1011
	dsp = 1;
	UserPaddleSensitivity = 0;
	PaddleSensitivity = 0;
	UserTraceCount = 0;
	TraceCount = 0;
	UserTraceEnabled = 0;
	TraceEnabled = 0;
	OldTraceCount = 0;
	
	UserMouseBaseX = 0xff;
	UserMouseBaseY = 0xff;
	MouseBaseX = 0xff;
	MouseBaseY = 0xff;
	
	SimColourLoss = 0;
	UserLightgun = 0;
	UserLGadjust = 5;
	Lightgun = 0;
	LGadjust = 5;
	ShowLineCount = 0;
	UserDepth = 60;
	Depth = 60;
	KidVid = 0;
	KidVidTape = 0;
	MaxLines = 256 ;
	DoInterlace = 0;
	DoScanline = 0;
	MouseRude = 0;
	GrabInput = 0;
	ExitEmulator = 0;
	GamePaused = 0;
	GameReallyPaused = 0;
	StartInGUI = 0;
	OldCFirst = 0;
	PaddleAdjust = 0;

	UserJoystickEnabled = 1;
	JoystickEnabled = 1;
	MouseEnabled = 1;
	KeyboardEnabled = 1;
	StelladaptorEnabled = 1;

	Seconds = get_time(); //time(&MyTime);	/* randomize RIOT timer (in RIOT.ASM)*/
}

// C-engine specific

void (* ReadAccess[0x10000])(void);
void (* WriteAccess[0x10000])(void);
void (* TIARIOTReadAccess[0x1000])(void);
void (* TIARIOTWriteAccess[0x1000])(void);

db RiotRam[128];	/* RIOT RAM (must be zero initially) */
db TIA[64];			/* TIA registers (also zeroed) */
db Ram[32768];		/* Extra RAM (used or not depends on bankswitch scheme) */

int Frame = 0;		/* Frame counter */
int PrevFrame = 0;	/* Previous value of frame counter */

int VBlanking = 0;	/* 0 if vblanking, -1 otherwise */
int VBlank = 0;		/* VBlank flag */
int VSyncFlag = 0;	/* VSync flag */

int ScanLine = 0;	/* Current scan line */
int OurBailoutLine = 0; /* Initial bailout line (fine tune if exceeded) */

db WByte = 0;		/* byte to write */

dw *DisplayPointer; /* should init to RealScreenBuffer 
					   this is 16-bit because emulator core stores
					   2 bytes at a time most of the time.*/

#define CYCLESPERSCANLINE 76
#define TRIGMIN 1

db DataBus;			/* state of the data bus - used for  moving data to and from the CPU emu */
dw AddressBus;		/* state of the address bus - used for read and write accesses */
void (* ReadROMAccess)(void);	/* the bankswitching read handlers will be called though this */
void (* WriteROMAccess)(void);	/* the bankswitching write handlers will be called through this */

/* variables from CPU emulation - used in tracing too */

dw reg_pc = 0;		/*  program counter */
db reg_sp = 0;		/*  stack pointer */
db reg_a = 0;		/*  accumulator (stored in AL) */
db flag_C = 0;		/*  carry bit   (stored in AH) */
db reg_x = 0;		/*  x register */
db reg_y = 0;		/*  y register */
db flag_Z = 0;		/*  zero test value (Z set when 0) */
db flag_N = 0;		/*  sign test value (N set when negative) */
db flag_D = 0;		/*  decimal flag */
db flag_V = 0;		/*  overflow flag */
db flag_I = 0;		/*  interrupt disable flag */
db flag_B = 0;		/*  break flag */
db RCycles = 0;		/*  cycles per instruction */
db RClock = 0;		/*  clock cycles */

db TriggerWSYNC = 0;	/* TIA tells CPU to pause on next read cycle */


/* 
** Init C Variables every time emulator() is called.
**
** called from init.asm
*/

void InitCVars(void)
{
	int i;

	VBlankOff=0;
	VBlankOn=0;
	LinesInFrame=262;
	BailoutLine=500;	/* 320, 360, 380 */
	for (i=0; i<KEYTABLESIZE; i++) KeyTable[i] = 0;
	p0_mask = 0xff;
	p1_mask = 0xff;
	m0_mask = 0xff;
	m1_mask = 0xff;
	pf_mask = 0xff;
	
	memset(RiotRam, 0, sizeof(RiotRam));
	memset(TIA, 0, sizeof(TIA));
	memset(Ram, 0, sizeof(Ram));
	
	Frame = 0;
	PrevFrame = 0;
	VBlanking = -1;		/* 0 if vblanking, -1 otherwise */
	VBlank = 0;			/* VBlank flag */
	VSyncFlag = 0;		/* VSync flag */

	ScanLine = 1;		/* Current scan line */
	OurBailoutLine = 500; /* Initial bailout line (fine tune if exceeded) */
	
	WByte = 0;

	DisplayPointer = (dw*) ScreenBuffer;
}


/**
	z26 is Copyright 1997-2011 by John Saeger and contributors.  
	z26 is released subject to the terms and conditions of the 
	GNU General Public License Version 2 (GPL).
	z26 comes with no warranty.
	Please see COPYING.TXT for details.
*/
