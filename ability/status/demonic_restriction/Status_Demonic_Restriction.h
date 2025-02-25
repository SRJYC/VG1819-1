#pragma once
#include "ability/status/statusEvent/TimePointEvent.h"
#include "ability/status/Status.h"

namespace ability
{
	class Status_Demonic_Restriction : public Status
	{
	public:
		Status_Demonic_Restriction();
		virtual Status* clone() const = 0;
		virtual int effect(const TimePointEvent::TPEventType& p_type, TimePointEvent* p_event) override;
		virtual void restrictionOn() = 0;
		virtual void restrictionOff() = 0;
	protected:
		bool m_restriction = false;

		TimePointEvent::TPEventType m_refreshTP;

		void refreshOnTP(TimePointEvent::TPEventType p_type);
		void refresh();
	};

	class Status_DR_AD_Change : public Status_Demonic_Restriction
	{
	public:
		Status_DR_AD_Change();
		Status* clone() const { return new Status_DR_AD_Change(*this); };
		void restrictionOn() override;
		void restrictionOff() override;
		int effect() override;
	};

	class Status_DR_Eternal_Eye : public Status_Demonic_Restriction
	{
	public:
		Status_DR_Eternal_Eye();
		Status* clone() const { return new Status_DR_Eternal_Eye(*this); };
		void restrictionOn() override;
		void restrictionOff() override;
	};

	class Status_DR_Mud_Demon : public Status_Demonic_Restriction
	{
	public:
		Status_DR_Mud_Demon();
		Status* clone() const { return new Status_DR_Mud_Demon(*this); };
		void restrictionOn() override;
		void restrictionOff() override;
	private:
		int m_deltaMV;
	};

	/*
	class Status_DR_Horror_Lord : public Status_Demonic_Restriction
	{
	public:
		Status_DR_Horror_Lord();
		Status* clone() const { return new Status_DR_Horror_Lord(*this); };
		void restrictionOn() override;
		void restrictionOff() override;
		int effect() override;
	};*/

	class Status_DR_Wraith : public Status_Demonic_Restriction
	{
	public:
		Status_DR_Wraith();
		Status* clone() const { return new Status_DR_Wraith(*this); };
		void restrictionOn() override;
		void restrictionOff() override;
	};

	/*
	class Status_DR_Gorefiend : public Status_Demonic_Restriction
	{
	public:
		Status_DR_Gorefiend();
		Status* clone() const { return new Status_DR_Gorefiend(*this); };
		void restrictionOn() override;
		void restrictionOff() override;
	};*/
}