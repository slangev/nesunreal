// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMain.h"

DEFINE_LOG_CATEGORY(LogNesMain);

// Sets default values for this component's properties
UNesMain::UNesMain()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UNesMain::Log(FString msg) {
	UE_LOG(LogNesMain, Log, TEXT("%s"), *msg);
}


// Called when the game starts
void UNesMain::BeginPlay()
{
	Super::BeginPlay();
	ppu = std::unique_ptr<NesPPU>(new NesPPU(256, 240, 4));
	AActor *a = GetOwner();
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(a->FindComponentByClass(UStaticMeshComponent::StaticClass()));
	if(mesh) {
		UMaterialInstanceDynamic * mat = mesh->CreateDynamicMaterialInstance(0, ((UMaterialInterface*)nullptr),FName(TEXT("Dynamic Mat")));
		mat->SetTextureParameterValue(FName(TEXT("TextureInput")),ppu->GetScreen());
	}
	Log(pathToRom);
	TArray<uint8> romData;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*pathToRom))
	{
		FString str = pathToRom+"File does not exist!";
					UE_LOG(LogTemp, Warning, TEXT("%s,  file does not exist!"), *str);
	}
	/* projectDir = FPaths::GameDir();
     projectDir += "Content/Data/Icosahedron_Data.obj";*/
	bool result = FFileHelper::LoadFileToArray(romData,*pathToRom);
	Log(FString::FromInt(romData.Num()));
	for(auto s : romData) {
		Log(FString::FromInt(s));
	}
}




// Called every frame
void UNesMain::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	uint64 cyclesThisUpdate = 0 ; 
    while (cyclesThisUpdate < MAXCYCLES) {
		//gbJoyPad.HandleKeyEvents();
		//uint cycles = nesCPU.Tick();
		cyclesThisUpdate+=4;
		/*gbGraphic.UpdateGraphics(cycles);
		gbAudio.UpdateAudioTimer(cycles);*/
	}
	ppu->RenderStaticByMatrix();
}

