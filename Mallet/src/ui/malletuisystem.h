#pragma once

#include <systems/system.h>
#include "windows/menubar.h"
#include "windows/toolbar.h"

class MalletUiSystem : public System
{
public:
	MalletUiSystem()
	{};

	~MalletUiSystem()
	{
		for (auto it = windowPs.begin(); it != windowPs.end(); ++it)
		{
			delete *it;
		}
		windowPs.clear();
	};

	void Init() override;
	void Update() override;

public:
	MenuBar menuBar;
	std::list<MalletWindow*> windowPs;

	static inline bool enableExportDialog = false;

}; inline MalletUiSystem malletUI;