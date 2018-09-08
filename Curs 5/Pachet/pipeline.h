#ifndef __PIPELINE_H__
#define __PIPELINE_H__

typedef void(*CALLBACK_SURUB)();
typedef void(*CALLBACK_PIULITA)();
typedef void(*CALLBACK_PRODUS)();

typedef struct __InitParams
{
	CALLBACK_SURUB pfnSurub;
	CALLBACK_PIULITA pfnPiulita;
	CALLBACK_PRODUS pfnProdus;

}InitParams;

typedef enum 
{ 
	SURUB,
	PIULITA,
} ELEMENT;

typedef int (*INITPIPELINE)(InitParams * pInitParams);
typedef int (*GENERATEELEMENT)(ELEMENT elem);
typedef int (*UNINITPIPELINE)();

int InitPipeline(InitParams * pInitParams);
int GenerateElement(ELEMENT elem);
int UninitPipeline();

#endif