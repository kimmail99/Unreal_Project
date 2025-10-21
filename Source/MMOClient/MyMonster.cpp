#include "MyMonster.h"
#include "MyCharacter.h"              // ✅ 추가!
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimSequence.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"


AMyMonster::AMyMonster()
{
    PrimaryActorTick.bCanEverTick = true;

    // === 메쉬 불러오기 ===
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Monster/Meshy_Merged_Animations.Meshy_Merged_Animations"));
    if (MeshAsset.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(MeshAsset.Object);
        GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

        // 지면에 맞춤
        GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()-96.f));
    }

    // 충돌 설정
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->InitCapsuleSize(42.f, 95.f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
}


void AMyMonster::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("MyMonster spawned at (%s)"), *GetActorLocation().ToString());

    if (IdleAnim)
        GetMesh()->PlayAnimation(IdleAnim, true); // 기본 대기 애니메이션 반복
}

void AMyMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AMyCharacter* Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Player) return;

    float Dist = FVector::Dist(Player->GetActorLocation(), GetActorLocation());

    // 플레이어가 가까워지면 공격 애니메이션
    if (Dist < 400.f)
    {
        if (FMath::RandBool())
        {
            if (PunchAnim)
            {
                GetMesh()->PlayAnimation(PunchAnim, false);
                UE_LOG(LogTemp, Warning, TEXT("MyMonster punches!"));
            }
        }
        else
        {
            if (KickAnim)
            {
                GetMesh()->PlayAnimation(KickAnim, false);
                UE_LOG(LogTemp, Warning, TEXT("MyMonster kicks!"));
            }
        }
    }
    else
    {
        // 멀어지면 다시 Idle로 전환
        if (IdleAnim)
        {
            GetMesh()->PlayAnimation(IdleAnim, true);
        }
    }
}

