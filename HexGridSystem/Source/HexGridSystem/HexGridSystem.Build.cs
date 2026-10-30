// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HexGridSystem : ModuleRules
{
	public HexGridSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        if (Target.bBuildEditor)
        {
            // unity build decreases compile times, but hides errors
            bUseUnity = false;

            // precompiled headers reduce compile time, but hides errors
            PCHUsage = PCHUsageMode.NoPCHs;

            // enforce "include-what-you-use" requires files to only include what they use
            // this is a #include strategy for reducing compile times, that does not hide errors
            IWYUSupport = IWYUSupport.Full;

            PublicDependencyModuleNames.AddRange(new string[] { "HexGridEditorPlugin", });
        }
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ProceduralMeshComponent", "UMG",
                "MEOHelpersPlugin",
                "HexGridPlugin", 
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
