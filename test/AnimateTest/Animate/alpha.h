#pragma once
#include "animate_base.h"

// λ�ƶ���

class AlphaAnimate : public Animate
{
public:
	virtual void Init() override
	{
		UI::IPanel*  pPanel = (UI::IPanel*)m_pWindow->FindObject(L"panel");

		UI::LayerAnimateParam param;
		param.finishCallback =
		[pPanel](const UI::LayerAnimateFinishParam& param)
		{
			pPanel->SetOpacity(255);
		};
		pPanel->SetOpacity(0, &param);
	}

	virtual void Release() override
	{

	}
};