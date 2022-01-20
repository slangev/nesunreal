
// Fill out your copyright notice in the Description page of Project Settings.


#include "NesMain.h"
#include <iomanip>

DEFINE_LOG_CATEGORY_STATIC(LogNesMain,Log,All)

ANesMain::ANesMain()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
    UStaticMesh* Asset = MeshAsset.Object; 

    static ConstructorHelpers::FObjectFinder<UMaterial>MaterialAsset(TEXT("Material'/Game/Materials/CanvasMaterial.CanvasMaterial'"));
    UMaterial* CanvasMaterial = MaterialAsset.Object; 

    m_screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyScreen"));
    // Setup the spring arm that the camera will attach to.
    m_springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MySpringArm"));
    SetRootComponent(m_springArm);
    m_springArm->SetRelativeRotation(FVector(0.0f, 0.0f, 0.0f).Rotation());
    m_springArm->SetAbsolute(false, false, false);
    m_springArm->TargetArmLength = 100.f;

    m_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCamera"));
    m_camera->SetupAttachment(m_springArm);
    m_camera->SetWorldRotation(FVector(0.0f, 0.0f, 0.0f).Rotation());
    m_camera->ProjectionMode = ECameraProjectionMode::Perspective;

	M_Sound = CreateDefaultSubobject<UNesAPU>(TEXT("NesAPU"));
	M_Sound->SetAutoActivate(true);
	
	M_Sound->SetupAttachment(m_springArm);
	
    m_screen->SetupAttachment(m_springArm);
    m_screen->SetStaticMesh(Asset);
    m_screen->SetMaterial(0,CanvasMaterial);
}

void ANesMain::Log(FString msg) {
	UE_LOG(LogNesMain, Log, TEXT("%s"), *msg);
}

// Called when the game starts
void ANesMain::BeginPlay()
{
	Super::BeginPlay();

	M_Ppu = make_shared<NesPPU>(256, 240, 4);
	M_Mmu = make_shared<NesCPUMMU>();
	M_Controller = make_shared<NesController>();
	M_CPU = make_unique<FNesCPU>(bTesting);
	M_Cart = make_shared<NesCart>(pathToRom);
	M_PPUMmu = make_shared<NesPPUMMU>(M_Cart);
	UMaterialInstanceDynamic* Mat = m_screen->CreateDynamicMaterialInstance(
            0, static_cast<UMaterialInterface*>(nullptr), FName(TEXT("Dynamic Mat")));
    if(Mat)
    {
        Mat->SetTextureParameterValue(FName(TEXT("TextureInput")), M_Ppu->GetScreen());
    }
    else
    {
        UE_LOG(LogTemp,Warning, TEXT("FAILED to set Texture ParameterValue"));
    }

	M_Mmu->AttachCart(M_Cart);
	M_Mmu->AttachController(M_Controller);
	M_Ppu->AttachPPUMUU(M_PPUMmu);
	M_Mmu->AttachPPU(M_Ppu);
	M_CPU->AttachMemory(M_Mmu);
	UE_LOG(LogNesMain,Log, TEXT("Starting PC: 0x%X") ,M_CPU->PC);
}

// Called every frame
void ANesMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	uint CyclesThisUpdate = 0 ; 
    while (CyclesThisUpdate < MAXCYCLES) {
		const uint Cycles = M_CPU->Tick();
		M_Ppu->Step(Cycles * 3);
		/*gbAudio.UpdateAudioTimer(cycles);*/
		CyclesThisUpdate+=Cycles;
	}
	if(M_CPU->bTesting)
	{
		M_Ppu->RenderStaticByMatrix();
	} else
	{
		M_Ppu->RenderScreen();
	}
}

void ANesMain::PressedA() {
	M_Controller->SetKey(NesController::Keys::A);
}

void ANesMain::ReleasedA() {
	M_Controller->ResetKey(NesController::Keys::A);
}

void ANesMain::PressedB() {
	M_Controller->SetKey(NesController::Keys::B);
}

void ANesMain::ReleasedB() {
	M_Controller->ResetKey(NesController::Keys::B);
}

void ANesMain::PressedUp() {
	M_Controller->SetKey(NesController::Keys::Up);
}

void ANesMain::ReleasedUp() {
	M_Controller->ResetKey(NesController::Keys::Up);
}

void ANesMain::PressedLeft() {
	M_Controller->SetKey(NesController::Keys::Left);
}

void ANesMain::ReleasedLeft() {
	M_Controller->ResetKey(NesController::Keys::Left);
}

void ANesMain::PressedRight() {
	M_Controller->SetKey(NesController::Keys::Right);
}

void ANesMain::ReleasedRight() {
	M_Controller->ResetKey(NesController::Keys::Right);
}

void ANesMain::PressedDown() {
	M_Controller->SetKey(NesController::Keys::Down);
}

void ANesMain::ReleasedDown() {
	M_Controller->ResetKey(NesController::Keys::Down);
}

void ANesMain::PressedStart() {
	M_Controller->SetKey(NesController::Keys::Start);
}

void ANesMain::ReleasedStart() {
	M_Controller->ResetKey(NesController::Keys::Start);
}

void ANesMain::PressedSelect() {
	M_Controller->SetKey(NesController::Keys::Select);
}

void ANesMain::ReleasedSelect() {
	M_Controller->ResetKey(NesController::Keys::Select);
}


// Called to bind functionality to input
void ANesMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAction("Up", IE_Pressed, this, &ANesMain::PressedUp);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &ANesMain::PressedDown);
    PlayerInputComponent->BindAction("Left", IE_Pressed, this, &ANesMain::PressedLeft);
    PlayerInputComponent->BindAction("Right", IE_Pressed, this, &ANesMain::PressedRight);
    PlayerInputComponent->BindAction("A", IE_Pressed, this, &ANesMain::PressedA);
    PlayerInputComponent->BindAction("B", IE_Pressed, this, &ANesMain::PressedB);
    PlayerInputComponent->BindAction("Start", IE_Pressed, this, &ANesMain::PressedStart);
    PlayerInputComponent->BindAction("Select", IE_Pressed, this, &ANesMain::PressedSelect);

	PlayerInputComponent->BindAction("Up", IE_Released, this, &ANesMain::ReleasedUp);
	PlayerInputComponent->BindAction("Down", IE_Released, this, &ANesMain::ReleasedDown);
    PlayerInputComponent->BindAction("Left", IE_Released, this, &ANesMain::ReleasedLeft);
    PlayerInputComponent->BindAction("Right", IE_Released, this, &ANesMain::ReleasedRight);
    PlayerInputComponent->BindAction("A", IE_Released, this, &ANesMain::ReleasedA);
    PlayerInputComponent->BindAction("B", IE_Released, this, &ANesMain::ReleasedB);
    PlayerInputComponent->BindAction("Start", IE_Released, this, &ANesMain::ReleasedStart);
    PlayerInputComponent->BindAction("Select", IE_Released, this, &ANesMain::ReleasedSelect);
}
