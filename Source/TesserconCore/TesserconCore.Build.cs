// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class TesserconCore : ModuleRules
{
	public TesserconCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		//Core Depemdencies
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
		//Networking Dependencies
		PublicDependencyModuleNames.AddRange(new string[] { "Http", "Json", "JsonUtilities" });
		//Matrix Dependency
		PublicDependencyModuleNames.AddRange(new string[] { "MatrixCore" });
		//SQLite ORM Dependency
		PublicDependencyModuleNames.AddRange(new string[] { "SQLite_ORM" });
		//Vivox Dependency
		PublicDependencyModuleNames.AddRange(new string[] { "VivoxCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(
			new string[] {
				string.Format("{0}", ModuleDirectory),
			}
			);

		
	}
}
