// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NesApu.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "Materials/Material.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NesPPU.h"
#include "NesCart.h"
#include "NesCPUMMU.h"
#include "NesPPUMMU.h"
#include "NesController.h"
#include "NesCPU.h"
#include <memory>

#include "Camera/CameraComponent.h"
#include "Containers/UnrealString.h"
#include "NesMain.generated.h"

using namespace std;

//https://unrealistic.dev/posts/binding-input-in-c
UCLASS()
class NESUNREAL_API ANesMain final : public APawn
{
	GENERATED_BODY()

public:	
	ANesMain();
	uint64 MAXCYCLES = 29781;
	UPROPERTY(EditAnywhere,Meta = (Bitmask))
	FString pathToRom = "/Users/slangev/nesunreal/Content/Data/DK.nes";

	UPROPERTY(EditAnywhere,Meta = (Bitmask))
    bool bTesting;

	UPROPERTY(EditAnywhere,Meta = (Bitmask))
    bool bLoggingCPU;

	std::shared_ptr<NesPPU> M_Ppu;
	std::shared_ptr<NesCart> M_Cart;
	std::shared_ptr<NesCPUMMU> M_Mmu;
	std::shared_ptr<NesPPUMMU> M_PpuMmu;
	std::unique_ptr<FNesCPU> M_Cpu;
	std::shared_ptr<NesController> M_Controller;
	
	static void Log(FString);
	void PressedUp();
	void ReleasedUp();
	void PressedLeft();
	void ReleasedLeft();
	void PressedRight();
	void ReleasedRight();
	void PressedDown();
	void ReleasedDown();
	void PressedA();
	void ReleasedA();
	void PressedB();
	void ReleasedB();
	void PressedStart();
	void ReleasedStart();
	void PressedSelect();
	void ReleasedSelect();

	void Print()
	{
		UE_LOG(LogTemp,Warning,TEXT("HERE!"));
	}
	
	// Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleAnywhere)
    USpringArmComponent *M_SpringArm;
    UPROPERTY(VisibleAnywhere)
    UCameraComponent *M_Camera;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent *M_Screen;
	UPROPERTY(VisibleAnywhere)
	UNesApu *M_Sound;



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
