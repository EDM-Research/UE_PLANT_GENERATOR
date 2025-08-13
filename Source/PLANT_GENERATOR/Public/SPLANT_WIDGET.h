#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SPLANT_WIDGET : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPLANT_WIDGET) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	float plantage = 0.5f;  
	int amount = 0;

	TSharedPtr<FString> SelectedOption;
	TArray<TSharedPtr<FString>> Options;
	FString exportpath;

	TSharedPtr<SVerticalBox> CornControls;
	TSharedPtr<SVerticalBox> CarrotControls;
	TSharedPtr<SVerticalBox> GrapeControls;
	TSharedPtr<SVerticalBox> AppleControls;
	TSharedPtr<SVerticalBox> GrapevineControls;

	FText GetSelectedOptionText() const;
	void OnPlantAgeChanged(float Value);
	void OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	void OnAmountChanged(int value);
	FReply OnGenerateClicked();

	bool cracked = false;
	ECheckBoxState IsCrackedChecked() const;
	void OnCrackedChanged(ECheckBoxState NewState);
};
