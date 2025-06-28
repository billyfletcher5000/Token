#include "TokenArrayHelperBPFL.h"

void UTokenArrayHelperBPFL::SortByPredicate(TArray<AActor*>& InOutArray, const FSortByPredicateActor& InPredicateActor)
{
	if (InPredicateActor.IsBound())
	{
		Algo::Sort(InOutArray, [&InPredicateActor](AActor* A1, AActor* A2) -> bool
		{
			return InPredicateActor.Execute(A1, A2);
		});
	}
}
