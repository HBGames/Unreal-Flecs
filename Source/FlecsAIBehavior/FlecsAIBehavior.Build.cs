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
				"FlecsLibrary",
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
				"FlecsSimulation",
			}
		);
	}
}