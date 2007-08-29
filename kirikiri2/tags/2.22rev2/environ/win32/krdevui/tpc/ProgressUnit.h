//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef ProgressUnitH
#define ProgressUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMainForm;
class TProgressForm : public TForm
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TProgressBar *ProgressBar;
	TLabel *CurrentFileLabel;
	TButton *CancelButton;
	TTimer *Timer;
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall TimerTimer(TObject *Sender);
private:	// ���[�U�[�錾
	bool Interrupted;
	bool Shutdown;
	bool AllSucceeded;
	TMainForm *MainForm;
	TStringList *FileList;
public:		// ���[�U�[�錾
	__fastcall TProgressForm(TComponent* Owner, TStringList *list);

	bool GetInterrupted() const {return Interrupted; }


	bool GetSucceeded() const { return AllSucceeded; }
protected:

	void __fastcall Process();
BEGIN_MESSAGE_MAP
	VCL_MESSAGE_HANDLER( WM_USER+1,  TMessage, WMStartProcess);
END_MESSAGE_MAP(TForm)
	void __fastcall WMStartProcess(TMessage &Msg);
};
//---------------------------------------------------------------------------
extern PACKAGE TProgressForm *ProgressForm;
//---------------------------------------------------------------------------
#endif