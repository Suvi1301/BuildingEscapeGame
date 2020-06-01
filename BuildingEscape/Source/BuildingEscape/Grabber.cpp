// Copyright Suvineet Singh 2020


#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "WorldCollision.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
    
    FindPhysicsHandleComponent();
    SetupInputComponent();
	
}

void UGrabber::Grab()
{
    UE_LOG(LogTemp, Warning, TEXT("Grab key pressed!"))
    
    /// LINE TRACE and see if we reach any actors with physics body collision channel set
    GetFirstPhysicsBodyInReach();
    
    /// If we hit soemthing then attach a physics handle
    /// TODO: attach physics handle
}

void UGrabber::Release()
{
    UE_LOG(LogTemp, Warning, TEXT("Grab key released!"))
    /// TODO: Release the physics handle
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // if the physics handle is attached
        // move the object that we are holding.
}

/// Look for attached Physics Handler
void UGrabber::FindPhysicsHandleComponent()
{
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
    if (PhysicsHandle) {

    } else {
        UE_LOG(LogTemp, Error, TEXT("%s missing Physics handle component"), *GetOwner()->GetName())
    }
}


/// Look for attached Input Compinent (only appears at run time)
void UGrabber::SetupInputComponent()
{
    InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
    if (InputComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Input component found!"))
        
        /// Bind the input action
        InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
        InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
        
    } else {
        UE_LOG(LogTemp, Error, TEXT("%s missing the Input handle component"), *GetOwner()->GetName())
    }
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
    /// Get the player viewpoint this tick
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;
    
    /// Method takes "out params" by ref and updates their values.
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
        OUT PlayerViewPointLocation,
        OUT PlayerViewPointRotation
    );
    
    FVector LineTraceEnd = PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);

    // Draw a red trace in the world to visualise.
    DrawDebugLine(
        GetWorld(),
        PlayerViewPointLocation,
        LineTraceEnd,
        FColor(255, 0, 0),
        false,
        0.f,
        0.f,
        10.f
    );
    
    /// Setup query parameters
    // complex collission = false, ignore ourself (owner).
    FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
    
    /// Line Trace (Ray-cast) out to reach distance
    FHitResult Hit;
    GetWorld()->LineTraceSingleByObjectType(
        OUT Hit,
        PlayerViewPointLocation,
        LineTraceEnd,
        FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
        TraceParameters
    );
    
    /// See what we hit
    AActor* ActorHit = Hit.GetActor();
    if (ActorHit) {
        UE_LOG(LogTemp, Warning, TEXT("We just hit %s"), *(ActorHit->GetName()))
    }
    
    return Hit;
}
