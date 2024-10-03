// Copyright Surround, Inc. All Rights Reserved.


#include "SMJumpPad.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

ASMJumpPad::ASMJumpPad()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneComponent);
	BoxComponent->SetCollisionProfileName(SMCollisionProfileName::Gimmick);
	BoxComponent->InitBoxExtent(FVector(150.0, 150.0, 50.0));
	BoxComponent->SetRelativeLocation(FVector(0.0, 0.0, 50.0));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(BoxComponent);
	MeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	MeshComponent->SetRelativeLocationAndRotation(FVector(0.0, 0.0, -50.0), FRotator(0.0, 90.0, 0.0));

	JumpTarget = CreateDefaultSubobject<USceneComponent>(TEXT("JumpTarget"));
	JumpTarget->SetupAttachment(SceneComponent);

	DenineTags.AddTag(SMTags::Character::State::Catch);
	DenineTags.AddTag(SMTags::Character::State::Stun);
}

void ASMJumpPad::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMJumpPad::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Super::NotifyActorBeginOverlap(OtherActor);
	//
	// ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(OtherActor);
	// if (!TargetCharacter)
	// {
	// 	return;
	// }
	//
	// if (TargetCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	// {
	// 	return;
	// }
	//
	// USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetCharacter));
	// if (!ensureAlways(TargetASC))
	// {
	// 	return;
	// }
	//
	// if (TargetASC->HasAnyMatchingGameplayTags(DenineTags))
	// {
	// 	return;
	// }
	//
	// Jump(TargetCharacter, JumpTarget->GetComponentLocation());
	// TargetCharacter->OnCharacterLanded.AddUObject(this, &ThisClass::TargetLanded);
	//
	// // 점프대 사용 FX를 재생합니다. 로컬에서는 예측적으로 FX를 재생합니다.
	// FGameplayCueParameters GCParams;
	// GCParams.Location = GetActorLocation();
	// UGameplayCueManager::ExecuteGameplayCue_NonReplicated(this, SMTags::GameplayCue::JumpPad::UseJumpPad, GCParams);
	//
	// if (HasAuthority())
	// {
	// 	MulticastRPCPlayUseJumpPadFX(TargetCharacter);
	// 	TargetASC->AddTag(SMTags::Character::State::Jump);
	// }
}

void ASMJumpPad::Jump(ACharacter* InCharacter, const FVector& TargetLocation)
{
	// const FVector StartLocation = InCharacter->GetActorLocation();
	// UCharacterMovementComponent* TargetMovement = InCharacter->GetCharacterMovement();
	// if (!ensureAlways(TargetMovement))
	// {
	// 	return;
	// }
	//
	// OriginAirControl.Add(InCharacter, TargetMovement->AirControl);
	// TargetMovement->AirControl = 0.0f;
	//
	// OriginGravityScale.Add(InCharacter, TargetMovement->GravityScale);
	// TargetMovement->GravityScale = GravityScale;
	// const float Gravity = TargetMovement->GetGravityZ();
	//
	// const FVector LaunchVelocity = USMCalculateBlueprintLibrary::SuggestProjectileVelocity_CustomApexHeight2(this, StartLocation, TargetLocation, ApexHeight, Gravity);
	//
	// InCharacter->LaunchCharacter(LaunchVelocity, true, true);
	// InCharacter->OnDestroyed.AddDynamic(this, &ThisClass::OnDestroyedUsingJumpPadCharacter);
}

void ASMJumpPad::TargetLanded(ASMPlayerCharacterBase* LandedCharacter)
{
	// if (!LandedCharacter)
	// {
	// 	NET_LOG(this, Warning, TEXT("착지하는 캐릭터가 유효하지 않습니다."));
	// 	return;
	// }
	//
	// USMAbilitySystemComponent* TargetASC = Cast<USMAbilitySystemComponent>(LandedCharacter->GetAbilitySystemComponent());
	// if (!ensureAlways(TargetASC))
	// {
	// 	return;
	// }
	//
	// UCharacterMovementComponent* TargetMovement = LandedCharacter->GetCharacterMovement();
	// if (!ensureAlways(TargetMovement))
	// {
	// 	return;
	// }
	//
	// if (HasAuthority())
	// {
	// 	TargetASC->RemoveTag(SMTags::Character::State::Jump);
	// }
	//
	// if (OriginGravityScale.Find(LandedCharacter))
	// {
	// 	TargetMovement->GravityScale = OriginGravityScale[LandedCharacter];
	// }
	//
	// if (OriginAirControl.Find(LandedCharacter))
	// {
	// 	TargetMovement->AirControl = OriginAirControl[LandedCharacter];
	// }
	//
	// LandedCharacter->OnDestroyed.RemoveAll(this);
	// LandedCharacter->OnCharacterLanded.RemoveAll(this);
	//
	// // 착지 FX를 재생합니다. 로컬에서는 예측적으로 먼저 재생합니다.
	// FGameplayCueParameters GCParams;
	// GCParams.Location = LandedCharacter->GetActorLocation();
	// UGameplayCueManager::ExecuteGameplayCue_NonReplicated(this, SMTags::GameplayCue::JumpPad::Land, GCParams);
	//
	// if (HasAuthority())
	// {
	// 	MulticastRPCPlayLandedFX(LandedCharacter);
	// }
}

void ASMJumpPad::OnDestroyedUsingJumpPadCharacter(AActor* DestroyedActor)
{
	// DestroyedActor->OnDestroyed.RemoveAll(this);
}

void ASMJumpPad::MulticastRPCPlayUseJumpPadFX_Implementation(ASMPlayerCharacterBase* SourceCharacter)
{
	// if (!HasAuthority() && !SourceCharacter->IsLocallyControlled())
	// {
	// 	FGameplayCueParameters GCParams;
	// 	GCParams.Location = GetActorLocation();
	// 	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(this, SMTags::GameplayCue::JumpPad::Land, GCParams);
	// }
}

void ASMJumpPad::MulticastRPCPlayLandedFX_Implementation(ASMPlayerCharacterBase* SourceCharacter)
{
	// if (!HasAuthority() && !SourceCharacter->IsLocallyControlled())
	// {
	// 	FGameplayCueParameters GCParams;
	// 	GCParams.Location = SourceCharacter->GetActorLocation();
	// 	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(this, SMTags::GameplayCue::JumpPad::Land, GCParams);
	// }
}
