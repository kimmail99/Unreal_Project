#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyMonster.generated.h"

UCLASS()
class MMOCLIENT_API AMyMonster : public ACharacter
{
    GENERATED_BODY()

public:
    AMyMonster();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    // === 애니메이션 ===
    UPROPERTY()
    class UAnimSequence* IdleAnim;

    UPROPERTY()
    class UAnimSequence* PunchAnim;

    UPROPERTY()
    class UAnimSequence* KickAnim;

    // === 플레이어 참조 ===
    class AMyCharacter* PlayerRef;

    // === 전투 관련 ===
    float DetectRange;
    bool bIsAttacking;
    float AttackCooldown;
    float TimeSinceLastAttack;

    // === 함수 ===
    void PerformAttack();
    void PlayAnimationSafe(UAnimSequence* Anim, bool bLoop = false);
};

