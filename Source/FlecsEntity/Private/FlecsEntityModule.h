// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "IFlecsEntityModule.h"

class FFlecsEntityModule : public IFlecsEntityModule
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
