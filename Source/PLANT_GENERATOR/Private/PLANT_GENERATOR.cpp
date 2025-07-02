// Copyright Epic Games, Inc. All Rights Reserved.

#include "PLANT_GENERATOR.h"
#include "PLANT_GENERATORStyle.h"
#include "PLANT_GENERATORCommands.h"
#include "SPLANT_WIDGET.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName PLANT_GENERATORTabName("PLANT_GENERATOR");

#define LOCTEXT_NAMESPACE "FPLANT_GENERATORModule"

void FPLANT_GENERATORModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FPLANT_GENERATORStyle::Initialize();
	FPLANT_GENERATORStyle::ReloadTextures();

	FPLANT_GENERATORCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FPLANT_GENERATORCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FPLANT_GENERATORModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPLANT_GENERATORModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(PLANT_GENERATORTabName, FOnSpawnTab::CreateRaw(this, &FPLANT_GENERATORModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FPLANT_GENERATORTabTitle", "PLANT_GENERATOR"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FPLANT_GENERATORModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FPLANT_GENERATORStyle::Shutdown();

	FPLANT_GENERATORCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PLANT_GENERATORTabName);
}

TSharedRef<SDockTab> FPLANT_GENERATORModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
	 .TabRole(ETabRole::NomadTab)
	 [
		 SNew(SPLANT_WIDGET)
	 ];
}

void FPLANT_GENERATORModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(PLANT_GENERATORTabName);
}

void FPLANT_GENERATORModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FPLANT_GENERATORCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FPLANT_GENERATORCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPLANT_GENERATORModule, PLANT_GENERATOR)