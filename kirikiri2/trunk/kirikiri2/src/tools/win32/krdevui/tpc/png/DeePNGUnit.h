//---------------------------------------------------------------------------
/*
	fgconvert   (graphic converter for transparent images)
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef DeePNGUnitH
#define DeePNGUnitH

#include <vcl.h>

//---------------------------------------------------------------------------
extern void __fastcall SetGrayscalePalette(Graphics::TBitmap *dib, int bit);
//---------------------------------------------------------------------------
// ��O�N���X
class EDeePNG : public Exception
{
public:
	__fastcall EDeePNG(const System::AnsiString Msg) : Sysutils::Exception(
		Msg) { }
	__fastcall EDeePNG(const System::AnsiString Msg, const System::TVarRec
		* Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	__fastcall EDeePNG(int Ident, Extended Dummy) : Sysutils::Exception(
		Ident, Dummy) { }
	__fastcall EDeePNG(int Ident, const System::TVarRec * Args, const
		int Args_Size) : Sysutils::Exception(Ident, Args, Args_Size) { }
	__fastcall EDeePNG(const System::AnsiString Msg, int AHelpContext
		) : Sysutils::Exception(Msg, AHelpContext) { }
	__fastcall EDeePNG(const System::AnsiString Msg, const System::TVarRec
		* Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg,
		 Args, Args_Size, AHelpContext) { }
	__fastcall EDeePNG(int Ident, int AHelpContext) : Sysutils::
		Exception(Ident, AHelpContext) { }
	__fastcall EDeePNG(int Ident, const System::TVarRec * Args
		, const int Args_Size, int AHelpContext) : Sysutils::Exception(Ident,
		 Args, Args_Size, AHelpContext) { }

	__fastcall virtual ~EDeePNG(void) { }

};
//---------------------------------------------------------------------------
// TDeePNG �N���X
class TDeePNG : public Graphics::TBitmap
{
private:
protected:

public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	__fastcall TDeePNG(void);
	__fastcall ~TDeePNG(void);


	// Stream I/O  �̃I�[�o�[���C�h
	void __fastcall LoadFromStream(Classes::TStream * Stream);
	void __fastcall SaveToStream(Classes::TStream * Stream);

public:
};
//---------------------------------------------------------------------------
#endif
