//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------
USEFORM("SDIMain.cpp", SDIAppForm);
USERES("sdiapp.res");
USEUNIT("ftusbdev.cpp");
USEFILE("copt.h");
USEUNIT("test_bm.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application->Initialize();
	Application->Title = "ftdi_st";
         Application->HelpFile = "C:\\Program Files\\Borland\\CBuilder\\IMAGES\\ICONS\\Ftdi.ico";
		Application->CreateForm(__classid(TSDIAppForm), &SDIAppForm);
         Application->Run();

	return 0;
}
//---------------------------------------------------------------------
