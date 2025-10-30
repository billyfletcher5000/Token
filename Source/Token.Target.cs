// Copyright (C) Billy Fletcher 2025

using UnrealBuildTool;
using System.Collections.Generic;

public class TokenTarget : TargetRules
{
	public TokenTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange( new string[] { "Token" } );
	}
}
