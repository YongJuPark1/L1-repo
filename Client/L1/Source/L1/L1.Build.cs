using UnrealBuildTool;

public class L1 : ModuleRules
{
    public L1(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = true;

        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore",
            "LyraGame",
            "ModularGameplay"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { 
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks"
        });
    }
} 