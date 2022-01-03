// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "NesPPU.h"
#include "NesCart.h"
#include "NesCPUMMU.h"
#include "NesPPUMMU.h"
#include "NesCPU.h"
#include <memory>

#include "Camera/CameraComponent.h"
#include "Containers/UnrealString.h"
#include "NesMain.generated.h"

using namespace std;

//https://unrealistic.dev/posts/binding-input-in-c
UCLASS()
class NESUNREAL_API UNesMain final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UNesMain();
	uint64 MAXCYCLES = 29781;
	UPROPERTY(EditAnywhere,Meta = (Bitmask))
	FString pathToRom;

	UPROPERTY(EditAnywhere,Meta = (Bitmask))
    bool bTesting;

	std::shared_ptr<NesPPU> M_Ppu;
	std::shared_ptr<NesCart> M_Cart;
	std::shared_ptr<NesCPUMMU> M_Mmu;
	std::shared_ptr<NesPPUMMU> M_PPUMmu;
	std::unique_ptr<FNesCPU> M_CPU;
	static void Log(FString);
	void Action();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(VisibleDefaultsOnly)
		UCameraComponent *Camera;

	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent *Canvas;

	UPROPERTY(VisibleDefaultsOnly)
		UInputComponent *InputComp;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
