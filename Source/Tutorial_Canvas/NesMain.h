// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NesPPU.h"
#include "NesCart.h"
#include <memory>
#include "Containers/UnrealString.h"
#include "NesMain.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesMain, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TUTORIAL_CANVAS_API UNesMain : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UNesMain();
	uint64 MAXCYCLES = 29781;
	UPROPERTY(EditAnywhere,Meta = (Bitmask))
	FString pathToRom;
	std::unique_ptr<NesPPU> ppu;
	std::unique_ptr<NesCart> cart;
	void Log(FString);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
