#include "SPLANT_WIDGET.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Crop_generator.h"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPLANT_WIDGET::Construct(const FArguments& InArgs)
{
    // Populate dropdown options
    Options.Add(MakeShared<FString>("Corn"));
    Options.Add(MakeShared<FString>("Carrot"));
    Options.Add(MakeShared<FString>("Grape"));
    Options.Add(MakeShared<FString>("Apple"));
	Options.Add(MakeShared<FString>("Grapevine"));

    // Default selected option
    SelectedOption = Options[0];

    ChildSlot
    [
        SNew(SVerticalBox)

        // Dropdown selection - always shown
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Select Crop:"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SComboBox<TSharedPtr<FString>>)
                .OptionsSource(&Options)
                .OnSelectionChanged(this, &SPLANT_WIDGET::OnSelectionChanged)
                .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
                {
                    return SNew(STextBlock).Text(FText::FromString(*Item));
                })
                .InitiallySelectedItem(SelectedOption)
                [
                    SNew(STextBlock)
                    .Text(this, &SPLANT_WIDGET::GetSelectedOptionText)
                ]
            ]
        ]

		// Grapevine-specific controls
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5)
		[
			SAssignNew(GrapevineControls, SVerticalBox)
			// Only visible when Grapevine is selected
			.Visibility_Lambda([this]() {
			    return *SelectedOption == "Grapevine" ? EVisibility::Visible : EVisibility::Collapsed;
			})

            // Number of generations
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Amount:"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SSpinBox<int>)
                    .Value(this->amount)
                    .OnValueChanged(this, &SPLANT_WIDGET::OnAmountChanged)
                ]
            ]
        ]

        // Corn-specific controls
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(CornControls, SVerticalBox)
            // Only visible when Corn is selected
            .Visibility_Lambda([this]() {
                return *SelectedOption == "Corn" ? EVisibility::Visible : EVisibility::Collapsed;
            })

            // Slider 1
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Plant age:"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SSlider)
                    .Value(this->plantage)
                    .OnValueChanged(this, &SPLANT_WIDGET::OnPlantAgeChanged)
                ]
            ]
            
            // Number of generations
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Amount:"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SSpinBox<int>)
                    .Value(this->amount)
                    .OnValueChanged(this, &SPLANT_WIDGET::OnAmountChanged)
                ]
            ]
        ]

        // Carrot-specific controls
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(CarrotControls, SVerticalBox)
            // Only visible when Carrot is selected
            .Visibility_Lambda([this]() {
                return *SelectedOption == "Carrot" ? EVisibility::Visible : EVisibility::Collapsed;
            })
            
            // Example carrot control
            // Number of generations
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Amount:"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SSpinBox<int>)
                    .Value(this->amount)
                    .OnValueChanged(this, &SPLANT_WIDGET::OnAmountChanged)
                ]
            ]
            
            // New checkbox for "Cracked" option
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Cracked:"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SCheckBox)
                    .IsChecked(this, &SPLANT_WIDGET::IsCrackedChecked)
                    .OnCheckStateChanged(this, &SPLANT_WIDGET::OnCrackedChanged)
                ]
            ]
        ]

        // Grape-specific controls
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(GrapeControls, SVerticalBox)
            // Only visible when Grape is selected
            .Visibility_Lambda([this]() {
                return *SelectedOption == "Grape" ? EVisibility::Visible : EVisibility::Collapsed;
            })
            
            // Example carrot control
           // Number of generations
           + SVerticalBox::Slot()
           .AutoHeight()
           .Padding(5)
           [
               SNew(SHorizontalBox)
               + SHorizontalBox::Slot()
               .AutoWidth()
               [
                   SNew(STextBlock)
                   .Text(FText::FromString("Amount:"))
               ]
               + SHorizontalBox::Slot()
               [
                   SNew(SSpinBox<int>)
                   .Value(this->amount)
                   .OnValueChanged(this, &SPLANT_WIDGET::OnAmountChanged)
               ]
           ]
        ]

        // Apple-specific controls
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(AppleControls, SVerticalBox)
            // Only visible when Apple is selected
            .Visibility_Lambda([this]() {
                return *SelectedOption == "Apple" ? EVisibility::Visible : EVisibility::Collapsed;
            })
            
            // Example Apple control
           // Number of generations
           + SVerticalBox::Slot()
           .AutoHeight()
           .Padding(5)
           [
               SNew(SHorizontalBox)
               + SHorizontalBox::Slot()
               .AutoWidth()
               [
                   SNew(STextBlock)
                   .Text(FText::FromString("Amount:"))
               ]
               + SHorizontalBox::Slot()
               [
                   SNew(SSpinBox<int>)
                   .Value(this->amount)
                   .OnValueChanged(this, &SPLANT_WIDGET::OnAmountChanged)
               ]
           ]
        ]

        // Generate button - shown for all options
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Generate"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SButton)
                .OnClicked(this, &SPLANT_WIDGET::OnGenerateClicked)
            ]
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

// Add these new functions to handle the checkbox state
ECheckBoxState SPLANT_WIDGET::IsCrackedChecked() const
{
    return cracked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SPLANT_WIDGET::OnCrackedChanged(ECheckBoxState NewState)
{
    cracked = (NewState == ECheckBoxState::Checked);
}

// Dropdown selection callback
void SPLANT_WIDGET::OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
    SelectedOption = NewValue;
    
    // Update visibility of control groups when selection changes
    if (CornControls.IsValid())
    {
        CornControls->SetVisibility(*SelectedOption == "Corn" ? EVisibility::Visible : EVisibility::Collapsed);
    }
    if (CarrotControls.IsValid())
    {
        CarrotControls->SetVisibility(*SelectedOption == "Carrot" ? EVisibility::Visible : EVisibility::Collapsed);
    }
    if (GrapeControls.IsValid())
    {
        GrapeControls->SetVisibility(*SelectedOption == "Grape" ? EVisibility::Visible : EVisibility::Collapsed);
    }
    if (AppleControls.IsValid())
    {
        AppleControls->SetVisibility(*SelectedOption == "Apple" ? EVisibility::Visible : EVisibility::Collapsed);
    }
	if (GrapevineControls.IsValid())
	{
		GrapevineControls->SetVisibility(*SelectedOption == "Grapevine" ? EVisibility::Visible : EVisibility::Collapsed);
	}
}

// Get selected dropdown text
FText SPLANT_WIDGET::GetSelectedOptionText() const
{
    return FText::FromString(*SelectedOption);
}

// Slider callbacks
void SPLANT_WIDGET::OnPlantAgeChanged(float Value)
{
    plantage = Value;
}

void SPLANT_WIDGET::OnAmountChanged(int value)
{
    amount = value;
}

FReply SPLANT_WIDGET::OnGenerateClicked()
{

    TMap<FString, float> parameters = {};

    if (*SelectedOption == "Corn")
    {
        parameters.Add("plantage", plantage);
        for (int i = 0; i < amount;i++)
        {
            FVector location(i * 100.0f, 0.0f, 0.0f); 
            FTransform transform(location);   
            UCrop_Generator::Create_variation(Plant_types::Corn, transform, parameters);
        }
    }
    else if (*SelectedOption == "Carrot")
    {
        parameters.Add("cracked", (float)cracked);
        for (int i = 0; i < amount;i++)
        {
            FVector location(i * 100.0f, 0.0f, 0.0f); 
            FTransform transform(location); 
            UCrop_Generator::Create_variation(Plant_types::Carrot, transform, parameters);
        }
    }
    else if (*SelectedOption == "Grape")
    {
        for (int i = 0; i < amount;i++)
        {
            FVector location(i * 1000.0f, 0.0f, 0.0f); 
            FTransform transform(location); 
            UCrop_Generator::Create_variation(Plant_types::Grape, transform, parameters);
        }
    }
    else if (*SelectedOption == "Apple")
	{
		for (int i = 0; i < amount;i++)
		{
			FVector location(i * 1000.0f, 0.0f, 0.0f);
			FTransform transform(location);
			UCrop_Generator::Create_variation(Plant_types::Apple, transform, parameters);
		}
	}
	else if (*SelectedOption == "Grapevine")
	{
		for (int i = 0; i < amount; i++)
		{
			FVector location(i * 100.0f, 0.0f, 0.0f);
			FTransform transform(location);
			UCrop_Generator::Create_variation(Plant_types::Grapevine, transform, parameters);
		}
	}
    return FReply::Handled();
}