// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class NesUnrealEditorTarget : TargetRules
{
	public NesUnrealEditorTarget(TargetInfo Target) : base(Target)
	{
		DefaultBuildSettings = BuildSettingsVersion.V2;
		Type = TargetType.Editor;
		ExtraModuleNames.AddRange( new string[] { "NesUnreal" } );
	}
}
