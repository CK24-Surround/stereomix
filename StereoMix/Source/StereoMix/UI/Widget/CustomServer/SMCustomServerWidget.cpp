// Copyright Surround, Inc. All Rights Reserved.


#include "SMCustomServerWidget.h"

#include "Kismet/GameplayStatics.h"

void USMCustomServerWidget::ConnectToServer(const FString Url, const FString UserName)
{
	if (Url.IsEmpty() || UserName.IsEmpty())
	{
		return;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName(Url), true, FString::Printf(TEXT("CustomServer?Nickname=%s"), *UserName));
}
