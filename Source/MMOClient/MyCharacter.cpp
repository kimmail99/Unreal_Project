#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "TimerManager.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

    // === Mesh ===
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Character/Meshy_Merged_Animations.Meshy_Merged_Animations"));
    if (MeshAsset.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(MeshAsset.Object);
        GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
        GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    }

    // === Camera ===
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 280.f;
    CameraBoom->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // === Movement ===
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = 400.f;
    bUseControllerRotationYaw = false;

    // === Animations ===
    static ConstructorHelpers::FObjectFinder<UAnimSequence> WalkAnimAsset(TEXT("/Game/Character/Meshy_Merged_Animations_Anim_Walking.Meshy_Merged_Animations_Anim_Walking"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> Skill1Asset(TEXT("/Game/Character/Meshy_Merged_Animations_Anim_Skill_03.Meshy_Merged_Animations_Anim_Skill_03"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> Skill2Asset(TEXT("/Game/Character/Meshy_Merged_Animations_Anim_Spartan_Kick.Meshy_Merged_Animations_Anim_Spartan_Kick"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> Skill3Asset(TEXT("/Game/Character/Meshy_Merged_Animations_Anim_Flying_Fist_Kick.Meshy_Merged_Animations_Anim_Flying_Fist_Kick"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> Skill4Asset(TEXT("/Game/Character/Meshy_Merged_Animations_Anim_Rising_Flying_Kick.Meshy_Merged_Animations_Anim_Rising_Flying_Kick"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> VictoryAsset(TEXT("/Game/Character/Meshy_Merged_Animations_Anim_victory.Meshy_Merged_Animations_Anim_victory"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> DefeatAsset(TEXT("/Game/Character/Meshy_Merged_Animations_Anim_Knock_Down.Meshy_Merged_Animations_Anim_Knock_Down"));

    if (WalkAnimAsset.Succeeded()) WalkAnim = WalkAnimAsset.Object;
    if (Skill1Asset.Succeeded()) Skill1Anim = Skill1Asset.Object;
    if (Skill2Asset.Succeeded()) Skill2Anim = Skill2Asset.Object;
    if (Skill3Asset.Succeeded()) Skill3Anim = Skill3Asset.Object;
    if (Skill4Asset.Succeeded()) Skill4Anim = Skill4Asset.Object;
    if (VictoryAsset.Succeeded()) VictoryAnim = VictoryAsset.Object;
    if (DefeatAsset.Succeeded()) DefeatAnim = DefeatAsset.Object;
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // (0,0,0) 위치에서 시작
    SetActorLocation(FVector(0.f, 0.f, 0.f));

    bIsMoving = false;
    bIsSkillPlaying = false;

    if (WalkAnim)
    {
        GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
        GetMesh()->PlayAnimation(WalkAnim, true);
        GetMesh()->Stop(); // Idle 상태처럼 멈춰있게
    }

    UE_LOG(LogTemp, Warning, TEXT("Player spawned at 0,0,0"));
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSkillPlaying) return;

    FVector Velocity = GetVelocity();
    bool bCurrentlyMoving = Velocity.Size() > 5.f;

    if (bCurrentlyMoving && !bIsMoving)
    {
        bIsMoving = true;
        if (WalkAnim)
            GetMesh()->PlayAnimation(WalkAnim, true);
    }
    else if (!bCurrentlyMoving && bIsMoving)
    {
        bIsMoving = false;
        if (WalkAnim)
        {
            GetMesh()->PlayAnimation(WalkAnim, true);
            GetMesh()->Stop();
        }
    }
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // === 이동 ===
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

    // === 마우스 회전 ===
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);  // ← Axis
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    // === 스킬 ===
    PlayerInputComponent->BindAction("Skill1", IE_Pressed, this, &AMyCharacter::UseSkill1);
    PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &AMyCharacter::UseSkill2);
    PlayerInputComponent->BindAction("Skill3", IE_Pressed, this, &AMyCharacter::UseSkill3);
    PlayerInputComponent->BindAction("Skill4", IE_Pressed, this, &AMyCharacter::UseSkill4);
}

void AMyCharacter::MoveForward(float Value)
{
    if (Controller && Value != 0.f)
    {
        const FRotator YawRot(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AMyCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.f)
    {
        const FRotator YawRot(0, Controller->GetControlRotation().Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AMyCharacter::PlaySkill(UAnimSequence* SkillAnim)
{
    if (!SkillAnim || bIsSkillPlaying) return;

    bIsSkillPlaying = true;
    GetMesh()->PlayAnimation(SkillAnim, false);

    float Duration = SkillAnim->GetPlayLength();
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(
        Handle,
        [this]()
        {
            bIsSkillPlaying = false;
            if (WalkAnim)
            {
                GetMesh()->PlayAnimation(WalkAnim, true);
                GetMesh()->Stop();
            }
        },
        Duration,
        false
    );
}

void AMyCharacter::UseSkill1() { PlaySkill(Skill1Anim); }
void AMyCharacter::UseSkill2() { PlaySkill(Skill2Anim); }
void AMyCharacter::UseSkill3() { PlaySkill(Skill3Anim); }
void AMyCharacter::UseSkill4() { PlaySkill(Skill4Anim); }

