// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CadenceEditor : ModuleRules
{
	public CadenceEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"Cadence", 
				"KismetWidgets",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"AssetTools",
				"UnrealEd",
				"EditorSubsystem",
				"PropertyEditor",
				"Projects",
				"ToolMenus",
				"GraphEditor",
				"ApplicationCore",
				"InputCore",
				"Sequencer",
				"SequencerCore",
				"MovieScene",
				"MovieSceneTracks",
				"ToolWidgets",
				"BlueprintGraph",
				"Harmonix",
				"HarmonixMidi",
				"HarmonixEditor",
				"HarmonixMidiEditor",
				"ToolMenus",
				"ToolMenusEditor"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
