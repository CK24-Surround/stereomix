// Copyright Surround, Inc. All Rights Reserved.


#include "SMSlashComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "Characters/SMBassCharacter.h"
#include "Components/CapsuleComponent.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "Utilities/SMLog.h"


USMSlashComponent::USMSlashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void USMSlashComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void USMSlashComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bNeedUpdateSlashing)
	{
		UpdateSlash();
	}

	if (bCanNextAction && bIsInput)
	{
		bCanNextAction = false;
		bIsInput = false;

		ReSlash();
	}
}

void USMSlashComponent::BeginPlay()
{
	Super::BeginPlay();

	SourceCharacter = GetOwner<ASMBassCharacter>();
	if (ensureAlways(SourceCharacter.Get()))
	{
		SourceSlashColliderRootComponent = SourceCharacter->GetSlashColliderRootComponent();
		ensureAlways(SourceSlashColliderRootComponent.Get());

		SourceSlashColliderComponent = SourceCharacter->GetSlashColliderComponent();
		if (ensureAlways(SourceSlashColliderComponent.Get()))
		{
			SourceSlashColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::USMSlashComponent::OnSlashOverlap);
		}
	}
}

void USMSlashComponent::TrySlash()
{
	if (!bIsSlashing) // 베기가 시작되지 않은상태라면 즉시 시작합니다.
	{
		bIsSlashing = true;
		SlashStart();
	}
	else
	{
		if (bCanInput)
		{
			NET_LOG(SourceCharacter.Get(), Warning, TEXT("입력"));
			bCanInput = false;
			bIsInput = true;
		}
	}
}

void USMSlashComponent::ColliderOrientaionForSlash()
{
	if (!SourceCharacter)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = SourceCharacter->GetAbilitySystemComponent();
	if (!SourceASC)
	{
		return;
	}

	if (SourceASC->HasAnyMatchingGameplayTags(DeactivateGameplayTags))
	{
		return;
	}

	if (!ensureAlways(SourceSlashColliderRootComponent.Get()))
	{
		return;
	}

	if (!ensureAlways(SourceSlashColliderComponent.Get()))
	{
		return;
	}

	HalfAngle = Angle / 2.0f;
	SlashSpeed = Angle * (1.0f / SlashTime);
	SourceSlashColliderComponent->SetCapsuleHalfHeight(Distance / 2.0f);
	SourceSlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	const double Yaw = bIsLeftSlashNext ? HalfAngle : -HalfAngle;
	SourceSlashColliderRootComponent->SetRelativeRotation(FRotator(0.0, Yaw, 0.0));
	bNeedUpdateSlashing = true;
}

void USMSlashComponent::SlashStart()
{
	UAnimInstance* SourceAnimInstance = GetSourceAnimInstance();
	if (!ensureAlways(SourceAnimInstance))
	{
		return;
	}

	SourceAnimInstance->Montage_Play(SlashMontage);
	FOnMontageEnded* MontageEnded = SourceAnimInstance->Montage_GetEndedDelegate(SlashMontage);
	if (MontageEnded)
	{
		MontageEnded->BindUObject(this, &ThisClass::SlashEnded);
	}

	ServerRPCPlaySlashStartAnimation();
}

void USMSlashComponent::ReSlash()
{
	UAnimInstance* SourceAnimInstance = GetSourceAnimInstance();
	if (!ensureAlways(SourceAnimInstance))
	{
		return;
	}

	const FName SectionName = bIsLeftSlashNext ? TEXT("Left") : TEXT("Right");
	NET_LOG(SourceCharacter.Get(), Warning, TEXT("%s로 점프시도"), *SectionName.ToString());
	SourceAnimInstance->Montage_JumpToSection(SectionName, SlashMontage);

	ServerRPCPlayReSlashAnimation(bIsLeftSlashNext);
}

void USMSlashComponent::UpdateSlash()
{
	const FRotator SourceRotation = SourceSlashColliderRootComponent->GetRelativeRotation();
	const double TargetYaw = bIsLeftSlashNext ? -HalfAngle : HalfAngle;
	const double NewYaw = FMath::FInterpConstantTo(SourceRotation.Yaw, TargetYaw, GetWorld()->GetDeltaSeconds(), SlashSpeed);
	SourceSlashColliderRootComponent->SetRelativeRotation(FRotator(0.0, NewYaw, 0.0));

	if (bShowDebug)
	{
		DrawDebugCapsule(GetWorld(), SourceSlashColliderComponent->GetComponentLocation(), SourceSlashColliderComponent->GetScaledCapsuleHalfHeight(), SourceSlashColliderComponent->GetScaledCapsuleRadius(), SourceSlashColliderComponent->GetComponentRotation().Quaternion(), FColor::Red, false, 0.5f);
	}

	if (FMath::IsNearlyEqual(SourceRotation.Yaw, NewYaw))
	{
		SourceSlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetectedActors.Reset();
		bNeedUpdateSlashing = false;
	}
}

void USMSlashComponent::SlashEnded(UAnimMontage* AnimMontage, bool bArg)
{
	NET_LOG(SourceCharacter.Get(), Warning, TEXT("애니메이션 종료"));
	bIsSlashing = false;
	bIsLeftSlashNext = true;
}

void USMSlashComponent::OnSlashOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == SourceCharacter)
	{
		return;
	}

	if (!IsValidTarget(OtherActor))
	{
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(OtherActor);
	if (TargetCharacter)
	{
		if (DetectedActors.Find(TargetCharacter) == INDEX_NONE)
		{
			DetectedActors.Push(TargetCharacter);
			PredictApplyDamage(TargetCharacter);
			ServerRPCRequestDamage(TargetCharacter, Damage);
		}
	}
}

void USMSlashComponent::PredictApplyDamage(AActor* TargetActor)
{
	NET_VLOG(SourceCharacter.Get(), -1, 3.0f, TEXT("%s 피격"), *TargetActor->GetName());

	ASMPlayerCharacterBase* TagetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	TagetCharacter->PredictHPChange(Damage);
}

UAnimInstance* USMSlashComponent::GetSourceAnimInstance()
{
	if (!SourceCharacter.Get())
	{
		return nullptr;
	}

	USkeletalMeshComponent* SourceMesh = SourceCharacter->GetMesh();
	if (!SourceMesh)
	{
		return nullptr;
	}

	UAnimInstance* SourceAnimInstance = SourceMesh->GetAnimInstance();
	if (!SourceAnimInstance)
	{
		return nullptr;
	}

	return SourceAnimInstance;
}

bool USMSlashComponent::IsValidTarget(AActor* TargetActor)
{
	ESMTeam SourceTeam = USMTeamBlueprintLibrary::GetTeam(GetOwner());
	ESMTeam TargetTeam = USMTeamBlueprintLibrary::GetTeam(TargetActor);
	if (SourceTeam == TargetTeam)
	{
		return false;
	}

	return true;
}

void USMSlashComponent::ServerRPCPlaySlashStartAnimation_Implementation()
{
	MulticastRPCPlaySlashStartAnimation();
}

void USMSlashComponent::MulticastRPCPlaySlashStartAnimation_Implementation()
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	if (SourceCharacter->IsLocallyControlled())
	{
		return;
	}

	UAnimInstance* SourceAnimInstance = GetSourceAnimInstance();
	if (!ensureAlways(SourceAnimInstance))
	{
		return;
	}

	SourceAnimInstance->Montage_Play(SlashMontage);
}


void USMSlashComponent::ServerRPCPlayReSlashAnimation_Implementation(bool bIsLeftSlash)
{
	MulticastRPCPlaySlashAnimation(bIsLeftSlash);
}

void USMSlashComponent::MulticastRPCPlaySlashAnimation_Implementation(bool bIsLeftSlash)
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	if (SourceCharacter->IsLocallyControlled())
	{
		return;
	}

	UAnimInstance* SourceAnimInstance = GetSourceAnimInstance();
	if (!ensureAlways(SourceAnimInstance))
	{
		return;
	}

	const FName SectionName = bIsLeftSlash ? TEXT("Left") : TEXT("Right");
	SourceAnimInstance->Montage_JumpToSection(SectionName, SlashMontage);
}

void USMSlashComponent::ServerRPCRequestDamage_Implementation(AActor* TargetActor, float Amount)
{
	if (!SourceCharacter.Get())
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = SourceCharacter->GetAbilitySystemComponent();
	if (!ensureAlways(SourceASC))
	{
		return;
	}

	FGameplayEffectSpecHandle GESpecHandle = SourceASC->MakeOutgoingSpec(SlashDamageGE, 1.0f, SourceASC->MakeEffectContext());
	if (!GESpecHandle.IsValid())
	{
		return;
	}

	FGameplayEffectSpec* GESpec = GESpecHandle.Data.Get();
	GESpec->SetSetByCallerMagnitude(SMTags::Data::Damage, Damage);
	SourceASC->BP_ApplyGameplayEffectSpecToTarget(GESpecHandle, TargetASC);
}
