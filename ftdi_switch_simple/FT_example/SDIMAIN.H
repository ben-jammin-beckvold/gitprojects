//----------------------------------------------------------------------------
#ifndef SDIMainH
#define SDIMainH
//----------------------------------------------------------------------------
#include <vcl\ComCtrls.hpp>
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Dialogs.hpp>
#include <vcl\Menus.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
#include <vcl\Mask.hpp>
//----------------------------------------------------------------------------
#include <vcl\inifiles.hpp>
#include <vcl\registry.hpp>
#include "Serializer.h"
//----------------------------------------------------------------------------
class TSDIAppForm : public TForm
{
__published:
	TStatusBar *StatusBar;
    TBitBtn *ProgramBitBtn;
    TPanel *Panel1;
    TLabel *PassLabel;
    TLabel *FailLabel;
    TGroupBox *InfoGB;
    TLabel *ManuLabel;
    TLabel *DevIdLabel;
    TLabel *DescLabel;
    TCheckBox *DisableEEPromCB;
	void __fastcall ShowHint(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	BOOLEAN __fastcall Program(TObject *Sender);
	BOOLEAN __fastcall Test(TObject *Sender);
    void __fastcall ProgramBitBtnClick(TObject *Sender);

private:
    char InfoText[256];

public:
	virtual __fastcall TSDIAppForm(TComponent *AOwner);
};
//----------------------------------------------------------------------------
extern TSDIAppForm *SDIAppForm;
//----------------------------------------------------------------------------
#endif
