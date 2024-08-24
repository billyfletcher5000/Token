using UnrealBuildTool;

public class TokenBlueprint : ModuleRules
{
    public TokenBlueprint(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { 
	        "Core", 
	        "CoreUObject", 
	        "Engine", 
	        "Token",
	        "BlueprintGraph",
	        "UnrealEd",
	        "Slate", 
	        "SlateCore",
	        "Cadence"
        });
        
        if (Target.bBuildEditor)
        {
	        PrivateDependencyModuleNames.AddRange(new string[] 
	        { 
		        "CadenceEditor"
	        });
        }
    }
}