// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#include "StructUtils/StructUtilsMacros.h"

#ifndef WITH_FLECSENTITY_DEBUG
#define WITH_FLECSENTITY_DEBUG (!(UE_BUILD_SHIPPING || UE_BUILD_SHIPPING_WITH_EDITOR || UE_BUILD_TEST) && WITH_STRUCTUTILS_DEBUG && 1)
#endif
