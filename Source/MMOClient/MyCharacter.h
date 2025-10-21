#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class MMOCLIENT_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    // === 카메라 ===
    UPROPERTY(VisibleAnywhere)
    class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* FollowCamera;

    // === 애니메이션 ===
    UPROPERTY()
    class UAnimSequence* WalkAnim;
    UPROPERTY()
    class UAnimSequence* Skill1Anim;
    UPROPERTY()
    class UAnimSequence* Skill2Anim;
    UPROPERTY()
    class UAnimSequence* Skill3Anim;
    UPROPERTY()
    class UAnimSequence* Skill4Anim;
    UPROPERTY()
    class UAnimSequence* VictoryAnim;
    UPROPERTY()
    class UAnimSequence* DefeatAnim;

    // === 상태 ===
    bool bIsMoving;
    bool bIsSkillPlaying;

    // === 입력 ===
    void MoveForward(float Value);
    void MoveRight(float Value);
    void PlaySkill(UAnimSequence* SkillAnim);
    void UseSkill1();
    void UseSkill2();
    void UseSkill3();
    void UseSkill4();
};

