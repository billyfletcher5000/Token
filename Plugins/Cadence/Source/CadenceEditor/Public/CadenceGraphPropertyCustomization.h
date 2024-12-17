#pragma once

#include "IDetailCustomization.h"

struct FCadenceNamedVariable;
class UCadenceVariable;
class FReply;

DECLARE_LOG_CATEGORY_EXTERN(LogCadenceEditor, Log, All);

class FCadenceGraphCustomization : public IDetailCustomization
{
public:	
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};