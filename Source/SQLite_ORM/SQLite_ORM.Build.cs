// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SQLite_ORM : ModuleRules
{
	public SQLite_ORM(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject" });
    }
}