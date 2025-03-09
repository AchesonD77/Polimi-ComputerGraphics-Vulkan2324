// This has been adapted from the Vulkan tutorial

#define JSON_DIAGNOSTICS 1
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "modules/Scene.hpp"


std::vector<SingleText> outText = {
	{2, {"Lights", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0}
};

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
	alignas(16) glm::vec4 color;
};

struct GlobalUniformBufferObject {
	struct {
	alignas(16) glm::vec3 v;
	} lightDir[5];
	struct {
	alignas(16) glm::vec3 v;
	} lightPos[5];
	alignas(16) glm::vec4 lightColor[5];
	alignas(4) float cosIn;
	alignas(4) float cosOut;
	alignas(16) glm::vec3 eyePos;
	alignas(16) glm::vec4 eyeDir;
	alignas(16) glm::vec4 lightOn;
};



// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec3 norm;
};




// MAIN ! 
class A07 : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;

	// Vertex formats
	VertexDescriptor VD;

	// Pipelines [Shader couples]
	Pipeline P;

	// Models
	Scene SC;

	TextMaker txt;
	
	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 1.5, 7.0);
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
	float Ar;
	
	glm::mat4 LWm[5];
	glm::vec3 LCol[5];
	float LInt[5];
	float ScosIn, ScosOut;
	glm::vec4 lightOn;
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A07 - Light";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 7 * 2 + 2;
		texturesInPool = 7 + 1;
		setsInPool = 7 + 1;

		Ar = (float)windowWidth / (float)windowHeight;
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
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(UniformBufferObject)},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject)}
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

		SC.init(this, &VD, &DSL, P, "models/scene.json");
		
		txt.init(this, &outText);

		// Init local variables
		nlohmann::json js;
		std::ifstream ifs("models/Lights.json");
		if (!ifs.is_open()) {
		  std::cout << "Error! Lights file not found!";
		  exit(-1);
		}
		try {
			std::cout << "Parsing JSON\n";
			ifs >> js;
			ifs.close();
//			std::cout << "\n\n\nJson contains " << js.size() << " parts\n\n\n";
			nlohmann::json ns = js["nodes"];
			nlohmann::json ld = js["extensions"]["KHR_lights_punctual"]["lights"];
			for(int i = 0; i < 5; i++){
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
				LWm[i] = glm::translate(glm::mat4(1), T) *
						 glm::mat4(Q) *
						 glm::scale(glm::mat4(1), S);
//printMat4("LWm", LWm[i]);
				nlohmann::json cl = ld[i]["color"];
//std::cout << cl[0] << "\n";
				LCol[i] = glm::vec3(cl[0],cl[1],cl[2]);
//printVec3("LCol",LCol[i]);
				LInt[i] = ld[i]["intensity"];
//std::cout << LInt[i] << "\n\n";
			}
			ScosIn = cos((float)ld[4]["spot"]["innerConeAngle"]);
			ScosOut = cos((float)ld[4]["spot"]["outerConeAngle"]);
		} catch (const nlohmann::json::exception& e) {
			std::cout << e.what() << '\n';
		}
//std::cout << ScosIn << " " << ScosOut << "\n";

		lightOn = glm::vec4(1);
		std::cout << "Initialization completed!\n";
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();

		// Here you define the data set
		SC.pipelinesAndDescriptorSetsInit();
		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();

		SC.pipelinesAndDescriptorSetsCleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		
		// Cleanup descriptor set layouts
		DSL.cleanup();
		
		// Destroies the pipelines
		P.destroy();		

		SC.localCleanup();		
		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		P.bind(commandBuffer);
		SC.populateCommandBuffer(commandBuffer, currentImage, P);

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
				if(currScene != 1) {
					currScene = (currScene+1) % outText.size();

				}
				if(currScene == 1) {
					if(subpass >= 4) {
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

		if(glfwGetKey(window, GLFW_KEY_1)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_1;
				lightOn.x = 1-lightOn.x;
			}
		} else {
			if((curDebounce == GLFW_KEY_1) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		if(glfwGetKey(window, GLFW_KEY_2)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_2;
				lightOn.y = 1-lightOn.y;
			}
		} else {
			if((curDebounce == GLFW_KEY_2) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		if(glfwGetKey(window, GLFW_KEY_3)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_3;
				lightOn.z = 1-lightOn.z;
			}
		} else {
			if((curDebounce == GLFW_KEY_3) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		if(glfwGetKey(window, GLFW_KEY_4)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_4;
				lightOn.w = 1-lightOn.w;
			}
		} else {
			if((curDebounce == GLFW_KEY_4) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}
		
		if(currScene == 1) {
			switch(subpass) {
			  case 0:
CamPos   = glm::vec3(-3.2829, 3.32563, 6.59965);
CamAlpha = -0.247342;
CamBeta  = -0.289563;
lightOn  = glm::vec4(1,1,1,1);
				break;
			  case 1:
CamPos   = glm::vec3(7.46434, 5.44747, -0.464819);
CamAlpha = -5.12519;
CamBeta  = -0.586596;
lightOn  = glm::vec4(3,0,0,0);
				break;
			  case 2:
CamPos   = glm::vec3(0.803954, 6.86648, -7.92184);
CamAlpha = -3.34843;
CamBeta  = -0.824523;
lightOn  = glm::vec4(0,2,0,0);
				break;
			  case 3:
CamPos   = glm::vec3(-4.55548, 7.91204, -5.93445);
CamAlpha = -2.48351;
CamBeta  = -0.930016;
lightOn  = glm::vec4(0,0,5,0);
				break;
			}
		}
		
		if(currScene == 1) {
			subpassTimer += deltaT;
			if(subpassTimer > 4.0f) {
				subpassTimer = 0.0f;
				subpass++;
				std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
				char buf[20];
				sprintf(buf, "A07_%d.png", subpass);
				saveScreenshot(buf, currentImage);
				if(subpass == 4) {
CamPos   = glm::vec3(0, 1.5, 7);
CamAlpha = 0;
CamBeta  = 0;
lightOn  = glm::vec4(1,1,1,1);
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

//		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
		for(int i = 0; i <5; i++) {
			gubo.lightColor[i] = glm::vec4(LCol[i], LInt[i]);
			gubo.lightDir[i].v = LWm[i] * glm::vec4(0,0,1,0);
			gubo.lightPos[i].v = LWm[i] * glm::vec4(0,0,0,1);
		}
//printVec3("LD", gubo.lightDir[0]);
//printVec3("LP", gubo.lightPos[0]);
		gubo.cosIn = ScosIn;
		gubo.cosOut = ScosOut;
//printVec3("LD", gubo.lightDir);
//printVec4("lld", lld);
//		gubo.lightColor = glm::vec4(LCol[3], 1.0f);

		gubo.eyePos = CamPos;
		gubo.lightOn = lightOn;
		
		for(int i = 0; i < SC.InstanceCount; i++) {
//			ubo.mMat = baseTr * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f,1.0f,0.0f));
			ubo.mMat = baseTr * SC.M[SC.I[i].Mid]->Wm;
			ubo.mvpMat = ViewPrj * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
			ubo.color = glm::vec4(1,1,1,1);
			SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
			SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
		}
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    A07 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
