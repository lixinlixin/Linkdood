#pragma once
#include"../data/SysMsg.h"

#include <memory>
#include <vector>


class ISysMsgObserver {
public:
	/************************************************************************
	* @brief onSysMessageNotice
	* @description: ����ϵͳ��Ϣ
	* @param[in] msg ������Ϣ
	************************************************************************/
	virtual void onSysMessageNotice(SysMsg& msg) = 0;
};