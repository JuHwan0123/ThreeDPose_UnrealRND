// Copyright Off World Live Limited, 2020-2021. All rights reserved.

using UnrealBuildTool;
using System.IO;


public class Spout : ModuleRules
{
    public Spout(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        if ((Target.Platform == UnrealTargetPlatform.Win64))
        {
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
            PublicDelayLoadDLLs.Add("Spout.dll");
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib/amd64/Spout.lib"));
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "lib/amd64/Spout.dll"));
        }
    }
}