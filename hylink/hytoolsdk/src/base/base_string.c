#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "error_msg.h"
#include "base_api.h"

#define VERSION_MAX_LEN						18


extern char *base_strcpy(char *destin, char *source)
{
	return strcpy(destin, source);
}
extern char *base_strncpy(char *dest, char *src, int n)
{
	return strncpy(dest, src, n);
}

extern char *base_strcat(char *destin, char *source)
{
	return strcat(destin, source);
}

extern char *base_strchr(char *str, char c)
{
	return strchr(str, c);
}

extern char *base_strrchr(char *str, int c)
{
	return strrchr(str, c);
}

extern int base_strcmp(char *str1, char *str2)
{
	return strcmp(str1, str2);
}

extern int base_strncmp(char *str1, char *str2, int n)
{
	return strncmp(str1, str2, n);
}

extern int base_strcasecmp(char *str1, char *str2)
{
	return strcasecmp(str1, str2);
}

extern int base_strlen(char *str)
{
	return strlen(str);
}

extern char *base_strtok_r(char *str, char *delim, char **ptrptr)
{
	return strtok_r(str, delim, ptrptr);
}


extern int base_isprint(char c)
{
	return isprint(c);
}

extern int base_atoi(char *str)
{
	return atoi(str);
}
extern long int base_atol(char *str)
{
	return atol(str);
}

extern double base_atof(char *str)
{
	return atof(str);
}

extern unsigned long int base_strtoul(char *nptr, char **endptr, int base)
{
	return strtoul(nptr, endptr, base);
}

/*************************************************************
*����:	base_version_comp
*����:	pcNewVer: �µİ汾��
*		pcOldVer: �ɵİ汾��
*����:	1��ʾ�µİ汾�Ÿ��ھɵİ汾��, 0��ʾ�¾ɰ汾����ͬ,
*		-1��ʾ�µİ汾�ŵ��ھɵİ汾��,-2��ʾ�¾ɰ汾�Ÿ�ʽ����
*����:	�汾�űȽϽӿڣ��ýӿڴ�����¾ɰ汾�Ÿ�ʽҪ����ͬ
*************************************************************/
extern int
base_version_comp(char *pcNewVer, char *pcOldVer)
{
	/*��������*/
	int iRet = 0;
	int i = 0;
	int iCountNew = 0;
	int iCountOld = 0;
	char acNewVer[VERSION_MAX_LEN] = {0};
	char acOldVer[VERSION_MAX_LEN] = {0};
	char *apcNewVerNum[VERSION_MAX_LEN] = {0};
	char *apcOldVerNum[VERSION_MAX_LEN] = {0};
	char *pcNewVer2 = NULL;
	char *pcOldVer2 = NULL;
	char *pcTmp = NULL;
	char *pStrtokPtr = NULL;
	/*������ʼ��*/
	strncpy(acNewVer, pcNewVer, VERSION_MAX_LEN);
	strncpy(acOldVer, pcOldVer, VERSION_MAX_LEN);
	pcNewVer2 = acNewVer;
	pcOldVer2 = acOldVer;

	/*��������ȥ���������ַ�*/
	while(*pcNewVer2)
	{
		if(*pcNewVer2 > '9' || *pcNewVer2 < '0')
		{
			pcNewVer2++;
		}
		else
		{
			break;
		}
	}
	while(*pcOldVer2)
	{
		if(*pcOldVer2 > '9' || *pcOldVer2 < '0')
		{
			pcOldVer2++;
		}
		else
		{
			break;
		}
	}
	/*��������ȥ���������ַ�*/
	for(i = strlen(pcNewVer2) - 1; i >= 0; i--)
	{
		if(pcNewVer2[i] > '9' || pcNewVer2[i] < '0')
		{
			pcNewVer2[i] = '\0';
		}
		else
		{
			break;
		}
	}
	for(i = strlen(pcOldVer2) - 1; i >= 0; i--)
	{
		if(pcOldVer2[i] > '9' || pcOldVer2[i] < '0')
		{
			pcOldVer2[i] = '\0';
		}
		else
		{
			break;
		}
	}
	/*���ʣ���ַ������Ƿ���ڼȲ���"0-9"Ҳ����"."���ַ�*/
	pcTmp = pcNewVer2;
	while(*pcTmp)
	{
		if((*pcTmp > '9' || *pcTmp < '0') &&
			*pcTmp != '.')
		{
			iRet++;
		}
		pcTmp++;
	}
	if(iRet)
	{
		return -2;
	}

	pcTmp = pcOldVer2;
	while(*pcTmp)
	{
		if((*pcTmp > '9' || *pcTmp < '0') &&
			*pcTmp != '.')
		{
			iRet++;
		}
		pcTmp++;
	}
	if(iRet)
	{
		return -2;
	}

	/*�ж��¾ɰ汾���еĵ���Ƿ���ͬ*/
	pcTmp = pcNewVer2;
	while((apcNewVerNum[iCountNew]=strtok_r(pcTmp, ".", &pStrtokPtr))!= NULL)
	{
		iCountNew++;
		pcTmp = NULL;
	}
	pcTmp = pcOldVer2;
	while((apcOldVerNum[iCountOld]=strtok_r(pcTmp, ".", &pStrtokPtr))!= NULL)
	{
		iCountOld++;
		pcTmp = NULL;
	}
	if(iCountNew != iCountOld)
	{
		return -2;
	}

	/*�жϰ汾��*/
	for(i = 0; i < iCountOld; i++)
	{
		if(atoi(apcNewVerNum[i]) > atoi(apcOldVerNum[i]))
		{
			return 1;
		}
		else if(atoi(apcNewVerNum[i]) < atoi(apcOldVerNum[i]))
		{
			return -1;
		}
	}

	return 0;
}

