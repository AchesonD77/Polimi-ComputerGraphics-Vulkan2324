// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
#include "modules/Scene.hpp"


std::vector<SingleText> outText = {
	{2, {"BRDF", "Press SPACE to save the screenshots","",""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.", "", "",""}, 0, 0}
};

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
};



// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec3 norm;
};

struct VertexTan {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec4 tangent;
	glm::vec2 texCoord;
};



// MAIN ! 
class A08 : public BaseProject {
	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSLBlinn, DSLToon, DSLWard;

	// Vertex formats
	VertexDescriptor VD, VDT;

	// Pipelines [Shader couples]
	Pipeline PBlinn, PToon, PWard;

	// Scenes and texts
	Scene SC;
	std::vector<VertexDescriptorRef>  VDRs;
	std::vector<PipelineRef> PRs;
	
    TextMaker txt;
	
	// Other application parameters
	int currScene = 0;
	int subpass = 0;
		
	glm::vec3 CamPos = glm::vec3(0.0, 1.5, 7.0);
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
	float Ar;
	
	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A08 - BRDF";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
		
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
		DSLBlinn.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(UniformBufferObject)},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject)}
				});
		DSLToon.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(UniformBufferObject)},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject)}
				});
		DSLWard.init(this, {
					{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(UniformBufferObject)},
					{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0},
					{2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS, sizeof(GlobalUniformBufferObject)},
					{3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1}
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
		VDT.init(this, {
				  {0, sizeof(VertexTan), VK_VERTEX_INPUT_RATE_VERTEX}
				}, {
				  {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexTan, pos),
				         sizeof(glm::vec3), POSITION},
				  {0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexTan, normal),
				         sizeof(glm::vec3), NORMAL},
				  {0, 2, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(VertexTan, tangent),
				         sizeof(glm::vec3), TANGENT},
				  {0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexTan, texCoord),
				         sizeof(glm::vec2), UV}
				});

		VDRs.resize(2);
		VDRs[0].init("VD",  &VD);
		VDRs[1].init("VDT", &VDT);

		// Pipelines [Shader couples]
		PBlinn.init(this, &VD,  "shaders/Vert.spv",    "shaders/BlinnFrag.spv", {&DSLBlinn});
		PToon.init( this, &VD,  "shaders/Vert.spv",    "shaders/ToonFrag.spv",  {&DSLToon});
		PWard.init( this, &VDT, "shaders/TanVert.spv", "shaders/WardFrag.spv",  {&DSLWard});

		PRs.resize(3);
		PRs[0].init("PBlinn", &PBlinn, &VD);
		PRs[1].init("PToon",  &PToon,  &VD);
		PRs[2].init("PWard",  &PWard,  &VDT);

		// Descriptor pool sizes
		// WARNING!!!!!!!!
		// Must be set before initializing the text and the scene
		DPSZs.uniformBlocksInPool = 0;
		DPSZs.texturesInPool = 0;
		DPSZs.setsInPool = 0;

std::cout << "Loading the scene\n";
		if(SC.init(this, VDRs, PRs, "models/scene.json") != 0) {
			std::cout << "ERROR LOADING THE SCENE\n";
			exit(0);
		}
		
std::cout << "Initializing text\n";
		txt.init(this, &outText);

		std::cout << "Initialization completed!\n";
		std::cout << "Uniform Blocks in the Pool  : " << DPSZs.uniformBlocksInPool << "\n";
		std::cout << "Textures in the Pool        : " << DPSZs.texturesInPool << "\n";
		std::cout << "Descriptor Sets in the Pool : " << DPSZs.setsInPool << "\n";
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		PBlinn.create();
		PToon.create();
		PWard.create();

		// Here you define the data set
		SC.pipelinesAndDescriptorSetsInit();
		txt.pipelinesAndDescriptorSetsInit();		
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		PBlinn.cleanup();
		PToon.cleanup();
		PWard.cleanup();

		SC.pipelinesAndDescriptorSetsCleanup();
		txt.pipelinesAndDescriptorSetsCleanup();
	}

	// Here you destroy all the Models, Texture and Desc. Set Layouts you created!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	// You also have to destroy the pipelines: since they need to be rebuilt, they have two different
	// methods: .cleanup() recreates them, while .destroy() delete them completely
	void localCleanup() {	
		
		// Cleanup descriptor set layouts
		DSLBlinn.cleanup();
		DSLToon.cleanup();
		DSLWard.cleanup();
		
		// Destroies the pipelines
		PBlinn.destroy();
		PToon.destroy();
		PWard.destroy();

		SC.localCleanup();		
		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		SC.populateCommandBuffer(commandBuffer, currentImage);

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
		
		static float autoTime = true;
		static float cTime = 0.0;
		const float turnTime = 36.0f;
		const float angTurnTimeFact = 2.0f * M_PI / turnTime;
		
		if(autoTime) {
			cTime = cTime + deltaT;
			cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
		}
		cTime += r.z * angTurnTimeFact * 4.0;
		
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
				std::cout << "cTime    = " << cTime    << ";\n";
			}
		} else {
			if((curDebounce == GLFW_KEY_V) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		if(glfwGetKey(window, GLFW_KEY_M)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_M;

				autoTime = !autoTime;
			}
		} else {
			if((curDebounce == GLFW_KEY_M) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		
		if(currScene == 1) {
			switch(subpass) {
			  case 0:
CamPos   = glm::vec3(0.0644703, 6.442, 8.83251);
CamAlpha = 0;
CamBeta  = -0.4165;
cTime    = 2.40939;
autoTime = false;
				break;
			  case 1:
CamPos   = glm::vec3(-1.21796, 6.82323, 5.58497);
CamAlpha = 0.284362;
CamBeta  = -0.58455;
cTime    = 23.3533;
				break;
			  case 2:
CamPos   = glm::vec3(0.921455, 3.97743, 0.855181);
CamAlpha = -1.16426;
CamBeta  = -0.388393;
cTime    = 36.6178;
				break;
			  case 3:
 CamPos   = glm::vec3(5.59839, 4.04786, 2.59767);
CamAlpha = 1.01073;
CamBeta  = -0.213902;
cTime    = 15.6739;
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
				sprintf(buf, "A08_%d.png", subpass);
				saveScreenshot(buf, currentImage);
				if(subpass == 4) {
CamPos   = glm::vec3(0, 1.5, 7);
CamAlpha = 0;
CamBeta  = 0;
autoTime = true;
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
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(cTime * angTurnTimeFact), sin(glm::radians(135.0f)), cos(glm::radians(135.0f)) * sin(cTime * angTurnTimeFact));
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = CamPos;
		
		for(int i = 0; i < SC.InstanceCount; i++) {
			ubo.mMat = baseTr * SC.M[SC.I[i]->Mid]->Wm * SC.I[i]->Wm;
			ubo.mvpMat = ViewPrj * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

			SC.I[i]->DS[0]->map(currentImage, &ubo, sizeof(ubo), 0);
			SC.I[i]->DS[0]->map(currentImage, &gubo, sizeof(gubo), 2);
		}
	}
};

// This is the main: probably you do not need to touch this!
int main() {
    A08 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
