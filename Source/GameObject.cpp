#include "GameObject.h"
#include "Globals.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "imgui.h"

GameObject::GameObject() {
}

GameObject::~GameObject() {
	for each(Component* comp in components)
		delete comp;
	components.clear();
}

void GameObject::Draw() {
	ImGui::TreeNode(name.c_str());
	//ImGui::TreePop();
}

void GameObject::Update() {
	for each(Component* comp in components) {
		if (comp->active)
			comp->OnEditor();
	}
}

Component* GameObject::CreateComponent(Type type) {
	Component* component = nullptr;

	switch (type) {
	case Type::TRANSFORM:
		component = new ComponentTransform();
		break;

	case Type::MESH:
		component = new ComponentMesh();
		break;

	case Type::MATERIAL:
		component = new ComponentMaterial();
		break;

	default:
		LOG("Incorrect component type!\n");
		break;
	}

	if (component != nullptr)
		components.push_back(component);

	return component;
}

void GameObject::SetParent(int parentID, GameObject *parent) {
	this->parent = parent;
	//TODO errase object from gameobjects modulescene and insert it as aa children
}
