#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm_ive.h"
#include "sample_ive_main.h"

static char **s_ppChCmdArgv = NULL;

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
#ifndef __HuaweiLite__
HI_VOID SAMPLE_IVE_HandleSig(HI_S32 s32Signo)
{
    signal(SIGINT,SIG_IGN);
    signal(SIGTERM,SIG_IGN);

    if (SIGINT == s32Signo || SIGTERM == s32Signo)
    {
        switch (*s_ppChCmdArgv[1])
        {
            case '0':
                {
                    SAMPLE_IVE_Od_HandleSig();
                }
                break;
            case '1':
                {
                   SAMPLE_IVE_Canny_HandleSig();
                }
                break;
            case '2':
                {
                    SAMPLE_IVE_Gmm2_HandleSig();
                }
                break;
            case '3':
                {
                    SAMPLE_IVE_TestMemory_HandleSig();
                }
                break;
            case '4':
                {
                    SAMPLE_IVE_Sobel_HandleSig();
                }
                break;
            case '5':
                {
                    SAMPLE_IVE_St_Lk_HandleSig();
                }
                break;
            default :
                {
                }
                break;
        }

        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}
#endif
/******************************************************************************
* function : show usage
******************************************************************************/
HI_VOID SAMPLE_IVE_Usage(HI_CHAR* pchPrgName)
{
    printf("Usage : %s <index> [complete] [encode] [vo]\n", pchPrgName);
    printf("index:\n");
    printf("\t 0)Occlusion detected.(VI->VPSS->IVE->VO_HDMI).\n");
    printf("\t 1)Canny,<complete>:0, part canny;1,complete canny.(FILE->IVE->FILE).\n");
    printf("\t 2)Gmm2.(FILE->IVE->FILE).\n");
    printf("\t 3)MemoryTest.(FILE->IVE->FILE).\n");
    printf("\t 4)Sobel.(FILE->IVE->FILE).\n");
    printf("\t 5)St Lk.(FILE->IVE->FILE).\n");


}

/******************************************************************************
* function : ive sample
******************************************************************************/
#ifdef __HuaweiLite__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32ChipId = 0;
    s32Ret = HI_MPI_SYS_GetChipId(&u32ChipId);
    if(HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("Get chipId failed\n");
        return HI_FAILURE;
    }
    if (argc < 2)
    {
        SAMPLE_IVE_Usage(argv[0]);
        return HI_FAILURE;
    }
    s_ppChCmdArgv = argv;
#ifndef __HuaweiLite__
    signal(SIGINT, SAMPLE_IVE_HandleSig);
    signal(SIGTERM, SAMPLE_IVE_HandleSig);
#endif

    switch (*argv[1])
    {
        case '0':
            {
                if(u32ChipId == HI3516E_V200 || u32ChipId == HI3518E_V300)
                {
                    SAMPLE_PRT("%x chip can't  support Occlusion detected.\n", u32ChipId);
                    return HI_FAILURE;
                }
                else
                {
                    SAMPLE_IVE_Od();
                }
            }
            break;  
        case '1':
            {
                if ((argc < 3) || (('0' != *argv[2]) && ('1' != *argv[2])))
                {
                    SAMPLE_IVE_Usage(argv[0]);
                    return HI_FAILURE;
                }
                SAMPLE_IVE_Canny(*argv[2]);
            }
            break;
        case '2':
            {
                SAMPLE_IVE_Gmm2();
            }
            break;
        case '3':
            {
                SAMPLE_IVE_TestMemory();
            }
            break;
        case '4':
            {
                SAMPLE_IVE_Sobel();
            }
            break;
        case '5':
            {
                SAMPLE_IVE_St_Lk();
            }
            break;
        default :
            {
                SAMPLE_IVE_Usage(argv[0]);
            }
            break;
    }

    return 0;

}



