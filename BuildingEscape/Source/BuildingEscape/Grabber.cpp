// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Gameframework/Actor.h"
#include "DrawDebugHelpers.h"

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

/// look for attached physics handle
void UGrabber::FindPhysicsHandleComponent() {
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No physics handle component found on %s"), *(GetOwner()->GetName()));
	}
}

/// look for attached input component (only appears on runtime)
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No input component  found on %s"), *(GetOwner()->GetName()));
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	// if the physics handle is attached
	
	if (PhysicsHandle->GrabbedComponent)
	{
	//	UE_LOG(LogTemp, Warning, TEXT("The distance from Grabbed is %s"), *FString::SanitizeFloat((GetDistanceFromGrabbed())));
		

		float PlayerYaw = GetPlayerViewPoint(); // The player's Yaw is updated here.
		float NewYaw = (PlayerYaw - PlayerCapturedYaw) + GrabbedCapturedYaw; // Your changes in rotation are added to the grabbed object.
		
		//move and turn the object that we are holding
		PhysicsHandle->SetTargetLocation(GetLineTraceStartEnd().v2);
		PhysicsHandle->SetTargetRotation(FRotator(0.0f, NewYaw, 0.0f)); // Grabbed object is made up-right and it's Yaw rotates with the player's.
		
		//get the actor we are holding
		ActorGrabbed = PhysicsHandle->GrabbedComponent->GetOwner();
		
		//if the distance to object is too big - release it
		if (GetDistanceFromGrabbed() > MaxHoldingDistance)
		{
			Release();
		}
	

	}
}

void UGrabber::Grab() {
	///line trace  and reach any actor with physics body collision set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	//take a “snapshot” of theplayer and grabbed actor yaw at the moment you grab
	GrabbedCapturedYaw = ActorHit->GetActorRotation().Yaw;
	PlayerCapturedYaw = GetPlayerViewPoint();
	

	/// if we hit something then attach a phisics handle
	if (ActorHit)
	{
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None, // no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // allow rotation
		);
	}
}

void UGrabber::Release() {
	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// line trace (ray cast) out to reach distance
	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	FTwoVectors Tracepoints = GetLineTraceStartEnd();
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		Tracepoints.v1,
		Tracepoints.v2,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	return HitResult;
}

const FTwoVectors UGrabber::GetLineTraceStartEnd()
{
	FVector StartLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT StartLocation,
		OUT PlayerViewPointRotation
	);
	FVector EndLocation = StartLocation + PlayerViewPointRotation.Vector() * Reach;
	return FTwoVectors(StartLocation,EndLocation);
}

// get the distance from grabbed object to player
float UGrabber::GetDistanceFromGrabbed()
{
	float Distance = 10.f;
	
	if (!ActorGrabbed) { return Distance; }

	//determine the location of the grabbed actor
	FVector GrabbedActorLocation = ActorGrabbed->GetActorLocation();
	// determine the vector difference between actor location and linetrace end
	FVector VectorDifference = GrabbedActorLocation - GetLineTraceStartEnd().v2;

	FVector Direction; //helping variable to use in the ToDirectionAndLength method
	VectorDifference.ToDirectionAndLength(Direction, Distance);

	return Distance;
}

float UGrabber::GetPlayerViewPoint()
{
	FVector StartLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT StartLocation,
		OUT PlayerViewPointRotation
	);
	
	return PlayerViewPointRotation.Yaw;
}