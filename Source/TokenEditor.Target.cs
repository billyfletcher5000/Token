// Copyright (C) Billy Fletcher 2025

using UnrealBuildTool;
using System.Collections.Generic;

public class TokenEditorTarget : TargetRules
{
	public TokenEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "TokenEditor", "TokenBlueprint", "Token" } );
	}
}
