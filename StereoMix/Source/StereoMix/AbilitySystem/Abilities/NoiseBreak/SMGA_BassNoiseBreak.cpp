// Copyright Surround, Inc. All Rights Reserved.


#include "SMGA_BassNoiseBreak.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/Player/SMBassCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Data/Character/SMPlayerCharacterDataAsset.h"
#include "FunctionLibraries/SMCalculateBlueprintLibrary.h"
#include "Games/SMGameState.h"
#include "HoldInteraction/SMHIC_Character.h"
#include "Tiles/SMTile.h"
#include "Tiles/SMTileManagerComponent.h"
#include "Utilities/SMCollision.h"

void USMGA_BassNoiseBreak::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	USMHIC_Character* SourceHIC = GetHIC<USMHIC_Character>();
	if (!ensureAlways(SourceHIC))
	{
		EndAbilityByCancel();
		return;
	}

	const USMPlayerCharacterDataAsset* SourceDataAsset = SourceCharacter->GetDataAsset();
	if (!ensureAlways(SourceDataAsset))
	{
		EndAbilityByCancel();
		return;
	}

	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (!SourceCapsule)
	{
		EndAbilityByCancel();
		return;
	}

	AActor* TargetActor = SourceHIC->GetActorIAmHolding();
	USMHoldInteractionComponent* TargetHIC = nullptr;
	if (TargetActor)
	{
		TargetHIC = Cast<USMHoldInteractionComponent>(USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor));
		if (!ensureAlways(TargetHIC))
		{
			EndAbilityByCancel();
			return;
		}
	}

	ESMTeam SourceTeam = SourceCharacter->GetTeam();
	CachedNoiseBreakMontage = SourceDataAsset->NoiseBreakMontage[SourceTeam];

	OriginalCollisionProfileName = SourceCapsule->GetCollisionProfileName();
	SourceCapsule->SetCollisionProfileName(SMCollisionProfileName::NoiseBreak);

	// 스매시는 착지 시 판정이 발생하고 이에 대한 처리가 수행되어야합니다. 착지 델리게이트를 기다립니다.
	SourceCharacter->OnCharacterLanded.AddUObject(this, &ThisClass::OnNoiseBreak);

	// 스매시 애니메이션을 재생합니다.
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("BassNoiseBreak"), CachedNoiseBreakMontage);
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	PlayMontageAndWaitTask->ReadyForActivation();

	// 타겟 측에 스매시가 시작됨을 알립니다.
	if (ActorInfo->IsNetAuthority())
	{
		TargetHIC->OnNoiseBreakActionStarted(SourceCharacter);
	}

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	// 스매시 중 필요한 중력 값을 적용합니다. 착지 후 복구해줘야합니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensure(SourceMovement))
	{
		OriginalGravityScale = SourceMovement->GravityScale;
		SourceMovement->GravityScale = NoiseBreakGravityScale;
	}

	if (ActorInfo->IsLocallyControlled())
	{
		// 시작점과 목표 지점의 위치를 구합니다.
		const FVector SourceLocation = SourceCharacter->GetActorLocation();
		const FVector TargetLocation = CalculateMaxDistanceLocation(SourceLocation, SourceCharacter->GetCursorTargetingPoint(true));

		// 서버로 타겟 위치를전송합니다.
		SendToServerLocationData(SourceLocation, TargetLocation);

		// 정점 높이를 기준으로 타겟을 향하는 벨로시티를 구한 후 캐릭터를 도약 시킵니다.
		LaunchCharacterToTargetWithApex(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());
	}

	if (ActorInfo->IsNetAuthority())
	{
		// 서버는 데이터를 수신을 준비합니다. 수신 후 알아서 바인드 해제됩니다.
		FAbilityTargetDataSetDelegate& TargetDataSetDelegate = SourceASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey());
		TargetDataSetDelegate.AddUObject(this, &ThisClass::OnReceiveTargetData);
	}

	// SourceASC->AddGameplayCue(SMTags::GameplayCue::SpecialAction::Smash);
}

void USMGA_BassNoiseBreak::OnReceiveTargetData(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (!ensureAlways(SourceMovement))
	{
		EndAbilityByCancel();
		return;
	}

	const FGameplayAbilityTargetData* TargetData = GameplayAbilityTargetDataHandle.Get(0);
	if (!ensureAlways(TargetData))
	{
		EndAbilityByCancel();
		return;
	}

	// 서버에서도 클라이언트와 동일한 방향으로 캐릭터를 도약시킵니다.
	const FVector SourceLocation = TargetData->GetOrigin().GetLocation();
	const FVector TargetLocation = TargetData->GetEndPoint();
	LaunchCharacterToTargetWithApex(SourceLocation, TargetLocation, SourceMovement->GetGravityZ());
}

void USMGA_BassNoiseBreak::OnNoiseBreak(ASMPlayerCharacterBase* LandedCharacter)
{
	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	USMAbilitySystemComponent* SourceASC = GetASC<USMAbilitySystemComponent>();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	USMHIC_Character* SourceHIC = Cast<USMHIC_Character>(GetHIC());
	if (!ensureAlways(SourceHIC))
	{
		EndAbilityByCancel();
		return;
	}

	AActor* TargetActor = SourceHIC->GetActorIAmHolding();
	USMHoldInteractionComponent* TargetHIC = nullptr;
	if (TargetActor)
	{
		TargetHIC = USMHoldInteractionBlueprintLibrary::GetHoldInteractionComponent(TargetActor);
		if (!ensureAlways(TargetHIC))
		{
			EndAbilityByCancel();
			return;
		}
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("타겟 액터가 유효하지 않습니다. "));
	}

	// 착지 완료 했으니 델리게이트는 제거해줍니다.
	SourceCharacter->OnCharacterLanded.RemoveAll(this);

	// 기존 중력 스케일로 재설정합니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensureAlways(SourceMovement))
	{
		SourceMovement->GravityScale = OriginalGravityScale;
		SourceMovement->StopMovementImmediately();
	}

	UCapsuleComponent* SourceCapsule = SourceCharacter->GetCapsuleComponent();
	if (SourceCapsule)
	{
		SourceCapsule->SetCollisionProfileName(OriginalCollisionProfileName);
	}

	// 스매시 종료 애니메이션을 재생합니다. 이 애니메이션이 종료되면 스매시가 종료됩니다. 종료는 서버에서 수행하게 됩니다.
	const FName SectionName = TEXT("End");
	UAbilityTask_PlayMontageAndWait* PlayMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("NoiseBreakEnd"), CachedNoiseBreakMontage, 1.0f, SectionName, false);
	if (!ensureAlways(PlayMontageAndWaitTask))
	{
		EndAbilityByCancel();
		return;
	}
	PlayMontageAndWaitTask->ReadyForActivation();

	// 스매시 종료 애니메이션 진입까지 기다립니다. 이후 스매시 GA를 종료합니다.
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, SMTags::Event::AnimNotify::NoiseBreakEnd);
	if (!ensureAlways(WaitGameplayEventTask))
	{
		EndAbilityByCancel();
		return;
	}
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnNoiseBreakEnd);
	WaitGameplayEventTask->ReadyForActivation();

	if (CurrentActorInfo->IsNetAuthority())
	{
		if (TargetActor)
		{
			TSharedPtr<FSMNoiseBreakData> TempData = MakeShared<FSMNoiseBreakData>();
			TargetHIC->OnNoiseBreakActionPerformed(SourceCharacter, TempData);
		}

		TileCapture();
		SourceHIC->SetActorIAmHolding(nullptr);
	}

	// SourceASC->RemoveGameplayCue(SMTags::GameplayCue::SpecialAction::Smash);
}

void USMGA_BassNoiseBreak::OnNoiseBreakEnd(FGameplayEventData Payload)
{
	K2_EndAbilityLocally();
}

FVector USMGA_BassNoiseBreak::CalculateMaxDistanceLocation(const FVector& InStartLocation, const FVector& InTargetLocation)
{
	// 시작 위치의 Z값을 타겟의 Z값으로 정렬합니다. 타겟의 Z는 바닥과 동일한 높이이기 때문입니다. 이렇게하면 소스 위치가 소스의 중심에서 소스의 바닥으로 재정렬됩니다.
	FVector AlignedSourceZ = InStartLocation;
	AlignedSourceZ.Z = InTargetLocation.Z;

	// 사거리를 제한 합니다.
	// TODO: 임시로 150으로 타일 사이즈를 하드코딩 해두었습니다.
	constexpr float TileSize = 150.0f;
	const float Range = TileSize * SmashRangeByTile;
	if (FVector::DistSquared(AlignedSourceZ, InTargetLocation) > FMath::Square(Range))
	{
		const FVector SourceToTargetDirection = (InTargetLocation - AlignedSourceZ).GetSafeNormal();

		const FVector Result = AlignedSourceZ + (SourceToTargetDirection * Range);
		return Result;
	}

	return InTargetLocation;
}

void USMGA_BassNoiseBreak::SendToServerLocationData(const FVector& InStartLocation, const FVector& InTargetLocation)
{
	USMAbilitySystemComponent* SourceASC = GetSMAbilitySystemComponentFromActorInfo();
	if (!ensureAlways(SourceASC))
	{
		EndAbilityByCancel();
		return;
	}

	// 필요한 위치 데이터들을 타겟 데이터에 저장합니다.
	FGameplayAbilityTargetData_LocationInfo* TargetData = new FGameplayAbilityTargetData_LocationInfo();
	TargetData->SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	TargetData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	TargetData->SourceLocation.LiteralTransform.SetLocation(InStartLocation);
	TargetData->TargetLocation.LiteralTransform.SetLocation(InTargetLocation);

	// 타겟 데이터를 핸들에 담고 서버로 전송합니다.
	FGameplayAbilityTargetDataHandle TargetDatahandle(TargetData);
	SourceASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), TargetDatahandle, FGameplayTag(), SourceASC->ScopedPredictionKey);
}

void USMGA_BassNoiseBreak::LaunchCharacterToTargetWithApex(const FVector& InStartLocation, const FVector& InTargetLocation, float InGravityZ)
{
	ASMPlayerCharacterBase* SourceCharacter = GetAvatarActor<ASMPlayerCharacterBase>();
	if (!SourceCharacter)
	{
		EndAbilityByCancel();
		return;
	}

	const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight(SourceCharacter, InStartLocation, InTargetLocation, SmashApexHeight, InGravityZ);
	SourceCharacter->LaunchCharacter(LaunchVelocity, true, true);
}

void USMGA_BassNoiseBreak::TileCapture()
{
	ASMBassCharacter* SourceCharacter = GetAvatarActor<ASMBassCharacter>();
	if (!SourceCharacter)
	{
		return;
	}

	ASMGameState* GameState = GetWorld()->GetGameState<ASMGameState>();
	if (!GameState)
	{
		return;
	}

	USMTileManagerComponent* TileManager = GameState->GetTileManager();
	if (!TileManager)
	{
		return;
	}

	FHitResult HitResult;
	const FVector StartLocation = SourceCharacter->GetActorLocation();
	const FVector EndLocation = StartLocation + FVector::DownVector * 1000.0;
	const bool bSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction);

	if (bSuccess)
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			TileManager->TileCaptureDelayedSqaure(Tile, SourceCharacter->GetTeam(), MaxTriggerCount, TotalTriggerTime);
		}
	}
}
