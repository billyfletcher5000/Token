using UnrealBuildTool;

public class TokenEditor : ModuleRules
{
    public TokenEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Token", "Slate", "SlateCore" });
    }
}