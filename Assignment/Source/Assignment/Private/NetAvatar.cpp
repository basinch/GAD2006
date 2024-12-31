// Fill out your copyright notice in the Description page of Project Settings.


#include "NetAvatar.h"
#include "GameFramework/CharacterMovementComponent.h"

ANetAvatar::ANetAvatar():
    MovementScale(1.0f)
{
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void ANetAvatar::BeginPlay()
{
    Super::BeginPlay();

    Camera->bUsePawnControlRotation = false;
    SpringArm->bUsePawnControlRotation = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ANetAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("MoveForward", this, &ANetAvatar::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ANetAvatar::MoveRight);
}

void ANetAvatar::MoveForward(float Scale)
{
    FRotator Rotation = GetController()->GetControlRotation();
    FRotator YawRotation(0, Rotation.Yaw, 0);
    FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(ForwardDirection, MovementScale*Scale);
}

void ANetAvatar::MoveRight(float Scale)
{
    FRotator Rotation = GetController()->GetControlRotation();
    FRotator YawRotation(0, Rotation.Yaw, 0);
    FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(RightDirection, MovementScale*Scale);
}

