//---------------------------------------------------------------------------
// option information for kirikiri configurator
//---------------------------------------------------------------------------

#ifndef __OptionInfoH__
#define __OptionInfoH__

#include <windows.h>
#include "tp_stub.h"


//---------------------------------------------------------------------------
// GetOptionInfoString: returns option information string for kirikiri conf.
//---------------------------------------------------------------------------
static inline const wchar_t * GetOptionInfoString()
{
	if(GetACP() == 932) // 932 = Japan
	{
		return
L"�f�o�b�O:ROT�e�[�u���ւ̓o�^;���[�r�[�Đ�����"
L"ROT(Running Object Table)�֓o�^���邩�ǂ����̐ݒ�ł��B"
L"�u����v��I������ƁADirectX SDK�t����GraphEdit��p����"
L"���[�r�[�Đ��g���u���̉�͂��s�����Ƃ��ł��܂��B|"
L"movie_reg_rot|select,*no;������,yes;�͂�\n";
	}
	else
	{
		return
		// for other languages; currently only English information is available.
L"Debug:ROT registration;Whether to register into ROT(Running Object Table) when "
L"playbacking movies. Choosing 'Yes' enables you to inspect the trouble related with movies, "
L"using GraphEdit(comes from DirectX SDK).|"
L"movie_reg_rot|select,*no;No,yes;Yes\n";
	}
}


//---------------------------------------------------------------------------
// GetShouldRegisterToROT: returns whether to register the process into ROT
//---------------------------------------------------------------------------
static inline bool GetShouldRegisterToROT()
{
	static bool cached = false;
	static bool state = false;
	if(!cached)
	{
		tTJSVariant val;
		if(TVPGetCommandLine(TJS_W("-movie_reg_rot"), &val))
		{
			if(ttstr(val) == TJS_W("yes")) state = true;
		}
		cached = true;
	}
	return state;
}

#endif
