
#include "G4UserRunAction.hh"

#ifndef MilliQRunAction_h
#define MilliQRunAction_h 1

class MilliQRecorderBase;

class MilliQRunAction : public G4UserRunAction
{
  public:

    MilliQRunAction(MilliQRecorderBase*);
    virtual ~MilliQRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

  private:

    MilliQRecorderBase* fRecorder;
};

#endif
