// Copyright Surround, Inc. All Rights Reserved.


#include "SMBassCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Utilities/SMLog.h"


ASMBassCharacter::ASMBassCharacter()
{
	SlashColliderRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SlashColliderRootComponent"));
	SlashColliderRootComponent->SetupAttachment(RootComponent);

	SlashColliderComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SlashColliderComponent"));
	SlashColliderComponent->SetupAttachment(SlashColliderRootComponent);
	SlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	const double HalfDistance = 250.0;
	SlashColliderComponent->SetRelativeLocationAndRotation(FVector(HalfDistance, 0.0, 0.0), FRotator(-90.0, 0.0, 0.0));
	SlashColliderComponent->InitCapsuleSize(50.0f, HalfDistance);
	SlashColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSlashOverlap);
}

void ASMBassCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASMBassCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (SlashData.bIsSlashing)
	{
		UpdateSlash();
	}
}

void ASMBassCharacter::Slash(float Distance, float Angle, float SlashTime)
{
	if (SlashData.bIsForward)
	{
		SlashData.bIsForward = false;
	}
	else
	{
		SlashData.bIsForward = true;
	}


	SlashData.HalfAngle = Angle / 2.0f;
	SlashData.SlashSpeed = Angle * (1.0f / SlashTime);
	SlashColliderComponent->SetCapsuleHalfHeight(Distance / 2.0f);
	SlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	const double Yaw = SlashData.bIsForward ? -SlashData.HalfAngle : SlashData.HalfAngle;
	SlashColliderRootComponent->SetRelativeRotation(FRotator(0.0, Yaw, 0.0));
	SlashData.bIsSlashing = true;
}

void ASMBassCharacter::UpdateSlash()
{
	const FRotator SourceRotation = SlashColliderRootComponent->GetRelativeRotation();
	const double TargetYaw = SlashData.bIsForward ? SlashData.HalfAngle : -SlashData.HalfAngle;
	const double NewYaw = FMath::FInterpConstantTo(SourceRotation.Yaw, TargetYaw, GetWorld()->GetDeltaSeconds(), SlashData.SlashSpeed);
	SlashColliderRootComponent->SetRelativeRotation(FRotator(0.0, NewYaw, 0.0));

	DrawDebugCapsule(GetWorld(), SlashColliderComponent->GetComponentLocation(), SlashColliderComponent->GetScaledCapsuleHalfHeight(), SlashColliderComponent->GetScaledCapsuleRadius(), SlashColliderComponent->GetComponentRotation().Quaternion(), FColor::Red, false, 0.5f);

	if (FMath::IsNearlyEqual(SourceRotation.Yaw, NewYaw))
	{
		SlashEnd();
	}
}

void ASMBassCharacter::SlashEnd()
{
	SlashColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SlashData.DetectedActors.Reset();
	SlashData.bIsSlashing = false;

	(void)OnSlashEndSignature.ExecuteIfBound();
}

void ASMBassCharacter::OnSlashOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}

	ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(OtherActor);
	if (TargetCharacter)
	{
		if (SlashData.DetectedActors.Find(TargetCharacter) == INDEX_NONE)
		{
			SlashData.DetectedActors.Push(TargetCharacter);
			ApplyDamage(TargetCharacter);
		}
	}
}

void ASMBassCharacter::ApplyDamage(AActor* TargetActor)
{
	NET_VLOG(this, -1, 3.0f, TEXT("%s 피격"), *TargetActor->GetName());
}
