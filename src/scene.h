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
	V4 rf0; // incident reflectance
	V4 emissive;
	float power;
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
	// scene.lightCount++;

	scene.lights[scene.lightCount].position = {2.5f, 0.0f, 1.5f};
	scene.lights[scene.lightCount].color = {0.4f, 0.6f, 0.8f, 1.0f};
	scene.lights[scene.lightCount].intensity = 20.0f * 17.5f / (PI*4.0f);
	// scene.lightCount++;

	Material matBase = {};
	matBase.diffuse = {0.8f, 0.8f, 0.8f, 1.0f};
	matBase.rf0 = V4::FromFloat(0.005f);
	matBase.isConductor = false;

	Material matGold = matBase;
	matGold.diffuse = {0.0f, 0.0f, 0.0f};
	matGold.rf0 = {1.0f, 0.71f, 0.29f, 1.0f}; // gold
	matGold.isConductor = true;

	Material matCopper = matGold;
	matCopper.rf0 = {0.96f, 0.64f, 0.54f, 1.0f}; // copper

	Material matLeft = matBase;
	matLeft.diffuse = V4{254, 105, 36, 255}/255.0f;

	Material matRight = matBase;
	matRight.diffuse = V4{120, 204, 237, 255}/255.0f;

	Material matEmissive = matBase;
	matEmissive.diffuse = V4::FromFloat(0.0f);
	matEmissive.rf0 = V4::FromFloat(0.0f);
	matEmissive.emissive = V4::FromFloat(1.0f);
	matEmissive.power = 100;

#if 0
	matRight.reflectivity = 1.0f;
	matLeft.reflectivity = 1.0f;
#endif

	Material matBack = matBase;
	Material matBottom = matBase;
	Material matTop = matBase;


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
	scene.objects[scene.objectCount].material = matGold;
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
	scene.objects[scene.objectCount].material = matCopper;
	scene.objects[scene.objectCount].material = matEmissive;
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

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb;
	scene.objects[scene.objectCount].material = matRight;
	ComputeMeshBound(&scene.objects[scene.objectCount].geometry.mesh);
	scene.objectCount++;


	// left
	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[6] =  {{ 3.0f, -3.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
	vb[7] =  {{-3.0f, -3.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
	vb[8] =  {{-3.0f, -3.0f, 6.0f}, {0.0f, 1.0f, 0.0f}};
	vb[9] =  {{ 3.0f, -3.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
	vb[10] = {{-3.0f, -3.0f, 6.0f}, {0.0f, 1.0f, 0.0f}};
	vb[11] = {{ 3.0f, -3.0f, 6.0f}, {0.0f, 1.0f, 0.0f}};

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 6;
	scene.objects[scene.objectCount].material = matLeft;
	ComputeMeshBound(&scene.objects[scene.objectCount].geometry.mesh);
	scene.objectCount++;

	// back
	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[12] = {{ 3.0f, -3.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[13] = {{ 3.0f,  3.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[14] = {{ 3.0f,  3.0f, 6.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[15] = {{ 3.0f, -3.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[16] = {{ 3.0f,  3.0f, 6.0f}, {-1.0f, 0.0f, 0.0f}};
	vb[17] = {{ 3.0f, -3.0f, 6.0f}, {-1.0f, 0.0f, 0.0f}};

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 12;
	scene.objects[scene.objectCount].material = matBack;
	ComputeMeshBound(&scene.objects[scene.objectCount].geometry.mesh);
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

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 18;
	scene.objects[scene.objectCount].material = matBottom;
	ComputeMeshBound(&scene.objects[scene.objectCount].geometry.mesh);
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

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 24;
	scene.objects[scene.objectCount].material = matTop;
	ComputeMeshBound(&scene.objects[scene.objectCount].geometry.mesh);
	scene.objectCount++;
#endif

	scene.objects[scene.objectCount].geometry.type = GeoType::MESH;
	vb[30] = {{ 0.5f, -0.5f, 5.9f}, {0.0f, 0.0f, -1.0f}};
	vb[31] = {{-0.5f,  0.5f, 5.9f}, {0.0f, 0.0f, -1.0f}};
	vb[32] = {{-0.5f, -0.5f, 5.9f}, {0.0f, 0.0f, -1.0f}};
	vb[33] = {{ 0.5f, -0.5f, 5.9f}, {0.0f, 0.0f, -1.0f}};
	vb[34] = {{ 0.5f,  0.5f, 5.9f}, {0.0f, 0.0f, -1.0f}};
	vb[35] = {{-0.5f,  0.5f, 5.9f}, {0.0f, 0.0f, -1.0f}};

	scene.objects[scene.objectCount].geometry.mesh.vertexCount = 6;
	scene.objects[scene.objectCount].geometry.mesh.vertices = vb + 30;
	scene.objects[scene.objectCount].material = matEmissive;
	ComputeMeshBound(&scene.objects[scene.objectCount].geometry.mesh);
	scene.objectCount++;
#endif
}
