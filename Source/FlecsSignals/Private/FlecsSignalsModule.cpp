// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "CoreMinimal.h"
#include "FlecsSignalTypes.h"
#include "IFlecsSignalsModule.h"

DEFINE_LOG_CATEGORY(LogFlecsSignals)

class FFlecsSignalsModule : public IFlecsSignalsModule
{
};

IMPLEMENT_MODULE(FFlecsSignalsModule, FlecsSignals)
