// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor)) {
		RemoveFromParent();
		UE_LOG(LogTemp, Log, TEXT("AttachedActor no longer valid. Removing Health Widget."));
		return;
	}

	FVector2D screenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, screenPosition)) {
		float scale = UWidgetLayoutLibrary::GetViewportScale(this);

		screenPosition /= scale;

		if (ParentSizeBox) {
			ParentSizeBox->SetRenderTranslation(screenPosition);
		}
	}
}
