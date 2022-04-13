// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class MatrixCore : ModuleRules
{
	public MatrixCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
		//Database Dependency
		PublicDependencyModuleNames.AddRange(new string[] { "SQLite_ORM" });
		//Networking Dependency
		PublicDependencyModuleNames.AddRange(new string[] { "Http", "Json", "JsonUtilities" });
		//Tessercon Core Dependency
		PrivateDependencyModuleNames.AddRange(new string[] { "TesserconCore" });

		PublicIncludePaths.AddRange(
			new string[] {
				string.Format("{0}", ModuleDirectory),
			}
			);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
