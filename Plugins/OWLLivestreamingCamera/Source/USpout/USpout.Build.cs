// Copyright Off World Live Limited, 2020-2021. All rights reserved.

using UnrealBuildTool;
using System.IO;


public class USpout : ModuleRules
{
    public USpout(ReadOnlyTargetRules Target) : base(Target)
    {

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "USpout/Private",
				// ... add other private include paths required here ...
			}
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Spout", 
				// ... add other public dependencies that you statically link with here ...
			}
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "RHI",
                "RenderCore",
                "Projects",
                "Spout", 
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {	
				// ... add any modules that your module loads dynamically here ...
			}
            );

        if ((Target.Platform == UnrealTargetPlatform.Win64))
        {

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                   "D3D12RHI",
                });

            PrivateIncludePaths.AddRange(
                new string[]
                {
                     Path.Combine(EngineDirectory, "Source/Runtime/D3D12RHI/Private"),
                     Path.Combine(EngineDirectory, "Source/Runtime/D3D12RHI/Private/Windows"),

                });

            AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");

        }

    }
}