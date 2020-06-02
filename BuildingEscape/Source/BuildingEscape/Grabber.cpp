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


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // if the physics handle is attached
    if (PhysicsHandle->GrabbedComponent) {
        // move the object that we are holding.
        PhysicsHandle->SetTargetLocation(GetReachLineEnd());
    }
}


void UGrabber::Grab()
{
    /// LINE TRACE and see if we reach any actors with physics body collision channel set
    auto HitResult = GetFirstPhysicsBodyInReach();
    auto ComponentToGrab = HitResult.GetComponent();
    auto ActorHit = HitResult.GetActor();
    
    /// If we hit soemthing then attach a physics handle
    if (ActorHit) {
        PhysicsHandle->GrabComponentAtLocation(
            ComponentToGrab,
            NAME_None, // no bones needed.
            ComponentToGrab->GetOwner()->GetActorLocation()
        );
    }
}


void UGrabber::Release()
{
    PhysicsHandle->ReleaseComponent();
}


/// Look for attached Physics Handler
void UGrabber::FindPhysicsHandleComponent()
{
    PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
    if (PhysicsHandle == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("%s missing Physics handle component"), *GetOwner()->GetName())
    }
}


/// Look for attached Input Compinent (only appears at run time)
void UGrabber::SetupInputComponent()
{
    InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
    if (InputComponent)
    {
        InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
        InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
    } else {
        UE_LOG(LogTemp, Error, TEXT("%s missing the Input handle component"), *GetOwner()->GetName())
    }
}


FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
    /// Line Trace (Ray-cast) out to reach distance
    FHitResult HitResult;
    
    /// Setup query parameters - complex collission = false, ignore ourself (owner).
    FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
    
    GetWorld()->LineTraceSingleByObjectType(
        OUT HitResult,
        GetReachLineStart(),
        GetReachLineEnd(),
        FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
        TraceParameters
    );
    return HitResult;
}


FVector UGrabber::GetReachLineStart() const
{
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;
    
    /// Method takes "out params" by ref and updates their values.
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
        OUT PlayerViewPointLocation,
        OUT PlayerViewPointRotation
    );
    
    return PlayerViewPointLocation;
}


FVector UGrabber::GetReachLineEnd() const
{
    FVector PlayerViewPointLocation;
    FRotator PlayerViewPointRotation;
    
    /// Method takes "out params" by ref and updates their values.
    GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
        OUT PlayerViewPointLocation,
        OUT PlayerViewPointRotation
    );
    
    return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}
