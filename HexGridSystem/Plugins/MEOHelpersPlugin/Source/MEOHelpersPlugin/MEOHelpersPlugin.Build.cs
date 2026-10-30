// Copyright 2022 Marc Ohlmann, All Rights Reserved.

using UnrealBuildTool;

public class MEOHelpersPlugin : ModuleRules
{
	public MEOHelpersPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        if (Target.bBuildEditor)
        {
            // unity build decreases compile times, but hides errors
            bUseUnity = false;

            // precompiled headers reduce compile time, but hides errors
            PCHUsage = PCHUsageMode.NoPCHs;

            // enforce "include-what-you-use" requires files to only include what they use
            // this is a #include strategy for reducing compile times, that does not hide errors
            IWYUSupport = IWYUSupport.Full;
        }

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
				"InputCore",
				"RHI",
				"AIModule",
				"UMG",
				"AudioMixer",
				"AudioMixerCore",
				"EnhancedInput",
                "DeveloperSettings",
                "EngineSettings",
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
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		//bUsePrecompiled = true;
		PrecompileForTargets = PrecompileTargetsType.Any;
	}
}
