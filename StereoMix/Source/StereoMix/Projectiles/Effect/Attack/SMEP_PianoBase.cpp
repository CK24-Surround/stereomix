// Copyright Studio Surround. All Rights Reserved.


#include "SMEP_PianoBase.h"

void ASMEP_PianoBase::HandleHitEffect(AActor* InTarget)
{
	Super::HandleHitEffect(InTarget);

	ApplyDamage(InTarget);
}
