#include "MyMonster.h"
#include "MyCharacter.h"
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
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Monster/Meshy_Merged_Animations__1_.Meshy_Merged_Animations__1_"));
    if (MeshAsset.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(MeshAsset.Object);
        GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
        // 메쉬를 캡슐 바닥에 맞춤
        GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -290.f));
        
        // 몬스터 크기 2배로 키우기 (X, Y, Z 모두)
        GetMesh()->SetWorldScale3D(FVector(2.f, 2.f, 2.f));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Monster Mesh!"));
    }

    // === 애니메이션 불러오기 ===
    static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleAnimAsset(TEXT("/Game/Monster/Meshy_Merged_Animations__1__Anim_Walking.Meshy_Merged_Animations__1__Anim_Walking"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> PunchAnimAsset(TEXT("/Game/Monster/Meshy_Merged_Animations__1__Anim_Punch_Forward_with_Both_Fists.Meshy_Merged_Animations__1__Anim_Punch_Forward_with_Both_Fists"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> KickAnimAsset(TEXT("/Game/Monster/Meshy_Merged_Animations__1__Anim_Large_step_then_high_kick.Meshy_Merged_Animations__1__Anim_Large_step_then_high_kick"));

    if (IdleAnimAsset.Succeeded())
    {
        IdleAnim = IdleAnimAsset.Object;
        UE_LOG(LogTemp, Warning, TEXT("Monster Idle animation loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Monster Idle animation"));
    }
    
    if (PunchAnimAsset.Succeeded())
    {
        PunchAnim = PunchAnimAsset.Object;
        UE_LOG(LogTemp, Warning, TEXT("Monster Punch animation loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Monster Punch animation"));
    }
    
    if (KickAnimAsset.Succeeded())
    {
        KickAnim = KickAnimAsset.Object;
        UE_LOG(LogTemp, Warning, TEXT("Monster Kick animation loaded"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load Monster Kick animation"));
    }

    // 충돌 설정
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->InitCapsuleSize(42.f, 95.f);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

    // 초기화
    bIsAttacking = false;
    DetectRange = 400.f;
    AttackCooldown = 2.0f;
    TimeSinceLastAttack = 0.f;
}


void AMyMonster::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("MyMonster spawned at (%s)"), *GetActorLocation().ToString());

    // 애니메이션 모드 설정
    if (GetMesh())
    {
        GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    }

    // 기본 Idle 애니메이션 재생
    PlayAnimationSafe(IdleAnim, true);
}

void AMyMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 공격 쿨다운 업데이트
    TimeSinceLastAttack += DeltaTime;

    // 이미 공격 중이면 스킵
    if (bIsAttacking) return;

    // 플레이어 찾기
    AMyCharacter* Player = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Player) return;

    float Dist = FVector::Dist(Player->GetActorLocation(), GetActorLocation());

    // 플레이어가 감지 범위 내에 있고, 쿨다운이 끝났으면 공격
    if (Dist < DetectRange && TimeSinceLastAttack >= AttackCooldown)
    {
        PerformAttack();
    }
}

void AMyMonster::PerformAttack()
{
    bIsAttacking = true;
    TimeSinceLastAttack = 0.f;

    // 랜덤하게 펀치 또는 킥 선택
    UAnimSequence* AttackAnim = FMath::RandBool() ? PunchAnim : KickAnim;
    
    if (AttackAnim)
    {
        PlayAnimationSafe(AttackAnim, false);
        UE_LOG(LogTemp, Warning, TEXT("MyMonster attacks!"));

        // 애니메이션 끝나면 Idle로 돌아가기
        float Duration = AttackAnim->GetPlayLength();
        FTimerHandle Handle;
        GetWorld()->GetTimerManager().SetTimer(
            Handle,
            [this]()
            {
                bIsAttacking = false;
                PlayAnimationSafe(IdleAnim, true);
            },
            Duration,
            false
        );
    }
    else
    {
        bIsAttacking = false;
    }
}

void AMyMonster::PlayAnimationSafe(UAnimSequence* Anim, bool bLoop)
{
    if (Anim && GetMesh())
    {
        GetMesh()->PlayAnimation(Anim, bLoop);
    }
}
