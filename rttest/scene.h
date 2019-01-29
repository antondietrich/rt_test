#pragma once

struct Light
{
	V4 color;
	V3 position;
	float intensity;
};

struct Material
{
	V4 diffuse;
	V4 specular;
	float reflectivity;
	bool isConductor;
};

struct Object
{
	Geometry geometry;
	Material material;
};

struct Scene
{
	Object objects[32];
	Light lights[32];
	unsigned int objectCount;
	unsigned int lightCount;
};

Vertex * vb;
Scene scene = Scene();

void InitScene()
{
	scene.lights[scene.lightCount].position = {0.0f, 0.0f, 5.0f};
	scene.lights[scene.lightCount].color = {0.8f, 0.6f, 0.5f, 1.0f};
	scene.lights[scene.lightCount].intensity = 30.0f;
	scene.lights[scene.lightCount].intensity = 60.0f * 17.5f / (PI*4.0f);
	scene.lightCount++;

	Material matBase;
	matBase.diffuse = {0.8f, 0.8f, 0.8f, 1.0f};
	matBase.specular = {0.0f, 0.0f, 0.0f, 1.0f};
	matBase.reflectivity = 0;
	matBase.isConductor = false;

	Material matSphere = matBase;
	matSphere.diffuse = {0.98f, 0.5f, 0.01f};
	matSphere.specular = {1.0f, 1.0f, 1.0f, 1.0f};
	matSphere.reflectivity = 1.0f;

	Material matLeft = matBase;
	matLeft.diffuse = V4{254, 105, 36, 255}/255.0f;

	Material matRight = matBase;
	matRight.diffuse = V4{120, 204, 237, 255}/255.0f;

#if 0
	matRight.reflectivity = 1.0f;
	matLeft.reflectivity = 1.0f;
#endif

	Material matBack = matBase;
	Material matBottom = matBase;
	Material matTop = matBase;

#if 1
	matBottom.reflectivity = 0.5f;
#endif

#if 0
	matBack.diffuse = V4{0.0f, 1.0f, 0.0f, 1.0f};
	matBottom.diffuse = V4{1.0f, 0.0f, 0.0f, 1.0f};
	matTop.diffuse = V4{0.0f, 0.0f, 1.0f, 1.0f};
#endif

	vb = new Vertex[256];

	// sphere
	scene.objects[scene.objectCount].geometry.type = GeoType::SPHERE;
	scene.objects[scene.objectCount].geometry.sphere.o = {1.0f, 0.8f, 1.0f};
	scene.objects[scene.objectCount].geometry.sphere.r = 1.0f;
	scene.objects[scene.objectCount].material = matSphere;
	scene.objectCount++;

#if 1
	scene.objects[scene.objectCount].geometry.type = GeoType::SPHERE;
	scene.objects[scene.objectCount].geometry.sphere.o = {0.0f, -1.0f, 1.0f};
	scene.objects[scene.objectCount].geometry.sphere.r = 1.0f;
	scene.objects[scene.objectCount].material = matBase;
	scene.objectCount++;
#endif

	#if 1
	scene.objects[scene.objectCount].geometry.type = GeoType::SPHERE;
	scene.objects[scene.objectCount].geometry.sphere.o = {0, 2.1f, 0.5f};
	scene.objects[scene.objectCount].geometry.sphere.r = 0.5f;
	scene.objects[scene.objectCount].material = matSphere;
	scene.objectCount++;
#endif
	
	// right
	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[0] = {{ 3.0f,  3.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};
	vb[1] = {{-3.0f,  3.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};
	vb[2] = {{-3.0f,  3.0f, 6.0f}, {0.0f, -1.0f, 0.0f}};
	vb[3] = {{ 3.0f,  3.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};
	vb[4] = {{-3.0f,  3.0f, 6.0f}, {0.0f, -1.0f, 0.0f}};
	vb[5] = {{ 3.0f,  3.0f, 6.0f}, {0.0f, -1.0f, 0.0f}};

	Sphere boundRight;
	boundRight.o = {0.0f, 3.0f, 3.0f};
	boundRight.r = (float)sqrt(18.0);

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb;
	scene.objects[scene.objectCount].geometry.mesh.bound = boundRight;
	scene.objects[scene.objectCount].material = matRight;
	scene.objectCount++;

	// left
	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[6] =  {{ 3.0f, -3.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
	vb[7] =  {{-3.0f, -3.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
	vb[8] =  {{-3.0f, -3.0f, 6.0f}, {0.0f, 1.0f, 0.0f}};
	vb[9] =  {{ 3.0f, -3.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
	vb[10] = {{-3.0f, -3.0f, 6.0f}, {0.0f, 1.0f, 0.0f}};
	vb[11] = {{ 3.0f, -3.0f, 6.0f}, {0.0f, 1.0f, 0.0f}};

	Sphere boundLeft;
	boundLeft.o = {0.0f, -3.0f, 3.0f};
	boundLeft.r = (float)sqrt(18.0);

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 6;
	scene.objects[scene.objectCount].geometry.mesh.bound = boundLeft;
	scene.objects[scene.objectCount].material = matLeft;
	scene.objectCount++;

	// back
	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[12] = {{ 3.0f, -3.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[13] = {{ 3.0f,  3.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[14] = {{ 3.0f,  3.0f, 6.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[15] = {{ 3.0f, -3.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[16] = {{ 3.0f,  3.0f, 6.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[17] = {{ 3.0f, -3.0f, 6.0f}, {-1.0f, 0.0f, 0.0f}};

	Sphere boundBack;
	boundBack.o = {3.0f, 0.0f, 3.0f};
	boundBack.r = (float)sqrt(18.0);

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 12;
	scene.objects[scene.objectCount].geometry.mesh.bound = boundBack;
	scene.objects[scene.objectCount].material = matBack;
	scene.objectCount++;

#if 1
	// bottom
	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[18] = {{ 3.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
	vb[19] = {{-3.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
	vb[20] = {{-3.0f,  3.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
	vb[21] = {{ 3.0f, -3.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
	vb[22] = {{-3.0f,  3.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
	vb[23] = {{ 3.0f,  3.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

	Sphere boundBottom;
	boundBottom.o = {0.0f, 0.0f, 0.0f};
	boundBottom.r = (float)sqrt(18.0);

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 18;
	scene.objects[scene.objectCount].geometry.mesh.bound = boundBottom;
	scene.objects[scene.objectCount].material = matBottom;
	scene.objectCount++;

	// top
#if 1
	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[24] = {{ 3.0f, -3.0f, 6.0f}, {0.0f, 0.0f, -1.0f}};
	vb[25] = {{-3.0f,  3.0f, 6.0f}, {0.0f, 0.0f, -1.0f}};
	vb[26] = {{-3.0f, -3.0f, 6.0f}, {0.0f, 0.0f, -1.0f}};
	vb[27] = {{ 3.0f, -3.0f, 6.0f}, {0.0f, 0.0f, -1.0f}};
	vb[28] = {{ 3.0f,  3.0f, 6.0f}, {0.0f, 0.0f, -1.0f}};
	vb[29] = {{-3.0f,  3.0f, 6.0f}, {0.0f, 0.0f, -1.0f}};

	Sphere boundTop;
	boundTop.o = {0.0f, 0.0f, 6.0f};
	boundTop.r = (float)sqrt(18.0);

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 24;
	scene.objects[scene.objectCount].geometry.mesh.bound = boundTop;
	scene.objects[scene.objectCount].material = matTop;
	scene.objectCount++;
#endif
#endif
}