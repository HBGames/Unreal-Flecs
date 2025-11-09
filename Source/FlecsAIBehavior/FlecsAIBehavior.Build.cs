using UnrealBuildTool;

public class FlecsAIBehavior : ModuleRules
{
	public FlecsAIBehavior(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"FlecsEntity",
				"FlecsSignals",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"FlecsActors",
				"FlecsLOD",
			}
		);
	}
}