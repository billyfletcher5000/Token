#include "CadenceGraphPropertyCustomization.h"

#include "Graph/CadenceGraph.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "Input/Reply.h"

DEFINE_LOG_CATEGORY(LogCadenceEditor);

TSharedRef<IDetailCustomization> FCadenceGraphCustomization::MakeInstance()
{
	return MakeShareable(new FCadenceGraphCustomization);
}

void FCadenceGraphCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.HideProperty(GET_MEMBER_NAME_CHECKED(UCadenceGraph, UserVariables));
}