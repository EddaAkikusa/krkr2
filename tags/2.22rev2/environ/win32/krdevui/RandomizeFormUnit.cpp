//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <time.h>

#define DISTANCE   1000i64
#define MIN_TIME 3000

#include "RandomizeFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRandomizeForm *RandomizeForm;
//---------------------------------------------------------------------------
__fastcall TRandomizeForm::TRandomizeForm(TComponent* Owner, prng_state *state)
	: TForm(Owner)
{
	State = state;
	GetCursorPos(&PrevMousePos);
	Distance = 0;
	StartTick = GetTickCount();
}
//---------------------------------------------------------------------------
void __fastcall TRandomizeForm::TimerTimer(TObject *Sender)
{
	// Add entropy noise

	DWORD tick = GetTickCount();

	yarrow_add_entropy(	(const unsigned char*)&tick, sizeof(tick), State);

	POINT pos;
	GetCursorPos(&pos);

	MEMORYSTATUS status;
	GlobalMemoryStatus(&status);

	time_t current;
	time(&current);

	yarrow_add_entropy(	(const unsigned char*)&current, sizeof(current), State);
	yarrow_add_entropy(	(const unsigned char*)&pos, sizeof(pos), State);
	yarrow_add_entropy(	(const unsigned char*)&Distance, sizeof(Distance), State);
	yarrow_add_entropy(	(const unsigned char*)&status, sizeof(status), State);

	Distance += (pos.x - PrevMousePos.x)*(pos.x - PrevMousePos.x);
	Distance += (pos.y - PrevMousePos.y)*(pos.y - PrevMousePos.y);

	PrevMousePos = pos;

	if(Distance > DISTANCE*DISTANCE && (tick - StartTick) > MIN_TIME)
		ModalResult = mrOk;
}
//---------------------------------------------------------------------------
bool RandomizePRNG(prng_state *state)
{
	// initialize random generator
	errno = register_prng(&yarrow_desc);
	if(errno != CRYPT_OK) throw Exception(error_to_string(errno));

	errno = yarrow_start(state);
	if(errno != CRYPT_OK) throw Exception(error_to_string(errno));

	TRandomizeForm *form = new TRandomizeForm(Application, state);
	int result = form->ShowModal();
	delete form;

	if(result != mrOk) return false;

	errno = yarrow_ready(state);
	if(errno != CRYPT_OK) throw Exception(error_to_string(errno));

	return true;
}
//---------------------------------------------------------------------------
static BOOL CALLBACK EnumWindowsProc(HWND wnd, LPARAM lp)
{
	prng_state *state = (prng_state*)lp;
	RECT r;
	GetWindowRect(wnd, &r);
	char tmp[256];
	GetWindowText(wnd, tmp, 255);
	yarrow_add_entropy(	(const unsigned char*)&r, sizeof(r), state);
	yarrow_add_entropy(	(const unsigned char*)tmp, sizeof(tmp), state);

	return TRUE;
}
//---------------------------------------------------------------------------
bool RandomizePRNGSimple(prng_state *state)
{
	// initialize random generator, in simple way
	errno = register_prng(&yarrow_desc);
	if(errno != CRYPT_OK) throw Exception(error_to_string(errno));

	errno = yarrow_start(state);
	if(errno != CRYPT_OK) throw Exception(error_to_string(errno));

	DWORD tick = GetTickCount();
	yarrow_add_entropy(	(const unsigned char*)&tick, sizeof(tick), state);

	POINT pos;
	GetCursorPos(&pos);

	MEMORYSTATUS status;
	GlobalMemoryStatus(&status);

	time_t current;
	time(&current);

	yarrow_add_entropy(	(const unsigned char*)&current, sizeof(current), state);
	yarrow_add_entropy(	(const unsigned char*)&pos, sizeof(pos), state);
	yarrow_add_entropy(	(const unsigned char*)&status, sizeof(status), state);

	EnumWindows((WNDENUMPROC)EnumWindowsProc, (LPARAM)state);

	tick = GetTickCount();
	yarrow_add_entropy(	(const unsigned char*)&tick, sizeof(tick), state);

	errno = yarrow_ready(state);
	if(errno != CRYPT_OK) throw Exception(error_to_string(errno));

	return true;
}
//---------------------------------------------------------------------------
