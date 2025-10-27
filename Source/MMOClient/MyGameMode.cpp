#include "MyGameMode.h"
#include "MyCharacter.h"
#include "MyMonster.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AMyGameMode::AMyGameMode()
{
    DefaultPawnClass = AMyCharacter::StaticClass();
}

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (World)
    {
        // Z값을 더 높여서 지면 위에 확실히 스폰되도록 수정
        FVector SpawnLoc(-1000.f, 0.f, 300.f);
        // 플레이어 방향을 보도록
        FRotator SpawnRot(0.f, 0.f, 0.f);
        
        AMyMonster* Monster = World->SpawnActor<AMyMonster>(AMyMonster::StaticClass(), SpawnLoc, SpawnRot);
        
        if (Monster)
        {
            UE_LOG(LogTemp, Warning, TEXT("Monster spawned successfully at %s"), *SpawnLoc.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn monster!"));
        }
    }
}
