// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NesPPU.h"
#include "NesCart.h"
#include "NesMMU.h"
#include "NesCPU.h"
#include <memory>
#include "Containers/UnrealString.h"
#include "NesMain.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogNesMain, Log, All);
using namespace std;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TUTORIAL_CANVAS_API UNesMain final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNesMain();
	uint64 MAXCYCLES = 29781;
	UPROPERTY(EditAnywhere,Meta = (Bitmask))
	FString pathToRom;

	UPROPERTY(EditAnywhere,Meta = (Bitmask))
    bool bTesting;

	std::unique_ptr<NesPPU> M_Ppu;
	std::unique_ptr<NesCart> M_Cart;
	std::shared_ptr<NesMMU> M_Mmu;
	std::unique_ptr<FNesCPU> M_CPU;
	static void Log(FString);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
