#ifndef __COMPONENTMESH_H__
#define __COMPONENTMESH_H__

#include "Component.h"

class ComponentMesh : public Component {
public:
	ComponentMesh();
	ComponentMesh(const ComponentMesh &mesh);
	~ComponentMesh();

	update_status Update() override;
	void OnEditor() override;
};

#endif // __COMPONENTMESH_H__