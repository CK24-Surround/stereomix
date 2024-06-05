// Copyright Surround, Inc. All Rights Reserved.


#include "SMViewModelResolver.h"

UObject* USMViewModelResolver::CreateInstance(const UClass* ExpectedType, const UUserWidget* UserWidget, const UMVVMView* View) const
{
	return Super::CreateInstance(ExpectedType, UserWidget, View);
}
