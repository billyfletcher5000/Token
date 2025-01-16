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
	static const FName Pin_Array = TEXT("Array");
	static const FName Pin_Float = TEXT("Float");
	static const FName Pin_Int = TEXT("Int");
	static const FName Pin_Bool = TEXT("Bool");
	static const FName Pin_String = TEXT("String");
	static const FName Pin_Name = TEXT("Name");
	static const FName Pin_Text = TEXT("Text");
	static const FName Pin_Vector = TEXT("Vector");
	static const FName Pin_Vector2 = TEXT("Vector2");
	static const FName Pin_Rotator = TEXT("Rotator");
	static const FName Pin_Object = TEXT("Object");
	static const FName Pin_Enum = TEXT("Enum");

	static const FName Pin_Section = TEXT("Section");
	static const FName Pin_Count = TEXT("Count");
	static const FName Pin_Period = TEXT("Period");

	static const FName Pin_Depth = TEXT("Depth");

	static const FName Pin_Condition = TEXT("Condition");
	static const FName Pin_True = TEXT("True");
	static const FName Pin_False = TEXT("False");
	
	static const FName Pin_LoopBody = TEXT("Loop Body");
	static const FName Pin_Completed = TEXT("Completed");
	static const FName Pin_ArrayElement = TEXT("Array Element");
	static const FName Pin_Index = TEXT("Index");
	static const FName Pin_FirstIndex = TEXT("First Index");
	static const FName Pin_LastIndex = TEXT("Last Index");
	static const FName Pin_Item = TEXT("Item");
	static const FName Pin_Size = TEXT("Size");
	static const FName Pin_Unique = TEXT("Unique");
	static const FName Pin_Removed = TEXT("Removed");

	static const FName Pin_CadenceAsset = TEXT("Cadence Asset");

	static const FName Pin_ReactionGroup = TEXT("Reaction Group");

	static const FName Pin_Primary = TEXT("Primary");
	static const FName Pin_Secondary = TEXT("Secondary");
	static const FName Pin_Result = TEXT("Result");
	static const FName Pin_Unnamed = TEXT("");
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
	static const FName PC_String = TEXT("String");
	static const FName PC_Name = TEXT("Name");
	static const FName PC_Text = TEXT("Text");
	static const FName PC_Enum = TEXT("Enum");
	static const FName PC_Object = TEXT("Object");
	static const FName PC_Actor = TEXT("Actor");	
	static const FName PC_Trigger = TEXT("Trigger");
	static const FName PC_CadenceAsset = TEXT("CadenceAsset");
	static const FName PC_ReactionGroup = TEXT("ReactionGroup");	
	static const FName PC_SectionName = TEXT("SectionName");
}

namespace FCadencePinSubCategoryConstants
{
	static const FName PSC_Array = TEXT("Array");
}

namespace FCadenceVariableColorConstants
{
	static constexpr FLinearColor VC_Wildcard = FLinearColor(0.4f, 0.4f, 0.4f);
	static constexpr FLinearColor VC_Int = FLinearColor(0.4f, 0.4f, 1.0f);	
	static constexpr FLinearColor VC_Float = FLinearColor(0.0f, 1.0f, 0.0f);
	static constexpr FLinearColor VC_Double = FLinearColor(0.4f, 0.4f, 1.0f); 
	static constexpr FLinearColor VC_Bool = FLinearColor(1.0f, 0.0f, 0.0f); 
	static constexpr FLinearColor VC_Vector = FLinearColor(1.0f, 1.0f, 0.0f); 
	static constexpr FLinearColor VC_Vector2 = FLinearColor(0.0f, 0.0f, 1.0f);
	static constexpr FLinearColor VC_Rotator = FLinearColor(1.0f, 0.7f, 0.2f);
	static constexpr FLinearColor VC_String = FLinearColor(0.969f, 0.471f, 0.875f);
	static constexpr FLinearColor VC_Name = FLinearColor(0.969f, 0.471f, 0.875f);
	static constexpr FLinearColor VC_Text = FLinearColor(0.969f, 0.471f, 0.875f);
	static constexpr FLinearColor VC_UObject = FLinearColor(0.3f, 0.3f, 1.0f); 
	static constexpr FLinearColor VC_Enum = FLinearColor(0.09f, 0.702f, 0.271f);
	static constexpr FLinearColor VC_Actor = FLinearColor(0.3f, 0.3f, 0.95f); 
	static constexpr FLinearColor VC_Trigger = FLinearColor(0.75f, 0.0f, 0.0f);
	static constexpr FLinearColor VC_CadenceAsset = FLinearColor(0.6f, 0.2f, 0.3f);	
	static constexpr FLinearColor VC_ReactionGroup = FLinearColor(0.635f, 0.0f, 1.0f);
	static constexpr FLinearColor VC_SectionName = FLinearColor(0.869f, 0.371f, 0.775f);
}
