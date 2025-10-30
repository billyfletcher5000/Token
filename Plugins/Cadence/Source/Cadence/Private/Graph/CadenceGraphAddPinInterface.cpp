// Copyright (C) Billy Fletcher 2025


#include "Graph/CadenceGraphAddPinInterface.h"

FName ICadenceGraphAddPinInterface::GetNameForAdditionalPin(int32 PinIndex)
{
	check(PinIndex < GetMaxInputPinsNum());
	const FName Name(*FString::Chr(TCHAR('A') + static_cast<TCHAR>(PinIndex)));
	return Name;
}
