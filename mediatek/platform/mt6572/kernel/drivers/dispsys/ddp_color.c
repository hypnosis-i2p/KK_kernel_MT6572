#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/xlog.h>
#include <linux/spinlock.h>
#include <mach/mt_clkmgr.h>
#include <mach/mt_gpio.h>

#include "ddp_reg.h"
#include "ddp_path.h"
#include "ddp_color.h"
#include "ddp_debug.h"


extern unsigned char pq_debug_flag;

// initailize 
static DISP_PQ_PARAM g_Color_Param = 
{
u4SHPGain:0,
u4SatGain:0,
u4HueAdj:{3,3,3,3},
u4SatAdj:{0,0,0,0}
};

static DISP_PQ_PARAM g_Color_Cam_Param = 
{
u4SHPGain:0,
u4SatGain:0,
u4HueAdj:{3,3,3,3},
u4SatAdj:{0,0,0,0}
};

static DISP_PQ_PARAM g_Color_Gal_Param = 
{
u4SHPGain:0,
u4SatGain:0,
u4HueAdj:{3,3,3,3},
u4SatAdj:{0,0,0,0}
};

//COLOR_TUNING_INDEX : 10

/*
#define PURP_TONE_START    0 
#define PURP_TONE_END      2
#define SKIN_TONE_START    3
#define SKIN_TONE_END     10
#define GRASS_TONE_START  11
#define GRASS_TONE_END    16
#define SKY_TONE_START    17
#define SKY_TONE_END      19
*/
//initialize index (because system default is 0, need fill with 0x80)

static DISPLAY_PQ_T g_Color_Index =
{
GLOBAL_SAT   :
{0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80}, //0~10

PURP_TONE_S  :
{//hue 0~10
	{//0 disable  
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	}, 

	{//1
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	
	{//2
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	{//3
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	
	{//4
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	
	{//5
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},

	{//6
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
  // 7
	{
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	// 8
	{
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	// 9
	{
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	}
},
SKIN_TONE_S:
{
	{//0 disable  
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},

	{//1
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//2
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//3
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},

	{//4
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//5
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	{//6
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//7
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//8
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//9
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	}
},
GRASS_TONE_S:
{
	{//0 disable  
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},

	{//1
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//2
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}

	},

	{//3
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}

	},
	
	{//4
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}

	},

	{//5
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}

	},
	
	{//6
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}

	},

	{//7
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//8
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	},
	
	{//9
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
	}
},
SKY_TONE_S:
{
	{//0 disable
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	{//1
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}

	},
	{//2
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}

	},

	{//3
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}

	},
	{//4
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}

	},

	{//5
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	
	{//6
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	
	{//7
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	
	{//8
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	},
	
	{//9
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80},
		{0x80, 0x80, 0x80}
	}
},

PURP_TONE_H :
{ 
//hue 0~2 
    {0x80, 0x80, 0x80},//3 
    {0x80, 0x80, 0x80},//4 
    {0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80},//3 
    {0x80, 0x80, 0x80},//4 
    {0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80}
},
	
SKIN_TONE_H:
{
//hue 3~10
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},   
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
},
	
GRASS_TONE_H :
{
// hue 11~16
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	{0x80, 0x80, 0x80, 0x80, 0x80, 0x80}
},

SKY_TONE_H:
{
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80},
        {0x80, 0x80, 0x80}
}

};


static unsigned long g_Color_Window = 0;
static DISP_AAL_STATISTICS gHist;
static unsigned long gHistIndicator = 0;
spinlock_t gHistLock;


void disp_set_hist_readlock(unsigned long bLock)
{
    unsigned long flag;

    spin_lock_irqsave(&gHistLock , flag);

    gHistIndicator = bLock;

    spin_unlock_irqrestore(&gHistLock , flag);
}

int disp_get_hist(unsigned int * pHist)
{
    disp_set_hist_readlock(1);

    memcpy(pHist , gHist.histogram  , (LUMA_HIST_BIN*sizeof(unsigned int)));
    
    disp_set_hist_readlock(0);
    return 0;
}

DISP_AAL_STATISTICS * disp_get_hist_ptr()
{
    return &gHist;
}

void disp_update_hist()
{
    int i = 0;
    unsigned long flag;

    spin_lock_irqsave(&gHistLock , flag);

    if(~gHistIndicator)
    {
        for (i = 0; i < LUMA_HIST_BIN; i++)
        {
            gHist.histogram[i] = DISP_REG_GET(DISPSYS_COLOR_BASE + 0x0520 + i * 4);
        }

        gHist.ChromHist = DISP_REG_GET(DISPSYS_COLOR_BASE + 0x74C);

        DISP_REG_SET((DISPSYS_BLS_BASE + 0x1c) , 1);
        for (i = 0; i < BLS_HIST_BIN; i++)
        {
            gHist.BLSHist[i] = DISP_REG_GET(DISPSYS_BLS_BASE + 0x100 + i * 4);
        }
        DISP_REG_SET((DISPSYS_BLS_BASE + 0x1c) , 0);
    }

    spin_unlock_irqrestore(&gHistLock , flag);

}

void disp_color_set_window(unsigned int sat_upper, unsigned int sat_lower,
                           unsigned int hue_upper, unsigned int hue_lower)
{
    g_Color_Window = (sat_upper << 24) | (sat_lower << 16) | (hue_upper << 8) | (hue_lower);
}


void disp_onConfig_luma(unsigned long * luma)
{ 
    unsigned long u4Index;
    unsigned long u4Val;
    
    for(u4Index = 0 ; u4Index < 8; u4Index += 1)
    {
        u4Val = luma[u4Index*2] | luma[u4Index*2+1] << 16;
        DISP_REG_SET((DISPSYS_COLOR_BASE + 0x440 + (u4Index << 2)), u4Val);
    }
     DISP_REG_SET((DISPSYS_COLOR_BASE + 0x440 + (8 << 2)), luma[16]);  
}

/*
*g_Color_Param
*/

DISP_PQ_PARAM * get_Color_config()
{
    return &g_Color_Param;
}


DISP_PQ_PARAM * get_Color_Cam_config(void)
{
    return &g_Color_Cam_Param;
}

DISP_PQ_PARAM * get_Color_Gal_config(void)
{
    return &g_Color_Gal_Param;
}
/*
*g_Color_Index
*/

DISPLAY_PQ_T * get_Color_index()
{
    return &g_Color_Index;
}


void DpEngine_COLORonInit(void)
{
       //DDP_DRV_DBG("===================init COLOR =======================\n");


       spin_lock_init(&gHistLock);


       DISP_REG_SET(CFG_MAIN,(1<<29)); //color enable

       DISP_REG_SET((DISPSYS_COLOR_BASE + 0x420),0);

        
        DISP_REG_SET((DISPSYS_COLOR_BASE + 0xf00), 0x00000001); //color start

        //enable interrupt
        DISP_REG_SET((DISPSYS_COLOR_BASE + 0xf04), 0x00000007);

        //Set 10bit->8bit Rounding 
        DISP_REG_SET((DISPSYS_COLOR_BASE + 0xf0c), 0x333);



#if defined (MTK_AAL_SUPPORT)
        //config cboost
        DISP_REG_SET((DISPSYS_COLOR_BASE + 0x428), 0xFF402280);
        DISP_REG_SET((DISPSYS_COLOR_BASE + 0x42c), 0x00000000);
#endif
}



void DpEngine_COLORonConfig(unsigned int srcWidth,unsigned int srcHeight)
{
	 int index = 0;
    unsigned int u4Temp = 0;
    unsigned char h_series[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    //DDP_DRV_DBG("==========config COLOR g_Color_Param [%d %d %d %d %d %d %d %d %d]\n===============\n", g_Color_Param.u4SatGain, g_Color_Param.u4HueAdj[PURP_TONE], g_Color_Param.u4HueAdj[SKIN_TONE], g_Color_Param.u4HueAdj[GRASS_TONE], g_Color_Param.u4HueAdj[SKY_TONE], g_Color_Param.u4SatAdj[PURP_TONE], g_Color_Param.u4SatAdj[SKIN_TONE], g_Color_Param.u4SatAdj[GRASS_TONE], g_Color_Param.u4SatAdj[SKY_TONE]);

    DISP_REG_SET((DISPSYS_COLOR_BASE + 0xf50), srcWidth);  //wrapper width
    DISP_REG_SET((DISPSYS_COLOR_BASE + 0xf54), srcHeight); //wrapper height


    DISP_REG_SET((DISPSYS_COLOR_BASE + 0x40C),(0xFFFF)<<16);

    // enable R2Y/Y2R in Color Wrapper
    DISP_REG_SET((DISPSYS_COLOR_BASE + 0xf60), 1    );
    DISP_REG_SET((DISPSYS_COLOR_BASE +  0xfa0), 1    );


	//config parameter from customer folder by DpConfig

	if(g_Color_Param.u4SatGain>= COLOR_TUNING_INDEX ||
	   g_Color_Param.u4HueAdj[PURP_TONE] >= COLOR_TUNING_INDEX ||
	   g_Color_Param.u4HueAdj[SKIN_TONE] >= COLOR_TUNING_INDEX ||
	   g_Color_Param.u4HueAdj[GRASS_TONE] >= COLOR_TUNING_INDEX|| 
	   g_Color_Param.u4HueAdj[SKY_TONE] >= COLOR_TUNING_INDEX || 
	   g_Color_Param.u4SatAdj[PURP_TONE] >= COLOR_TUNING_INDEX ||
	   g_Color_Param.u4SatAdj[SKIN_TONE] >= COLOR_TUNING_INDEX ||
	   g_Color_Param.u4SatAdj[GRASS_TONE] >= COLOR_TUNING_INDEX|| 
	   g_Color_Param.u4SatAdj[SKY_TONE] >= COLOR_TUNING_INDEX )
	{
		//XLOGD("COLOR Tuning index range error !\n");
		return;
	}
   
   
	DISP_REG_SET(G_PIC_ADJ_MAIN_2, (0x200<<16) |g_Color_Index.GLOBAL_SAT[g_Color_Param.u4SatGain]);


	// Partial Saturation Function

	DISP_REG_SET(PARTIAL_SAT_GAIN1_0 , ( g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG1][0] | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG1][1] << 8 | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG1][2] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][0] << 24 )) ;
	DISP_REG_SET(PARTIAL_SAT_GAIN1_1 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][1] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][2] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][3] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][4] << 24 )) ;
	DISP_REG_SET(PARTIAL_SAT_GAIN1_2 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][5] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][6] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG1][7] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG1][0] << 24 )) ;
	DISP_REG_SET(PARTIAL_SAT_GAIN1_3 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG1][1] | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG1][2] << 8 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG1][3] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG1][4] << 24 )) ;
	DISP_REG_SET(PARTIAL_SAT_GAIN1_4 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG1][5] | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG1][0] << 8 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG1][1] << 16 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG1][2] << 24 )) ;
             
	DISP_REG_SET(PARTIAL_SAT_GAIN2_0 , ( g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG2][0] | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG2][1] << 8 | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG2][2] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][0] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_GAIN2_1 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][1] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][2] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][3] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][4] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_GAIN2_2 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][5] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][6] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG2][7] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG2][0] << 24 )) ;       
	DISP_REG_SET(PARTIAL_SAT_GAIN2_3 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG2][1] | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG2][2] << 8 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG2][3] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG2][4] << 24 )) ; 
	DISP_REG_SET(PARTIAL_SAT_GAIN2_4 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG2][5] | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG2][0] << 8 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG2][1] << 16 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG2][2] << 24 )) ;             
             
	DISP_REG_SET(PARTIAL_SAT_GAIN3_0 , ( g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG3][0] | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG3][1] << 8 | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SG3][2] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][0] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_GAIN3_1 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][1] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][2] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][3] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][4] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_GAIN3_2 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][5] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][6] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SG3][7] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG3][0] << 24 )) ;       
	DISP_REG_SET(PARTIAL_SAT_GAIN3_3 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG3][1] | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG3][2] << 8 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG3][3] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG3][4] << 24 )) ; 
	DISP_REG_SET(PARTIAL_SAT_GAIN3_4 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SG3][5] | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG3][0] << 8 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG3][1] << 16 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SG3][2] << 24 )) ;             
             
	DISP_REG_SET(PARTIAL_SAT_POINT1_0 , ( g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SP1][0] | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SP1][1] << 8 | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SP1][2] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][0] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_POINT1_1 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][1] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][2] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][3] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][4] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_POINT1_2 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][5] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][6] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP1][7] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP1][0] << 24 )) ;       
	DISP_REG_SET(PARTIAL_SAT_POINT1_3 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP1][1] | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP1][2] << 8 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP1][3] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP1][4] << 24 )) ; 
	DISP_REG_SET(PARTIAL_SAT_POINT1_4 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP1][5] | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SP1][0] << 8 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SP1][1] << 16 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SP1][2] << 24 )) ;             
             
	DISP_REG_SET(PARTIAL_SAT_POINT2_0 , ( g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SP2][0] | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SP2][1] << 8 | g_Color_Index.PURP_TONE_S[g_Color_Param.u4SatAdj[PURP_TONE]][SP2][2] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][0] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_POINT2_1 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][1] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][2] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][3] << 16 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][4] << 24 )) ;         
	DISP_REG_SET(PARTIAL_SAT_POINT2_2 , ( g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][5] | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][6] << 8 | g_Color_Index.SKIN_TONE_S[g_Color_Param.u4SatAdj[SKIN_TONE]][SP2][7] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP2][0] << 24 )) ;       
	DISP_REG_SET(PARTIAL_SAT_POINT2_3 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP2][1] | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP2][2] << 8 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP2][3] << 16 | g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP2][4] << 24 )) ; 
	DISP_REG_SET(PARTIAL_SAT_POINT2_4 , ( g_Color_Index.GRASS_TONE_S[g_Color_Param.u4SatAdj[GRASS_TONE]][SP2][5] | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SP2][0] << 8 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SP2][1] << 16 | g_Color_Index.SKY_TONE_S[g_Color_Param.u4SatAdj[SKY_TONE]][SP2][2] << 24 )) ;   


	
    for (index = 0; index < 3; index++)
    {
		h_series[index+PURP_TONE_START] = g_Color_Index.PURP_TONE_H[g_Color_Param.u4HueAdj[PURP_TONE]][index];
    }

    for (index = 0; index < 8; index++)
    {
		h_series[index+SKIN_TONE_START] = g_Color_Index.SKIN_TONE_H[g_Color_Param.u4HueAdj[SKIN_TONE]][index];
    }

    for (index = 0; index < 6; index++)
    {
		h_series[index+GRASS_TONE_START] = g_Color_Index.GRASS_TONE_H[g_Color_Param.u4HueAdj[GRASS_TONE]][index];
    }

    for (index = 0; index < 3; index++)
    {
		h_series[index+SKY_TONE_START] = g_Color_Index.SKY_TONE_H[g_Color_Param.u4HueAdj[SKY_TONE]][index];
    }

    for (index = 0; index < 5; index++)
    {
        u4Temp = (h_series[4 * index]) +
                 (h_series[4 * index + 1] << 8) +
                 (h_series[4 * index + 2] << 16) +
                 (h_series[4 * index + 3] << 24);
        DISP_REG_SET(LOCAL_HUE_CD_0 + 4 * index, u4Temp);
    }

    // color window
    //FIXME! 6582 mark AAL
    //DISP_REG_SET((DISPSYS_COLOR_BASE+0x740), g_Color_Window);
}


void DpEngine_COLORonReset(void)
{
    unsigned int regValue;

    regValue = DISP_REG_GET((DISPSYS_COLOR_BASE + 0xF00));
    DISP_REG_SET((DISPSYS_COLOR_BASE + 0xF00), regValue | 0x100);
    DISP_REG_SET((DISPSYS_COLOR_BASE + 0xF00), regValue & (~0x100));
}


void DpEngine_COLORonEnableIrq(unsigned int value)
{
    DISP_REG_SET((DISPSYS_COLOR_BASE + 0xF08), 0x00);
    DISP_REG_SET((DISPSYS_COLOR_BASE + 0xF04), value);
}





