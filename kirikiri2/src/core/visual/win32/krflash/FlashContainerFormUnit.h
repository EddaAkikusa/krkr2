//---------------------------------------------------------------------------
// krflashmain.h ( part of KRFLASH.DLL )
// (c)2001-2004, W.Dee <dee@kikyou.info>
//---------------------------------------------------------------------------
#ifndef FlashContainerFormUnitH
#define FlashContainerFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ShockwaveFlashObjects_OCX.h"
#include <OleCtrls.hpp>
#include "..\krmovie.h"
#include "krflashmain.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFlashContainerForm : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TShockwaveFlash *Flash;
	TTimer *Timer;
	void __fastcall TimerTimer(TObject *Sender);
	void __fastcall FlashFSCommand(TObject *Sender, BSTR command, BSTR args);
private:	// ���[�U�[�錾
	RECT Rect;
	tTVPFlashOverlay * Overlay;
	HWND OwnerWindow;
public:		// ���[�U�[�錾
	__fastcall TFlashContainerForm(TComponent* Owner, tTVPFlashOverlay *overlay,
		HWND owner, const RECT &rect);
	
protected:
	void __fastcall CreateParams(Controls::TCreateParams &Params);
public:
	void __fastcall SetMovie(wchar_t *filename);
	void __fastcall Play();
	void __fastcall Stop();
	void __fastcall Pause();
	void __fastcall SetFlashVisible(bool b);
	tTVPVideoStatus __fastcall GetStatus();
};
//---------------------------------------------------------------------------
extern PACKAGE TFlashContainerForm *FlashContainerForm;
//---------------------------------------------------------------------------
#endif
