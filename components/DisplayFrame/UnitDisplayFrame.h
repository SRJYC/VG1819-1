#pragma once
#include "../DisplayFrame.h"
#include "components/clickables/AddUnitOnClick.h"
class UnitDisplayFrame : public DisplayFrame
{
private:
	friend class AddUnitOnClick;
	std::vector<int> m_unitVector;
public:
	static UnitDisplayFrame* getActiveInstance();

	void start() override;

	const int& getCurrentPickedUnitId() const { return m_unitVector[getCurrentPickedItemIndex()]; } // gives back -1 if no deck has been picked yet
	int getUnitTiedTo(const kitten::K_GameObject* p_gameObject) { return m_unitVector[getItemIndexTiedTo(p_gameObject)]; }

	int getTargetAvailable() override;
	void updateIndividualDisplayObject(int p_activeObjectIndex) override;
	void onObjectClicked(int p_clickedDataSetIndex);

	UnitDisplayFrame(int p_marginX, int p_marginY);
	~UnitDisplayFrame();
};