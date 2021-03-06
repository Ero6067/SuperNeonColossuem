#include "EnergyManager.h"
#include "UnrealNetwork.h"
#include "Runtime/Engine/Classes/Engine/World.h"


UEnergyManager::UEnergyManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	maxEnergy = 100.f;
	currentEnergy = maxEnergy;

	isEmpty = false;
	isUnlimited = false;
}

void UEnergyManager::BeginPlay()
{
	Super::BeginPlay();
}

void UEnergyManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UEnergyManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicated to everyone
	DOREPLIFETIME(UEnergyManager, maxEnergy);
	DOREPLIFETIME(UEnergyManager, currentEnergy);
	DOREPLIFETIME(UEnergyManager, isUnlimited);
}

float UEnergyManager::RestoreEnergy(float energyValue) {
	if (GetOwner()->GetWorld()->GetAuthGameMode()) {
		if (currentEnergy < maxEnergy) {
			energyValue = FMath::Min(FMath::Abs(energyValue), maxEnergy - currentEnergy);

			AddEnergy(energyValue);

			return energyValue;
		}
	}
	return 0;
}

bool UEnergyManager::ServerRestoreEnergy_Validate(float energyValue)
{
	return true;
}

void UEnergyManager::ServerRestoreEnergy_Implementation(float energyValue)
{
	RestoreEnergy(energyValue);
}

#pragma region UseEnergy
float UEnergyManager::UseEnergy(float energyValue) 
{
	if (GetOwner()->GetWorld()->GetAuthGameMode()) {
		if (!isEmpty) {
			energyValue = FMath::Min(FMath::Abs(energyValue), currentEnergy);

			ReduceEnergy(energyValue);

			return energyValue;
		}
	}
	return 0;
}

bool UEnergyManager::ServerUseEnergy_Validate(float energyValue)
{
	return true;
}

void UEnergyManager::ServerUseEnergy_Implementation(float energyValue)
{
	UseEnergy(energyValue);
}

#pragma endregion replication
void UEnergyManager::AddEnergy(const float energy)
{
	currentEnergy += FMath::Abs(energy);
	if (currentEnergy > maxEnergy)
		currentEnergy = maxEnergy;

	isEmpty = false;
}

void UEnergyManager::ReduceEnergy(const float energy)
{
	currentEnergy -= FMath::Abs(energy);
  
	if (currentEnergy <= 0)
	{ 
		currentEnergy = 0;
		isEmpty = true;
	}
}

float UEnergyManager::GetNormalizedEnergy()
{
	return currentEnergy / maxEnergy;
}

FString UEnergyManager::GetEnergy()
{
	return FString::SanitizeFloat(currentEnergy);
}

bool UEnergyManager::ToggleUnlimitedEnergy()
{
	return isUnlimited = !isUnlimited;
}
