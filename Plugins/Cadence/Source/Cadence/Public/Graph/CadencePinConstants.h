#pragma once

namespace FCadencePinConstants
{
	static const FName Pin_Default_Exec = TEXT("Exec");
	static const FName Pin_Default_Then = TEXT("Then");

	static const FName Pin_Wildcard = TEXT("Wildcard");
	
	static const FName Pin_Duration = TEXT("Duration");
	static const FName Pin_Trigger = TEXT("Trigger");
	static const FName Pin_Actor = TEXT("Actor");
	static const FName Pin_Position = TEXT("Position");
	static const FName Pin_Rotation = TEXT("Rotation");

	static const FName Pin_Value = TEXT("Value");
	static const FName Pin_Float = TEXT("Float");
	static const FName Pin_Int = TEXT("Int");
	static const FName Pin_Bool = TEXT("Bool");
	static const FName Pin_Vector = TEXT("Vector");
	static const FName Pin_Vector2 = TEXT("Vector2");
	static const FName Pin_Rotator = TEXT("Rotator");
	static const FName Pin_Object = TEXT("Object");
	static const FName Pin_Enum = TEXT("Enum");
}

namespace FCadencePinCategoryConstants
{
	static const FName PC_Exec = TEXT("Exec");
	static const FName PC_Wildcard = TEXT("Wildcard");	
	static const FName PC_Integer = TEXT("Integer");
	static const FName PC_Float = TEXT("Float");
	static const FName PC_Double = TEXT("Double");
	static const FName PC_Boolean = TEXT("Boolean");
	static const FName PC_Vector = TEXT("Vector");
	static const FName PC_Vector2 = TEXT("Vector2");
	static const FName PC_Rotator = TEXT("Rotator");
	static const FName PC_Enum = TEXT("Enum");
	static const FName PC_Object = TEXT("Object");
	static const FName PC_Actor = TEXT("Actor");	
	static const FName PC_Trigger = TEXT("Trigger");
}

namespace FCadencePinSubCategoryConstants
{
	static const FName PSC_Array = TEXT("Array");
}