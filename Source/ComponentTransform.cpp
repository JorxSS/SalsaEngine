#include "ComponentTransform.h"
#include "imgui.h"
#include "IconsFontAwesome5.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform() : position(math::float3::zero), rotation(math::float3::zero), scale(math::float3::one) {
	type = Type::TRANSFORM;
}

ComponentTransform::ComponentTransform(math::float3 &translation, math::float3 &rotation, math::float3 &scale) : position(position), rotation(rotation), scale(scale) {
}

ComponentTransform::ComponentTransform(const ComponentTransform &transform) : position(transform.position), rotation(transform.rotation), scale(transform.scale) {
}

ComponentTransform::~ComponentTransform() {
}

ComponentTransform ComponentTransform::operator+(const ComponentTransform &transform) const {
	return ComponentTransform(position + transform.position, rotation + transform.rotation, scale + transform.scale);
}

ComponentTransform ComponentTransform::operator-(const ComponentTransform &transform) const {
	return ComponentTransform(position - transform.position, rotation - transform.rotation, scale - transform.scale);
}

update_status ComponentTransform::Update() {
	return UPDATE_CONTINUE;
}

void ComponentTransform::OnEditor() {
	if (ImGui::CollapsingHeader(ICON_FA_RULER_COMBINED" Transform", &canDelete, ImGuiTreeNodeFlags_DefaultOpen)) {
		if (ImGui::Checkbox("Active Transform", &active)) {
			active ? Enable() : Disable();
		}
		ImGui::DragFloat3("Position", &position[0], 0.5F, -9999.F, 9999.F, "%.1f");
		ImGui::DragFloat3("Rotation", &rotation[0], 0.5F, -9999.F, 9999.F, "%.1f");
		ImGui::DragFloat3("Scale", &scale[0], 0.5F, -9999.F, 9999.F, "%.1f");
	}
	ImGui::Separator();
	Component::OnEditor();
}