#include <linux/module.h>
#include "hi_type.h"
#include "vdec_exp.h"
#include "hi_osal.h"

extern int TDE_DRV_ModInit(void);
extern void TDE_DRV_ModExit(void);
HI_U32 g_u32TdeBuf = 0x20000;
module_param(g_u32TdeBuf, uint, S_IRUGO);
bool g_bResizeFilter = true;
module_param(g_bResizeFilter, bool, S_IRUGO);


static int __init tde_mod_init(void){
    if(HI_SUCCESS != TDE_DRV_ModInit())
    {
        osal_printk("load tde.ko for HI3516EV200...FAILED!\n");
        return -1;
    }
    osal_printk("load tde.ko for HI3516EV200...OK!\n");
    return 0;
}
static void __exit tde_mod_exit(void){
    TDE_DRV_ModExit();
}

module_init(tde_mod_init);
module_exit(tde_mod_exit);

MODULE_LICENSE("Proprietary");
