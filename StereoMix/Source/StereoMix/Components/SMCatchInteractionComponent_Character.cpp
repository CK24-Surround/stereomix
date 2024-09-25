// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchInteractionComponent_Character.h"

#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/SMPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Data/SMSpecialAction.h"
#include "Net/UnrealNetwork.h"
#include "Tiles/SMTile.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


void USMCatchInteractionComponent_Character::InitializeComponent()
{
	Super::InitializeComponent();
}

void USMCatchInteractionComponent_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, IAmCatchingActor);
}

void USMCatchInteractionComponent_Character::BeginPlay()
{
	Super::BeginPlay();

	SourceCharacter = CastChecked<ASMPlayerCharacter>(GetOwner());
}

void USMCatchInteractionComponent_Character::InitASC(USMAbilitySystemComponent* NewASC)
{
	SourceASC = CastChecked<USMAbilitySystemComponent>(NewASC);
}

bool USMCatchInteractionComponent_Character::IsCatchable(AActor* TargetActor) const
{
	if (!SourceCharacter->HasAuthority())
	{
		return false;
	}

	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
	if (!ensureAlways(TargetCharacter))
	{
		return false;
	}

	// 무소속이라면 잡을 수 없습니다.
	// 사실상 무소속은 대미지를 받지 않기 때문에 기절할 일은 없긴하지만 만약을 위한 예외처리입니다.
	const ESMTeam SourceTeam = SourceCharacter->GetTeam();
	if (SourceTeam == ESMTeam::None)
	{
		return false;
	}

	// 팀이 같다면 잡을 수 없습니다.
	if (SourceTeam == TargetCharacter->GetTeam())
	{
		return false;
	}

	// 자신을 잡았던 캐릭터들을 순회하며 한 번이라도 자신을 잡았던 대상인지 확인합니다.
	bool bIsCaptureCharacter = false;
	for (const auto& CapturedMeCharcter : CapturedMeCharcters)
	{
		if (CapturedMeCharcter.Get())
		{
			if (CapturedMeCharcter == TargetCharacter)
			{
				bIsCaptureCharacter = true;
				break;
			}
		}
	}

	// 자신을 잡았던 타겟이라면 다시 잡을 수 없습니다.
	if (bIsCaptureCharacter)
	{
		return false;
	}

	// 잡을 수 없는 상태 태그가 부착되어 있다면 잡을 수 없습니다.
	if (SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Uncatchable))
	{
		return false;
	}

	// 스턴 된 상태가 아니라면 잡을 수 없습니다.
	if (!SourceASC->HasMatchingGameplayTag(SMTags::Character::State::Stun))
	{
		return false;
	}

	return true;
}

void USMCatchInteractionComponent_Character::OnCaught(AActor* TargetActor)
{
	if (!SourceCharacter->HasAuthority())
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	InternalCaught(TargetActor);
}

void USMCatchInteractionComponent_Character::OnCaughtReleased(AActor* TargetActor, bool bIsStunEnd)
{
	if (!SourceCharacter->HasAuthority())
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	InternalCaughtReleased(TargetActor, bIsStunEnd);
}

void USMCatchInteractionComponent_Character::OnSpecialActionPerformed(AActor* Instigator, ESpecialAction InSpecialAction)
{
	if (!SourceCharacter->HasAuthority())
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	switch (InSpecialAction)
	{
		case ESpecialAction::Smash:
		{
			InternalSmashed();
			break;
		}
	}
}

void USMCatchInteractionComponent_Character::OnSpecialActionEnded(AActor* Instigator, ESpecialAction InSpecialAction, float InMagnitude, TSubclassOf<UGameplayEffect> DamageGE, float InDamageAmount)
{
	if (!SourceCharacter->HasAuthority())
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	switch (InSpecialAction)
	{
		case ESpecialAction::Smash:
		{
			InternalSmashedEnded(Instigator, InMagnitude, DamageGE, InDamageAmount);
			break;
		}
	}
}

void USMCatchInteractionComponent_Character::SetActorIAmCatching(AActor* NewIAmCatchingActor)
{
	if (!SourceCharacter->HasAuthority())
	{
		return;
	}

	// 중복 실행되어도 안전하도록 값이 다른 경우에만 처리됩니다.
	if (IAmCatchingActor != NewIAmCatchingActor)
	{
		// nullptr이 매개변수로 넘어왔다면 잡기가 해제된 상황으로 구독했던 이벤트를 해제합니다.
		if (IAmCatchingActor.Get() && !NewIAmCatchingActor)
		{
			IAmCatchingActor->OnDestroyed.RemoveAll(this);
		}

		IAmCatchingActor = NewIAmCatchingActor;
		OnRep_IAmCatchingActor();
	}
}

void USMCatchInteractionComponent_Character::CaughtReleased(AActor* TargetActor)
{
	// 이 함수는 만약 타겟이 null이어도 수행해야할 작업을 수행해줘야합니다. 따라서 TargetActor가 null이어도 즉시 반환하지 않고 예외처리를 해줍니다.

	if (!SourceCharacter->HasAuthority())
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	// 디태치합니다.
	SourceCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 자신을 잡고 있는 캐릭터를 제거하고 잡히기 상태 태그도 제거합니다.
	SetActorCatchingMe(nullptr);
	SourceASC->RemoveTag(SMTags::Character::State::Caught);

	// 충돌판정, 움직임을 복구합니다.
	SourceCharacter->SetEnableCollision(true);
	SourceCharacter->SetEnableMovement(true);

	// 위치보정을 활성화해줍니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensureAlways(SourceMovement))
	{
		SourceMovement->bIgnoreClientMovementErrorChecksAndCorrection = false;
	}

	// 회전을 재지정합니다.
	float NewYaw;
	if (TargetActor)
	{
		NewYaw = TargetActor->GetActorRotation().Yaw;
	}
	else
	{
		NET_LOG(SourceCharacter, Warning, TEXT("타겟이 유효하지 않아 자신의 Yaw를 사용합니다."));
		NewYaw = SourceCharacter->GetActorRotation().Yaw;
	}
	SourceCharacter->MulticastRPCSetYawRotation(NewYaw);

	SourceCharacter->ServerRPCPreventGroundEmbedding();

	// 카메라 뷰를 원래대로 복구합니다.
	APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->Controller);
	if (ensureAlways(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(SourceCharacter, 1.0f, VTBlend_Cubic);
	}
}

void USMCatchInteractionComponent_Character::OnDestroyedIAmCatchingActor(AActor* DestroyedActor)
{
	// 대상이 파괴되었으니 잡기를 해제합니다.
	SetActorIAmCatching(nullptr);
}

void USMCatchInteractionComponent_Character::InternalCaught(AActor* TargetActor)
{
	if (!SourceCharacter->HasAuthority())
	{
		NET_LOG(SourceCharacter, Warning, TEXT("서버에서만 호출되야합니다."));
		return;
	}

	ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
	if (!ensureAlways(TargetActor))
	{
		return;
	}

	// 대상에게 어태치합니다.
	const bool bAttachSuccess = SourceCharacter->AttachToComponent(TargetCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetCharacter->CatchSocket);
	if (!ensureAlways(bAttachSuccess))
	{
		return;
	}

	// 자신을 잡은 캐릭터를 저장하고 잡힌 상태 태그를 부착합니다.
	SetActorCatchingMe(TargetActor);
	SourceASC->AddTag(SMTags::Character::State::Caught);

	// 자신을 잡았던 캐릭터 목록을 업데이트 해줍니다.
	CapturedMeCharcters.Add(TargetCharacter);

	// 충돌판정, 움직임을 잠급니다.
	SourceCharacter->SetEnableCollision(false);
	SourceCharacter->SetEnableMovement(false);

	// 위치 보정 무시를 활성화합니다.
	UCharacterMovementComponent* SourceMovement = SourceCharacter->GetCharacterMovement();
	if (ensureAlways(SourceMovement))
	{
		SourceMovement->bIgnoreClientMovementErrorChecksAndCorrection = true;
	}

	// 루트와 메시 사이에 생기는 오프셋 차이를 수정합니다.
	SourceCharacter->MulticastRPCApplyAttachOffset();

	// 플레이어의 카메라 뷰를 타겟의 카메라 뷰로 전환합니다.
	APlayerController* SourcePlayerController = Cast<APlayerController>(SourceCharacter->Controller);
	if (ensureAlways(SourcePlayerController))
	{
		SourcePlayerController->SetViewTargetWithBlend(TargetCharacter, 1.0f, VTBlend_Cubic);
	}

	// 잡히기 GA를 활성화합니다.
	SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Caught));
}

void USMCatchInteractionComponent_Character::InternalCaughtReleased(AActor* TargetActor, bool bIsStunEnd)
{
	// 아래 GA에서 잡히기 탈출와 애니메이션을 재생해줍니다. 완료되면 이벤트를 전송합니다.
	if (!bIsStunEnd)
	{
		SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::CaughtExit));
	}
	else
	{
		SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::CaughtExitOnStunEnd));
	}
}

void USMCatchInteractionComponent_Character::InternalSmashed()
{
	// 스매시 당하는 중을 표시하는 태그를 부착합니다. 버저비터와 연결됩니다.
	SourceASC->AddTag(SMTags::Character::State::Smashed);
}

void USMCatchInteractionComponent_Character::InternalSmashedEnded(AActor* Instigator, float Magnitude, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount)
{
	InternalTileTriggerdBySmash(Instigator, Magnitude, DamageGE, DamageAmount);

	CaughtReleased(Instigator);

	SourceASC->RemoveTag(SMTags::Character::State::Smashed);
	SourceASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SMTags::Ability::Smashed));
}

void USMCatchInteractionComponent_Character::InternalTileTriggerdBySmash(AActor* Instigator, float Magnitude, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount)
{
	ASMPlayerCharacter* InstigatorCharacter = Cast<ASMPlayerCharacter>(Instigator);
	if (!ensureAlways(InstigatorCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* InstigatorASC = Cast<USMAbilitySystemComponent>(InstigatorCharacter->GetAbilitySystemComponent());
	if (!ensureAlways(InstigatorASC))
	{
		return;
	}

	// 타일을 트리거합니다.
	FHitResult HitResult;
	const FVector Start = InstigatorCharacter->GetActorLocation();
	const FVector End = Start + (-FVector::UpVector * 1000.0f);
	const FCollisionQueryParams Param(SCENE_QUERY_STAT(TileTrace), false);
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(25.0f);
	const bool bSuccess = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Param);
	DrawDebugLine(GetWorld(), Start, End, FColor::Silver, false, 3.0f);

	if (bSuccess)
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (Tile)
		{
			// 트리거된 타일을 타겟 데이터에 담습니다. 타일 트리거 GA로 전송하기 위함입니다.
			FGameplayAbilityTargetData_SingleTargetHit* TagetData = new FGameplayAbilityTargetData_SingleTargetHit();
			TagetData->HitResult = HitResult;

			FGameplayAbilityTargetDataHandle TargetDataHandle;
			TargetDataHandle.Add(TagetData);

			// TargetData에 Tile 액터를, EventMagnitude에 트리거되야하는 규모를 적용합니다. 이 데이터 또한 타일 트리거 GA로 전송하기 위해 저장합니다.
			FGameplayEventData Payload;
			Payload.TargetData = TargetDataHandle;
			Payload.EventMagnitude = Magnitude;

			// 이펙트를 재생합니다.
			FGameplayCueParameters GCParams;
			GCParams.Location = Tile->GetTileLocation();
			InstigatorASC->ExecuteGameplayCue(SMTags::GameplayCue::SpecialAction::SmashSuccessWithCharacter, GCParams);

			// 스플래시 데미지를 적용합니다.
			UBoxComponent* TileBoxComponent = Tile->GetBoxComponent();
			if (ensureAlways(TileBoxComponent))
			{
				InternalApplySmashSplashDamage(GCParams.Location, TileBoxComponent->GetScaledBoxExtent().X * 2, Magnitude, InstigatorCharacter, DamageGE, DamageAmount);
			}

			// 타일 트리거 정보를 가지고 인스티게이터의 타일 트리거 GA를 활성화합니다.
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InstigatorCharacter, SMTags::Event::Tile::TileTrigger, Payload);
		}
	}
}

void USMCatchInteractionComponent_Character::InternalApplySmashSplashDamage(const FVector& TileLocation, float TileHorizonSize, float Magnitude, AActor* Instigator, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount)
{
	ASMPlayerCharacter* InstigatorCharacter = Cast<ASMPlayerCharacter>(Instigator);
	if (!ensureAlways(InstigatorCharacter))
	{
		return;
	}

	USMAbilitySystemComponent* InstigatorASC = Cast<USMAbilitySystemComponent>(InstigatorCharacter->GetAbilitySystemComponent());
	if (!ensureAlways(InstigatorASC))
	{
		return;
	}

	const ESMTeam InstigatorTeam = InstigatorCharacter->GetTeam();

	// 트리거된 타일 크기 영역내 적을 검사합니다.
	TArray<FOverlapResult> OverlapResults;
	const FVector Start = TileLocation;
	const float HalfHorizenSize = (TileHorizonSize * (Magnitude - 1)) + (TileHorizonSize / 2);

	FVector CollisionHalfExtend;
	if (HalfHorizenSize > 0.0f)
	{
		CollisionHalfExtend = FVector(HalfHorizenSize, HalfHorizenSize, 100.0);
	}

	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionHalfExtend);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(SmashSplash), false, SourceCharacter);
	const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, SMCollisionTraceChannel::Action, CollisionShape, Params);

	if (bSuccess)
	{
		for (const auto& OverlapResult : OverlapResults)
		{
			ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(OverlapResult.GetActor());
			if (!TargetCharacter)
			{
				return;
			}

			USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(TargetCharacter->GetAbilitySystemComponent());
			if (!ensureAlways(TargetASC))
			{
				return;
			}

			if (!IsValidateTargetForSmashSplashDamage(TargetCharacter, InstigatorTeam))
			{
				continue;
			}

			// 대미지를 GE로 적용합니다.
			FGameplayEffectSpecHandle GESpecHandle = InstigatorASC->MakeOutgoingSpec(DamageGE, 1.0f, InstigatorASC->MakeEffectContext());
			if (ensureAlways(GESpecHandle.IsValid()))
			{
				GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, DamageAmount);
			}
			InstigatorASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);

			// GC로 피격 이펙트를 재생합니다.
			FGameplayCueParameters GCParams;
			GCParams.Location = TargetCharacter->GetActorLocation();
			GCParams.Instigator = InstigatorCharacter;
			InstigatorASC->ExecuteGameplayCue(SMTags::GameplayCue::SpecialAction::SmashSplashHit, GCParams);
		}
	}

	DrawDebugBox(GetWorld(), Start, CollisionHalfExtend, FColor::Orange, false, 2.0f);
}

bool USMCatchInteractionComponent_Character::IsValidateTargetForSmashSplashDamage(const ASMPlayerCharacter* TargetActor, ESMTeam InstigatorTeam)
{
	// 타겟이 캐릭터가 아니라면 제외합니다.
	const ASMPlayerCharacter* TargetCharacter = Cast<ASMPlayerCharacter>(TargetActor);
	if (!TargetCharacter)
	{
		return false;
	}

	USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(TargetCharacter->GetAbilitySystemComponent());
	if (!ensureAlways(TargetASC))
	{
		return false;
	}

	// 팀이 없는 경우 대미지를 가하지 않습니다.
	const ESMTeam TargetTeam = TargetCharacter->GetTeam();
	if (TargetTeam == ESMTeam::None)
	{
		return false;
	}

	// 인스티게이터와 같은 팀의 경우 대미지를 가하지 않습니다.
	if (InstigatorTeam == TargetTeam)
	{
		return false;
	}

	// 면역상태인 경우 대미지를 가하지 않습니다.
	if (TargetASC->HasMatchingGameplayTag(SMTags::Character::State::Immune))
	{
		return false;
	}

	return true;
}

void USMCatchInteractionComponent_Character::OnRep_IAmCatchingActor()
{
	// 잡은 대상이 유효하다면 잡기 태그를 붙이고, 유효하지 않거나 nullptr인 경우 잡기 태그를 제거합니다.
	if (IAmCatchingActor.Get())
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->AddTag(SMTags::Character::State::Catch);
		}

		OnCatch.Broadcast();
	}
	else
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			SourceASC->RemoveTag(SMTags::Character::State::Catch);
		}

		/** 잡은 대상을 제거하면 잡기 풀기 델리게이트가 호출됩니다.*/
		OnCatchRelease.Broadcast();
	}
}
