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
        FVector SpawnLoc(-1000.f, 0.f, 100.f);
        FRotator SpawnRot(0.f, 90.f, 0.f);
        World->SpawnActor<AMyMonster>(AMyMonster::StaticClass(), SpawnLoc, SpawnRot);
    }
}

