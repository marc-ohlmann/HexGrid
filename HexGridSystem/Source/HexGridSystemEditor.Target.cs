// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HexGridSystemEditorTarget : TargetRules
{
	public HexGridSystemEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		ExtraModuleNames.Add("HexGridSystem");
		
		// suppress upgrade warning created when moving from UE 5.2 to 5.3
		DefaultBuildSettings = BuildSettingsVersion.Latest;

        // suppress upgrade warning created when moving from UE 5.0 to 5.1
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest; //  may cause build errors with future engine versions
                                                                //IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
                                                                //IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_0;
	}
}
