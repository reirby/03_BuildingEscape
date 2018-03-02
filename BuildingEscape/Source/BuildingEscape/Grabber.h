// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"

#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	// how far ahead of the player can we reach in cm
	float Reach = 100.f;
	
	//How far the grabbed object might be, before autorelease
	UPROPERTY(EditAnywhere)
	float MaxHoldingDistance = 130.f;

	// Actor which was grabbed
	AActor* ActorGrabbed = nullptr;

	// Used for making grabbed item always upright and rotate with you
	float GrabbedCapturedYaw = 0.0f; // Yaw of the Grabbed item is captured here the moment it's grabbed.
	float PlayerCapturedYaw = 0.0f; // Yaw of the Player is captured here the moment you grab something.
	
	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	UInputComponent* InputComponent = nullptr;

	//Raycast and grab what's in reach
	void Grab();

	// called when Grab is released
	void Release();

	// find attached physics
	void FindPhysicsHandleComponent();

	//setup attached input component
	void SetupInputComponent();

	//return hit for firsth physics body in reach
	const FHitResult GetFirstPhysicsBodyInReach();

	 // return current start & end of reach line
	 const FTwoVectors GetLineTraceStartEnd();

	 // return the distance from grabbed object to player
	 float GetDistanceFromGrabbed();

	 // return  player view point rotation yaw
	 float GetPlayerViewPoint();
};
