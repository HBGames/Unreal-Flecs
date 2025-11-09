// Copyright Hitbox Games, LLC. All Rights Reserved.

#include "FlecsEntity.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntity)

bool FFlecsEntity::ExportTextItem(FString& ValueStr, FFlecsEntity const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
	// String guarded by sentinels, don't use `"` because it can be used in the selector.
	TStringBuilder<256> StringBuilder;
	StringBuilder.Append(ToString());

	ValueStr += StringBuilder.ToString();
	return true;
}
