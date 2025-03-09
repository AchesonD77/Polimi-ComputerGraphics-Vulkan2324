// This has been adapted from the Vulkan tutorial

#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

std::vector<SingleText> outText = {
	{1, {"Press SPACE to save the screenshot", "", "", ""}, 0, 0},
	{2, {"Screenshot Saved!","Press ESC to quit","",""}, 0, 0}
};

// The uniform buffer object used in this example
struct UniformBufferObject {
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
	alignas(16) glm::vec4 overColor;
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




class A02;
void SetTransform(A02 *A);


// MAIN ! 
class A02 : public BaseProject {
	private:
		glm::mat4 mm[15];
		bool mhil[15];
	public:
		void SetMatrix(int i, glm::mat4 M, bool hil) {
			if((i >= 0) && (i < 15)) {
				mm[i] = M;
				mhil[i] = hil;
			}
		}
	protected:
	
	glm::mat4 Tpre[15];

	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;

	// Vertex formats
	VertexDescriptor VD;

	// Pipelines [Shader couples]
	Pipeline P;

	// Models, textures and Descriptors (values assigned to the uniforms)
	// Please note that Model objects depends on the corresponding vertex structure
	// Models
	Model<Vertex> M[15], MAx;
	// Descriptor sets
	DescriptorSet DS[15], DSAx;
	// Textures
	Texture T1, T2;

	TextMaker txt;
	
	// Other application parameters
	int currScene = 0;
	glm::vec3 CamPos = glm::vec3(0.0, 1.5, 7.0);
	float CamAlpha = 0.0f;
	float CamBeta = 0.0f;
	float Ar;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A02 - 3D Transformations";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.005f, 0.01f, 1.0f};
		
		// Descriptor pool sizes
		uniformBlocksInPool = 15 * 2 + 2;
		texturesInPool = 15 + 1 + 1;
		setsInPool = 15 + 1 + 1;

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
 								    VK_CULL_MODE_NONE, false);
		// Models, textures and Descriptors (values assigned to the uniforms)
		int i, j;
		char nameBuf[40];
		for(j = 0; j < 3; j++) {
			for(i = 0; i < 5; i++) {
				sprintf(nameBuf, "models/P%d%d.obj", j, i);
				M[j*5+i].init(this, &VD, nameBuf, OBJ);
			}
		}
		MAx.init(this, &VD, "models/axis.obj", OBJ);
		
		// Create the textures
		T1.init(this,   "textures/VanGogh.png");
		T2.init(this,   "textures/textures.png");

		// updates the text
		txt.init(this, &outText);

		// Init local variables
		SetTransform(this);

		Tpre[0] = glm::mat4(1);
		Tpre[1] = glm::mat4(1);
		Tpre[2] = glm::mat4(0.333333, -0, 0, -0, 0, 0.333333, 0, 0, 0, -0, 0.333333, -0, 2, 0, 1, 1);
		Tpre[3] = glm::mat4(0.965926, 0.258819, 0, 0, -0.258819, 0.965926, -0, 0, 0, -0, 1, -0, -0, 0, -1, 1);
		Tpre[4] = glm::mat4(-1, 0, -0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 2, 0, -1, 1);
		Tpre[5] = glm::mat4(1, -0, 0, -0, -0, 1, -0, 0, 0, -0, 1, -0, -4, 0, -4, 1);
		Tpre[6] = glm::mat4(0.5, -0, 0, -0, -0, 1, 0, 0, 0, -0, 0.8, -0, -0, 0, 1, 1);
		Tpre[7] = glm::mat4(1, -0, 0, -0, -0, 1, 0, 0, 0.5, -0, 1, -0, -2, 0, 1, 1);
		Tpre[8] = glm::mat4(0.333333, 0, 0.666667, 0, 0, 1, 0, 0, -0.666667, 0, -0.333333, 0, 0.333333, -2, -0.333333, 1);
		Tpre[9] = glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
		Tpre[10] = glm::mat4(0, 0, 0.5, 0, 0, 0.5, 0, 0, -0.5, 0,0, 0, 1, -1, -1.5, 1);
		Tpre[11] = glm::mat4(-0.707107, 0, 0.707107, 0, 0, 1, 0, 0, -0.707107, 0, -0.707107, 0, 2.70711, -2, 0.87868, 1);
		Tpre[12] = glm::mat4(0, 0, 1, 0, 0, 1, 0, 0, -2, 0, 0, 0, -1, -2, 0, 1);
		Tpre[13] = glm::mat4(0.5, -0.707107, 0.5, 0, -0.707107, 0, 0.707107, 0, -0.5, -0.707107, -0.5, 0, 0.914213, 0.707107, -0.914213, 1);
		Tpre[14] = glm::mat4(-0.866025, 0, 0.5, 0, 0, 1, 0, 0, -0.5, 0, -0.866025, 0, -2.23205, -2, -0.133975, 1);
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();

		// Here you define the data set
		for(int i = 0; i < 15; i++) {
			DS[i].init(this, &DSL, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T1},
					{2, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
				});
		}
		DSAx.init(this, &DSL, {
				{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
				{1, TEXTURE, 0, &T2},
				{2, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
			});
		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();

		// Cleanup datasets
		for(int i = 0; i < 15; i++) {
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
		T2.cleanup();
		
		// Cleanup models
		for(int i = 0; i < 15; i++) {
			M[i].cleanup();
		}	
		MAx.cleanup();
		
		// Cleanup descriptor set layouts
		DSL.cleanup();
		
		// Destroies the pipelines
		P.destroy();		
		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		P.bind(commandBuffer);

		for(int i = 0; i < 15; i++) {
			M[i].bind(commandBuffer);
			DS[i].bind(commandBuffer, P, 0, currentImage);
						
			vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(M[i].indices.size()), 1, 0, 0, 0);
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
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), CamAlpha, glm::vec3(0,1,0)) * glm::vec4(0,0,-1,1);
		CamPos = CamPos + MOVE_SPEED * m.x * ux * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.y * glm::vec3(0,1,0) * deltaT;
		CamPos = CamPos + MOVE_SPEED * m.z * uz * deltaT;
		

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				currScene = (currScene+1) % 2;
				std::cout << "Scene : " << currScene << "\n";

				if(!screenshotSaved) {
					saveScreenshot("A02.png", currentImage);
				}

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

		

		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 50.0f);
		M[1][1] *= -1;

		glm::mat4 Mv =  glm::rotate(glm::mat4(1.0), -CamBeta, glm::vec3(1,0,0)) *
						glm::rotate(glm::mat4(1.0), -CamAlpha, glm::vec3(0,1,0)) *
						glm::translate(glm::mat4(1.0), -CamPos);

		glm::mat4 ViewPrj =  M * Mv;
		UniformBufferObject ubo{};
		glm::mat4 baseTr = glm::mat4(1.0f);								
		// Here is where you actually update your uniforms

		// updates global uniforms
		GlobalUniformBufferObject gubo{};
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = CamPos;

		glm::mat4 AxTr = glm::scale(glm::mat4(1.0f), glm::vec3(0.0f));
		for(int i = 0; i < 15; i++) {
//			ubo.mMat = baseTr * Tpre[i] * AsTr[i] * Tpost[i];
			ubo.mMat = baseTr * Tpre[i] * mm[i];
			ubo.mvpMat = ViewPrj * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
			if(mhil[i]) {
				ubo.overColor = glm::vec4(1.0f, 0.2f, 0.2f, 0.3f);
				AxTr = Tpre[i] * mm[i];
			} else {
				ubo.overColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			}
			DS[i].map(currentImage, &ubo, sizeof(ubo), 0);
			DS[i].map(currentImage, &gubo, sizeof(gubo), 2);
		}
		ubo.mMat = baseTr * AxTr * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f,1.0f,0.0f));
		ubo.mvpMat = ViewPrj * ubo.mMat;
		ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
		DSAx.map(currentImage, &ubo, sizeof(ubo), 0);
		DSAx.map(currentImage, &gubo, sizeof(gubo), 2);

	}
};

#include "transforms.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    A02 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}