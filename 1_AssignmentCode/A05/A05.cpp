// This has been adapted from the Vulkan tutorial

// TO MOVE
#define JSON_DIAGNOSTICS 1
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
	{2, {"Meshes showcase", "Press SPACE to see the Blocky Beast"}, 0, 0},
	{2, {"Blocky Beast", "Press SPACE to save the screenshots","",""}, 0, 0},
	{3, {"Saving Screenshots. Please wait.", "01234567890123456789", "01234567890123456789",""}, 0, 0}
};

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
	alignas(16) glm::vec4 color;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 eyeDir;
};



// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec3 norm;
};

#include "Mesh.hpp"


// MAIN ! 
class A05 : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;

	// Vertex formats
	VertexDescriptor VD;

	// Pipelines [Shader couples]
	Pipeline P, PW;

	// Models
	Model M[9], MAx;
	// Descriptor sets
	DescriptorSet DS[27], DSAx;
	// Textures
	Texture T1, T2;
	
	glm::mat4 Wm[27], Wm2[27];
	glm::vec4 MCol[27], MCol2[27];
	const int JM2M[27] = {6, 7, 8, 8, 8, 8, 1, 1, 6,
						  2, 4, 4, 4, 4, 4, 4, 4, 4,
						  6, 3, 3, 3, 3, 5, 5, 7, 0};

	TextMaker txt;
	
	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 1.5, 7.0);
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
	float Ar;
	bool WireFrame = false;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A05 - Mesh";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.85f, 1.0f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 28 * 2 + 2;
		texturesInPool = 28 + 1;
		setsInPool = 28 + 1;

		Ar = 4.0f / 3.0f;
	}
	
	// What to do when the window changes size
	void onWindowResize(int w, int h) {
		std::cout << "Window resized to: " << w << " x " << h << "\n";
		Ar = (float)w / (float)h;
	}
	
	// Here you load and setup all your Vulkan Models and Texutures.
	// Here you also create your Descriptor set layouts and load the shaders for the pipelines
	void localInit() {
		// Descriptor Layouts [what will be passed to the shaders]
		DSL.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
				});

		// Vertex descriptors
		VD.init(this, {
				  {0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV),
				         sizeof(glm::vec2), UV},
				  {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, norm),
				         sizeof(glm::vec3), NORMAL}
				});

		// Pipelines [Shader couples]
		P.init(this, &VD, "shaders/PhongVert.spv", "shaders/PhongFrag.spv", {&DSL});
		P.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 								    VK_CULL_MODE_BACK_BIT, false);

		PW.init(this, &VD, "shaders/PhongVert.spv", "shaders/PhongFrag.spv", {&DSL});
		PW.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_LINE,
 								    VK_CULL_MODE_NONE, false);

		// Models, textures and Descriptors (values assigned to the uniforms)

/*		std::vector<Vertex> vertices = {
					   {{-10.0,0.0f,-10.0}, {0.0f,0.0f}, {0.0f,1.0f,0.0f}},
					   {{-10.0,0.0f, 10.0}, {0.0f,1.0f}, {0.0f,1.0f,0.0f}},
					   {{ 10.0,0.0f,-10.0}, {1.0f,0.0f}, {0.0f,1.0f,0.0f}},
					   {{ 10.0,0.0f, 10.0}, {1.0f,1.0f}, {0.0f,1.0f,0.0f}}};*/
//		M[0].vertices = std::vector<unsigned char>(vertices.size()*sizeof(Vertex), 0);
//		M[0].indices = {0, 1, 2,    1, 3, 2};
//		memcpy(&(M[0].vertices[0]), &vertices[0], vertices.size()*sizeof(Vertex));
/*for(int i = 0; i <= M[0].vertices.size(); i++) {
	std::cout << ((unsigned int)(M[0].vertices[i]) + 0) << "\n";
}*/
//		M[0].init(this, &VD, "models/LargePlane.obj", OBJ);

		
/*		std::vector<glm::vec3> vertices_pos = {
					   {-10.0,0.0f,-10.0},
					   {-10.0,0.0f, 10.0},
					   { 10.0,0.0f,-10.0},
					   { 10.0,0.0f, 10.0}};
		M[0].indices = {0, 1, 2,    1, 3, 2};*/
		std::vector<glm::vec3> vertices_pos[9];

		MakeSquare(2.0, vertices_pos[0], M[0].indices);
		MakeCube(2.0, vertices_pos[1], M[1].indices);
		MakeCube(1.0, vertices_pos[2], M[2].indices);
		MakeCube(0.5, vertices_pos[3], M[3].indices);
		MakeCylinder(1.0f, 2.0f, 32, vertices_pos[4], M[4].indices);
		MakeCone(1.0f, 2.0f, 32, vertices_pos[5], M[5].indices);
		MakeSphere(1.0f, 24, 48, vertices_pos[6], M[6].indices);
		MakeSphere(0.5f, 16, 32, vertices_pos[7], M[7].indices);
		MakeSphere(0.25f, 12, 24, vertices_pos[8], M[8].indices);


		nlohmann::json js;
		std::ifstream ifs("models/posmat.json");
		if (!ifs.is_open()) {
		  std::cout << "Error! Scene file not found!";
		  exit(-1);
		}
		try {
			std::cout << "Parsing JSON\n";
			ifs >> js;
			ifs.close();
//			std::cout << "\n\n\nJson contains " << js.size() << " parts\n\n\n";
			nlohmann::json ns = js["nodes"];
			nlohmann::json ma = js["materials"];
			nlohmann::json me = js["meshes"];
			for(int i = 0; i < 27; i++){
				glm::vec3 T;
				glm::vec3 S;
				glm::quat Q;
				if(ns[i].contains("translation")) {
//std::cout << "node " << i << " has T\n";
					T = glm::vec3(ns[i]["translation"][0],
								  ns[i]["translation"][1],
								  ns[i]["translation"][2]);
				} else {
					T = glm::vec3(0);
				}
				if(ns[i].contains("rotation")) {
//std::cout << "node " << i << " has Q\n";
					Q = glm::quat(ns[i]["rotation"][3],
								  ns[i]["rotation"][0],
								  ns[i]["rotation"][1],
								  ns[i]["rotation"][2]);
				} else {
					Q = glm::quat(1.0f,0.0f,0.0f,0.0f);
				}
				if(ns[i].contains("scale")) {
//std::cout << "node " << i << " has S\n";
					S = glm::vec3(ns[i]["scale"][0],
								  ns[i]["scale"][1],
								  ns[i]["scale"][2]);
				} else {
					S = glm::vec3(1);
				}
//printVec3("T",T);
//printQuat("Q",Q);
//printVec3("S",S);
				Wm2[i] = glm::translate(glm::mat4(1), T) *
						 glm::mat4(Q) *
						 glm::scale(glm::mat4(1), S);
//printMat4("Wm2", Wm2[i]);
				int mid = me[i]["primitives"][0]["material"];
				nlohmann::json cl = ma[mid]["pbrMetallicRoughness"]["baseColorFactor"];
//std::cout << cl[0] << "\n";
				MCol2[i] = glm::vec4(cl[0],cl[1],cl[2],cl[3]);
			}
		} catch (const nlohmann::json::exception& e) {
			std::cout << e.what() << '\n';
		}
		
		int mainStride = VD.Bindings[0].stride;
		for(int j = 0; j < 9; j++) {
			M[j].vertices = std::vector<unsigned char>(vertices_pos[j].size()*sizeof(Vertex), 0);
			for(int i = 0; i < vertices_pos[j].size(); i++) {
				Vertex *V_vertex = (Vertex *)(&(M[j].vertices[i * mainStride]));
				V_vertex->pos = vertices_pos[j][i];						
				V_vertex->UV = glm::vec2(vertices_pos[j][i].x, vertices_pos[j][i].z);						
				V_vertex->norm = glm::vec3(0.0f,1.0f,0.0f);		

//printVec3("V",vertices_pos[i]);			
			}
			M[j].initMesh(this, &VD);
			Wm[j] = glm::translate(glm::mat4(1), glm::vec3(2.5f*j - 11.25, 0, 0));
			MCol[j] = glm::vec4((float)j/8.0f, 1.0f-(float)j/8.0f, 0.5f, 1.0f);
		}
/*for(int i = 0; i < M[0].indices.size(); i++) {
	std::cout << M[0].indices[i] << ", ";
	if(i%3 == 2) std::cout << "\n";
}*/

		MAx.init(this, &VD, "models/axis.obj", OBJ);
		
		// Create the textures
		T1.init(this,   "textures/Textures.png");
		
		// updates the text
		outText[2].l[1] = (char *)malloc(200);
		sprintf((char *)outText[2].l[1], "QV%dI%d CyV%dI%d",
												   (int)vertices_pos[1].size(), (int)M[1].indices.size(),
												   (int)vertices_pos[4].size(), (int)M[4].indices.size());
		outText[2].l[2] = (char *)malloc(200);
		sprintf((char *)outText[2].l[2], "CoV%dI%d SV%dI%d",
												   (int)vertices_pos[5].size(), (int)M[5].indices.size(),
												   (int)vertices_pos[6].size(), (int)M[6].indices.size());
		txt.init(this, &outText);

		// Init local variables
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();
		PW.create();

		// Here you define the data set
		for(int i = 0; i < 27; i++) {
			DS[i].init(this, &DSL, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1},
					{2, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
				});
		}
		DSAx.init(this, &DSL, {
				{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
				{1, TEXTURE, 0, &T1},
				{2, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
			});
		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();
		PW.cleanup();

		for(int i = 0; i < 27; i++) {
			DS[i].cleanup();
		}
		DSAx.cleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		// Cleanup textures
		T1.cleanup();
		
		// Cleanup models
		for(int i = 0; i < 9; i++) {
			M[i].cleanup();
		}	
		MAx.cleanup();
		
		// Cleanup descriptor set layouts
		DSL.cleanup();
		
		// Destroies the pipelines
		P.destroy();		
		PW.destroy();		
		txt.localCleanup();		

		free((char *)outText[2].l[1]);
		free((char *)outText[2].l[2]);
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		if(WireFrame) {
			PW.bind(commandBuffer);
		} else {
			P.bind(commandBuffer);
		}
		
		if((currScene == 1) || ((currScene == 2) && (subpass >= 6))) {
			for(int i = 0; i < 27; i++) {
				M[JM2M[i]].bind(commandBuffer);
				DS[i].bind(commandBuffer, P, 0, currentImage);
							
				vkCmdDrawIndexed(commandBuffer,
						static_cast<uint32_t>(M[JM2M[i]].indices.size()), 1, 0, 0, 0);
			}
		} else if((currScene == 0) || ((currScene == 2) && (subpass < 6))) {
			for(int i = 0; i < 9; i++) {
				M[i].bind(commandBuffer);
				DS[i].bind(commandBuffer, P, 0, currentImage);
							
				vkCmdDrawIndexed(commandBuffer,
						static_cast<uint32_t>(M[i].indices.size()), 1, 0, 0, 0);
			}
		}		

		MAx.bind(commandBuffer);
		DSAx.bind(commandBuffer, P, 0, currentImage);
					
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MAx.indices.size()), 1, 0, 0, 0);

		txt.populateCommandBuffer(commandBuffer, currentImage, currScene);
	}

	// Here is where you update the uniforms.
	// Very likely this will be where you will be writing the logic of your application.
	void updateUniformBuffer(uint32_t currentImage) {
		static bool debounce = false;
		static int curDebounce = 0;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 2.0f;
		
		CamAlpha = CamAlpha - ROT_SPEED * deltaT * r.y;
		CamBeta  = CamBeta  - ROT_SPEED * deltaT * r.x;
		CamBeta  =  CamBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
				   (CamBeta > glm::radians( 90.0f) ? glm::radians( 90.0f) : CamBeta);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(1,0,0,1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,1,1);
		CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.z * uz * deltaT;
		
		static float subpassTimer = 0.0;

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				if(currScene != 2) {
					currScene = (currScene+1) % outText.size();

				}
				if(currScene == 2) {
					if(subpass >= 9) {
						currScene = 0;
					}
				}
				std::cout << "Scene : " << currScene << "\n";
				
				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		// Standard procedure to quit when the ESC key is pressed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if(glfwGetKey(window, GLFW_KEY_V)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_V;

				printVec3("CamPos  ", CamPos);				
				std::cout << "CamAlpha = " << CamAlpha << ";\n";
				std::cout << "CamBeta  = " << CamBeta  << ";\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		if(glfwGetKey(window, GLFW_KEY_X)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_X;

				WireFrame = !WireFrame;
				RebuildPipeline();
			}
		} else {
			if((curDebounce == GLFW_KEY_X) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		static int lastPipelineRebuilt = -1;
		if(currScene == 2) {
			switch(subpass) {
			  case 0:
				WireFrame = false;
				CamPos   = glm::vec3(-1.79185, 7.26235, 19.6948);
				CamAlpha = 0;
				CamBeta  = -0.347407;
				if(lastPipelineRebuilt < subpass) {
					RebuildPipeline();
					lastPipelineRebuilt = subpass;
				}
				break;
			  case 1:
				WireFrame = true;
				if(lastPipelineRebuilt < subpass) {
					RebuildPipeline();
					lastPipelineRebuilt = subpass;
				}
				break;
			  case 2:
				WireFrame = false;
				CamPos   = glm::vec3(-7.99313, 4.80239, 7.55174);
				CamAlpha = 0;
				CamBeta  = -0.504656;
				if(lastPipelineRebuilt < subpass) {
					RebuildPipeline();
					lastPipelineRebuilt = subpass;
				}
				break;
			  case 3:
				CamPos   = glm::vec3(-7.99313, -4.31876, 7.55174);
				CamAlpha = 0;
				CamBeta  = 0.492943;
				break;
			  case 4:
				CamPos   = glm::vec3(3.53189, -4.31876, 7.36667);
				CamAlpha = 0.165063;
				CamBeta  = 0.492943;
				break;
			  case 5:
				CamPos   = glm::vec3(-4.42871, 3.84966, 4.61547);
				CamAlpha = -0.878012;
				CamBeta  = -0.493502;
				break;
			  case 6:
				CamPos   = glm::vec3(-8.29774, 11.4186, -10.0742);
				CamAlpha = -2.42273;
				CamBeta  = -0.57736;
				if(lastPipelineRebuilt < subpass) {
					RebuildPipeline();
					lastPipelineRebuilt = subpass;
				}
				break;
			  case 7:
				CamPos   = glm::vec3(9.17948, 0.0244675, -8.76427);
				CamAlpha = -3.9461;
				CamBeta  = 0.370768;
				break;
			  case 8:
				CamPos   = glm::vec3(10.4681, 10.0861, 11.0488);
				CamAlpha = -5.40147;
				CamBeta  = -0.406513;
				break;
			}
		}
		
		if(currScene == 2) {
			subpassTimer += deltaT;
			if(subpassTimer > 1.0f) {
				subpassTimer = 0.0f;
				subpass++;
				std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
				char buf[20];
				sprintf(buf, "A05_%d.png", subpass);
				saveScreenshot(buf, currentImage);
				if(subpass == 9) {
					currScene = 0;
					std::cout << "Scene : " << currScene << "\n";
					RebuildPipeline();
				}
			}
		}


		// Here is where you actually update your uniforms
		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 50.0f);
		M[1][1] *= -1;

		glm::mat4 Mv =  glm::rotate(glm::mat4(1.0), -CamBeta, glm::vec3(1,0,0)) *
						glm::rotate(glm::mat4(1.0), -CamAlpha, glm::vec3(0,1,0)) *
						glm::translate(glm::mat4(1.0), -CamPos);

		glm::mat4 ViewPrj =  M * Mv;
		UniformBufferObject ubo{};
		glm::mat4 baseTr = glm::mat4(1.0f);								

		// updates global uniforms
		GlobalUniformBufferObject gubo{};
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = CamPos;

		if((currScene == 1) || ((currScene == 2) && (subpass >= 6))) {
			for(int i = 0; i < 27; i++) {
				ubo.mMat = Wm2[i] * baseTr;
				ubo.mvpMat = ViewPrj * ubo.mMat;
				ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
				ubo.color = MCol2[i];

				DS[i].map(currentImage, &ubo, sizeof(ubo), 0);
				DS[i].map(currentImage, &gubo, sizeof(gubo), 2);
			}
		} else if((currScene == 0) || ((currScene == 2) && (subpass < 6))) {
			for(int i = 0; i < 9; i++) {
				ubo.mMat = Wm[i] * baseTr;
				ubo.mvpMat = ViewPrj * ubo.mMat;
				ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
				ubo.color = MCol[i];

				DS[i].map(currentImage, &ubo, sizeof(ubo), 0);
				DS[i].map(currentImage, &gubo, sizeof(gubo), 2);
			}
		}

		ubo.mMat = baseTr * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f,1.0f,0.0f));
		ubo.mvpMat = ViewPrj * ubo.mMat;
		ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
		ubo.color = glm::vec4(1,1,1,1);
		DSAx.map(currentImage, &ubo, sizeof(ubo), 0);
		DSAx.map(currentImage, &gubo, sizeof(gubo), 2);
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    A05 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
