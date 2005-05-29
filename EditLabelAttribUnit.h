//---------------------------------------------------------------------------


#ifndef EditLabelAttribUnitH
#define EditLabelAttribUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "WaveView.h"
//---------------------------------------------------------------------------
class TEditLabelAttribFrame : public TFrame
{
__published:	// IDE �Ǘ��̃R���|�[�l���g
	TLabel *Label1;
	TEdit *LabelNameEdit;
	void __fastcall LabelNameEditExit(TObject *Sender);
	void __fastcall LabelNameEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall LabelNameEditChange(TObject *Sender);
private:	// ���[�U�[�錾
	TWaveView * FWaveView;
	int FLabelNum;
	bool InLoading;
	void __fastcall SetLabelNum(int num);

	void __fastcall AttribChanged();
	void __fastcall CommitChanges();
public:		// ���[�U�[�錾
	__fastcall TEditLabelAttribFrame(TComponent* Owner);

	__property TWaveView * WaveView = { read = FWaveView, write = FWaveView };
	__property int LabelNum = { read = FLabelNum, write = SetLabelNum };
};
//---------------------------------------------------------------------------
extern PACKAGE TEditLabelAttribFrame *EditLabelAttribFrame;
//---------------------------------------------------------------------------
#endif
