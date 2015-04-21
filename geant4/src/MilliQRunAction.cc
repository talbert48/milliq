
#include "MilliQRunAction.hh"
#include "MilliQRecorderBase.hh"


MilliQRunAction::MilliQRunAction(MilliQRecorderBase* r) : fRecorder(r) {}


MilliQRunAction::~MilliQRunAction() {}


void MilliQRunAction::BeginOfRunAction(const G4Run* aRun){
  if(fRecorder)fRecorder->RecordBeginOfRun(aRun);
}


void MilliQRunAction::EndOfRunAction(const G4Run* aRun){
  if(fRecorder)fRecorder->RecordEndOfRun(aRun);
}
