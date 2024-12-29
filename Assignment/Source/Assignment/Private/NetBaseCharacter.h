// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Net/UnrealNetwork.h"
#include "NetBaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EBodyPart : uint8
{
    BP_Face = 0,
    BP_Hair = 1,
    BP_Chest = 2,
    BP_Hands = 3,
    BP_Legs = 4,
    BP_Beard = 5,
    BP_Eyebrows = 6,
    BP_COUNT = 7,
};

USTRUCT(BlueprintType)
struct FSMeshAssetList : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<USkeletalMesh*> ListSkeletal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UStaticMesh*> ListStatic;
};

USTRUCT(BlueprintType)
struct FSBodyPartSelection
{
    GENERATED_USTRUCT_BODY()
    int Indices[(int)EBodyPart::BP_COUNT];
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isFemale;
};

USTRUCT(BlueprintType)
struct FSPlayerInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Nickname;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSBodyPartSelection BodyParts;

    bool Ready;
};


UCLASS()
class ANetBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ANetBaseCharacter();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    TArray<EBodyPart> GetAllBodyParts() const;

    UFUNCTION(BlueprintCallable)
    void RandomizeButton();

    UFUNCTION(BlueprintCallable)
    void ChangeBodyPart(EBodyPart index, int value, bool DirectSet);

    UFUNCTION(BlueprintCallable)
    void ChangeGender(bool isFemale);

    UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_PlayerInfoChanged)
    FSBodyPartSelection PartSelection;

    UFUNCTION(Server, Reliable)
    void SubmitPlayerInfoToServer(FSPlayerInfo Info);

    UFUNCTION()
    void OnRep_PlayerInfoChanged();

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifeTimeProps) const;

private:
    UPROPERTY()
    USkeletalMeshComponent* PartFace;

    UPROPERTY()
    UStaticMeshComponent* PartHair;

    UPROPERTY()
    UStaticMeshComponent* PartBeard;

    UPROPERTY()
    UStaticMeshComponent* PartEyes;

    UPROPERTY()
    USkeletalMeshComponent* PartHands;

    UPROPERTY()
    UStaticMeshComponent* PartEyebrows;

    UPROPERTY()
    USkeletalMeshComponent* PartLegs;

    static FSMeshAssetList* GetBodyPartList(EBodyPart part, bool isFemale);

    void UpdateBodyParts();
};
