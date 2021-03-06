// Copyright Suvineet Singh 2020


#include "OpenDoor.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
    Owner = GetOwner();
    
    if (!Owner) {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get Owner in OpenDoor."), *Owner->GetName())
    }
    
    if (!PressurePlate) {
        UE_LOG(LogTemp, Warning, TEXT("%s Missing PressurePlate."), *Owner->GetName())
    }
}


void UOpenDoor::OpenDoor()
{
    if (!Owner) { return; }
    // pitch, yaw, roll.
    Owner->SetActorRotation(FRotator(0.f, OpenAngle, 0.f));
}


void UOpenDoor::CloseDoor()
{
    if (!Owner) { return; }
    Owner->SetActorRotation(FRotator(0.f, 0.f, 0.f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the Trigger Volume
    if (GetTotalMassOfActorsOnPlate() > TriggerMass)
    {
        OpenDoor();
        LastDoorOpenTime = GetWorld()->GetTimeSeconds();
    }
    
    // Check if its time to close the door.
    if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime > DoorCloseDelay)
        CloseDoor();
}


float UOpenDoor::GetTotalMassOfActorsOnPlate() const
{
    float TotalMass = 0.f;
    
    // Find all the overlapping actors
    TArray<AActor*> ActorsOnPlate;
    
    if (!PressurePlate) { return TotalMass; }
    
    PressurePlate->GetOverlappingActors(OUT ActorsOnPlate);

    // Iterate through them adding their masses.
    for (const AActor* Actor : ActorsOnPlate) {
        TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
        UE_LOG(LogTemp, Warning, TEXT("%s on pressure plate"), *Actor->GetName())
    }
    
    return TotalMass;
}
