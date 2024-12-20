#pragma once
#include "LibertasEngine.h"


class Example_ImGuiRenderer : public wi::RenderPath3D
{
	wi::gui::Label label;
public:
	void Load() override;
	void Update(float dt) override;
	void ResizeLayout() override;
	void Render() const override;
	void DisplayPerformanceData(bool* p_open);
	void igTextTitle(const char* text);
};

class Example_ImGui : public wi::Application
{
	Example_ImGuiRenderer renderer;

public:
	~Example_ImGui() override;
	void Initialize() override;
	void Compose(wi::graphics::CommandList cmd) override;
};

