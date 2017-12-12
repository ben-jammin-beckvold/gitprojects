//----------------------------------------------------------------------------
#ifndef SerialiserH
#define SerialiserH
//----------------------------------------------------------------------------
#define MAX_SECTION_NAME_LENGTH	128

// program states
typedef enum {
	ProgramState,
    InputState,
    AutoState,
    MultiProgramState
} TSerialiserState;
extern const String iniFile;

#define PROGRAM_STATE(State)    ((State) == ProgramState) || ((State) == MultiProgramState)
//----------------------------------------------------------------------------
#endif