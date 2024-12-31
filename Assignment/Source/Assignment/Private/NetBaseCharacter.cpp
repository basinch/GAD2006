// Fill out your copyright notice in the Description page of Project Settings.

#include "NetBaseCharacter.h"
#include "NetPlayerState.h"
#include "NetGameInstance.h"


static UDataTable* SBodyParts = nullptr;

static const wchar_t* BodyPartNames[] =
{
    TEXT("Face"),
    TEXT("Hair"),
    TEXT("Chest"),
    TEXT("Hands"),
    TEXT("Legs"),
    TEXT("Beard"),
    TEXT("Eyebrows")
};

// Sets default values
ANetBaseCharacter::ANetBaseCharacter()
{
    // Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    PartFace = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
    PartFace->SetupAttachment(GetMesh());
    PartFace->SetLeaderPoseComponent(GetMesh());

    PartHands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
    PartHands->SetupAttachment(GetMesh());
    PartHands->SetLeaderPoseComponent(GetMesh());

    PartLegs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
    PartLegs->SetupAttachment(GetMesh());
    PartLegs->SetLeaderPoseComponent(GetMesh());

    PartHair = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hair"));
    PartHair->SetupAttachment(PartFace, FName("headSocket"));

    PartEyebrows = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eyebrows"));
    PartEyebrows->SetupAttachment(PartFace, FName("headSocket"));

    PartBeard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beard"));
    PartBeard->SetupAttachment(PartFace, FName("headSocket"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_Eyes(TEXT("/Script/Engine.StaticMesh'/Game/StylizedModularChar/Meshes/Male/Eye/SM_Eyes.SM_Eyes'"));

    PartEyes = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Eyes"));
    PartEyes->SetupAttachment(PartFace, FName("headSocket"));
    PartEyes->SetStaticMesh(SK_Eyes.Object);

    static ConstructorHelpers::FObjectFinder<UDataTable> DT_BodyParts(TEXT("/Script/Engine.DataTable'/Game/Blueprints/DataTables/DT_BodyParts.DT_BodyParts'"));
    SBodyParts = DT_BodyParts.Object;

}

void ANetBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GetNetMode() == NM_Standalone) return;
    SetActorHiddenInGame(true);
    CheckPlayerState();
}

void ANetBaseCharacter::OnConstruction(FTransform const& Transform)
{
    UpdateBodyParts();
}

void ANetBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ANetBaseCharacter::ChangeBodyPart(EBodyPart index, int value, bool DirectSet)
{
    FSMeshAssetList* List = GetBodyPartList(index, BodyPartIndices[static_cast<int>(EBodyPart::BP_BodyType)] != 0);
    if (List == nullptr) return;

    int CurrentIndex = BodyPartIndices[(int)index];

    if (DirectSet)
    {
        CurrentIndex = value;
    }
    else
    {
        CurrentIndex += value;
    }

    int Num = List->ListSkeletal.Num() + List->ListStatic.Num();

    if (CurrentIndex < 0)
    {
        CurrentIndex += Num;
    }
    else
    {
        CurrentIndex %= Num;
    }

    BodyPartIndices[(int)index] = CurrentIndex;

    switch (index)
    {
    case EBodyPart::BP_Face:PartFace->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]); break;
    case EBodyPart::BP_Beard:PartBeard->SetStaticMesh(List->ListStatic[CurrentIndex]); break;
    case EBodyPart::BP_Chest:GetMesh()->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]); break;
    case EBodyPart::BP_Hair:PartHair->SetStaticMesh(List->ListStatic[CurrentIndex]); break;
    case EBodyPart::BP_Hands:PartHands->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]); break;
    case EBodyPart::BP_Legs:PartLegs->SetSkeletalMeshAsset(List->ListSkeletal[CurrentIndex]); break;
    case EBodyPart::BP_Eyebrows:PartEyebrows->SetStaticMesh(List->ListStatic[CurrentIndex]); break;
    }
}

void ANetBaseCharacter::CheckPlayerState()
{
    ANetPlayerState* State = GetPlayerState<ANetPlayerState>();

    if (State == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("State == nullptr"));

        GWorld->GetTimerManager().SetTimer(ClientDataCheckTimer, this, &ANetBaseCharacter::CheckPlayerState, 0.25f,
            false);
    }
    else
    {
        if (IsLocallyControlled())
        {
            UNetGameInstance* Instance = Cast<UNetGameInstance>(GWorld->GetGameInstance());
            if (Instance)
            {
                SubmitPlayerInfoToServer(Instance->PlayerInfo);
            }
        }
        CheckPlayerInfo();
    }
}

void ANetBaseCharacter::CheckPlayerInfo()
{
    ANetPlayerState* State = GetPlayerState<ANetPlayerState>();

    if (State && PlayerInfoReceived)
    {
        ParseCustomizationData(State->Data.CustomizationData);
        UpdateBodyParts();
        OnPlayerInfoChanged();
        SetActorHiddenInGame(false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("State Not Received!"));
        GWorld->GetTimerManager().SetTimer(ClientDataCheckTimer, this, &ANetBaseCharacter::CheckPlayerInfo, 0.25f, false);
    }
}

FString ANetBaseCharacter::GetCustomizationData()
{
    FString Data;
    for (size_t i = 0; i < static_cast<int>(EBodyPart::BP_BodyType); i++)
    {
        Data += FString::FromInt(BodyPartIndices[i]);
        if (i < (static_cast<int>(EBodyPart::BP_COUNT)-1.0f))
        {
            Data += TEXT(",");
        }
    }
    return Data;
}

void ANetBaseCharacter::ParseCustomizationData(FString BodyPartData)
{
    TArray<FString> ArrayData;
    BodyPartData.ParseIntoArray(ArrayData, TEXT(","));
    for (size_t i = 0; i < ArrayData.Num(); i++)
    {
        BodyPartIndices[i] = FCString::Atoi(*ArrayData[i]);
    }
}

void ANetBaseCharacter::ChangeGender(bool _isFemale)
{
    UpdateBodyParts();
}

FSMeshAssetList* ANetBaseCharacter::GetBodyPartList(EBodyPart part, bool isFemale)
{
    FString Name = FString::Printf(TEXT("%s%s"), isFemale ? TEXT("Female") : TEXT("Male"), BodyPartNames[(int)part]);
    return SBodyParts ? SBodyParts->FindRow<FSMeshAssetList>(*Name, nullptr) : nullptr;
}

void ANetBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//TArray<EBodyPart> ANetBaseCharacter::GetAllBodyParts() const
//{
//    TArray<EBodyPart> BodyParts;
//    for (int32 i = 0; i < static_cast<int32>(EBodyPart::BP_COUNT); ++i)
//    {
//        BodyParts.Add(static_cast<EBodyPart>(i));
//    }
//    return BodyParts;
//}

//void ANetBaseCharacter::RandomizeButton()
//{
//    auto BodyParts = GetAllBodyParts();
//
//    for (const EBodyPart& Part : BodyParts) 
//    {
//        if (FSMeshAssetList* List = GetBodyPartList(Part, PartSelection.isFemale))
//        {
//            int32 NumSkeletalMeshes = List->ListSkeletal.Num();
//            int32 NumStaticMeshes = List->ListStatic.Num();
//            int32 NumOptions = NumSkeletalMeshes + NumStaticMeshes;
//
//            if (NumOptions > 0)
//            {
//                int32 RandomIndex = FMath::RandRange(0, NumOptions - 1);
//                ChangeBodyPart(Part, RandomIndex, true);
//            }
//        }
//    }
//}

void ANetBaseCharacter::UpdateBodyParts()
{

    ChangeBodyPart(EBodyPart::BP_Face, 0, false);
    ChangeBodyPart(EBodyPart::BP_Beard, 0, false);
    ChangeBodyPart(EBodyPart::BP_Chest, 0, false);
    ChangeBodyPart(EBodyPart::BP_Hair, 0, false);
    ChangeBodyPart(EBodyPart::BP_Hands, 0, false);
    ChangeBodyPart(EBodyPart::BP_Legs, 0, false);
    ChangeBodyPart(EBodyPart::BP_Eyebrows, 0, false);
}

