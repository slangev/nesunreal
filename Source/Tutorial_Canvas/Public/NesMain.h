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
class TUTORIAL_CANVAS_API UNesMain : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNesMain();
	uint64 MAXCYCLES = 29781;
	UPROPERTY(EditAnywhere,Meta = (Bitmask))
	FString pathToRom;
	std::unique_ptr<NesPPU> m_ppu;
	std::unique_ptr<NesCart> m_cart;
	std::shared_ptr<NesMMU> m_mmu;
	std::unique_ptr<NesCPU> m_cpu;
	void Log(FString);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
