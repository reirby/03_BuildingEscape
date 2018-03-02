// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include <math.h>
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Gameframework/Actor.h"

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
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing pressure plate"), *(GetOwner()->GetName()));
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the trigger volume 
	if (GetTotalMassOfActorsOnPLate() == TriggerMass) 
	{
		OnOpen.Broadcast();
	}
	else
	{
		OnClose.Broadcast();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPLate()
{
	float TotalMass = 0.f;

	//find the overlaping actors
	TArray<AActor*> OverlapingActors;
	if (!PressurePlate) { return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlapingActors);

	// iterate through them addig their masses
	for (const auto* Actor : OverlapingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	//	UE_LOG(LogTemp, Warning, TEXT("%s is in a trigger volume"), *Actor->GetName());
	//	FString rep = FString::SanitizeFloat(round(TotalMass));
	//	UE_LOG(LogTemp, Warning, TEXT("TotalMass is %s "), *rep);
	}
	//UE_LOG(LogTemp, Warning, TEXT("total mass is %s"), *std::to_string(TotalMass));

	return round(TotalMass);
}