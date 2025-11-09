// Copyright Hitbox Games, LLC. All Rights Reserved.

#pragma once

#define UE_API FLECSENTITY_API

UE_API DECLARE_LOG_CATEGORY_EXTERN(LogFlecs, Warning, All);

UE_API DECLARE_LOG_CATEGORY_EXTERN(LogFlecsJournal, VeryVerbose, All);

DECLARE_STATS_GROUP(TEXT("Flecs"), STATGROUP_Flecs, STATCAT_Advanced);

DECLARE_CYCLE_STAT_EXTERN(TEXT("Flecs Total Frame Time"), STAT_Flecs_Total, STATGROUP_Flecs, FLECSENTITY_API);

#undef UE_API
