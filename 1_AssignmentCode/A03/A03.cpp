// This has been adapted from the Vulkan tutorial

// TO MOVE
#define JSON_DIAGNOSTICS 1
#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"


std::vector<SingleText> outText = {
	{2, {"Ortographic Front", "Press SPACE to change projection", "", ""}, 0, 0},
	{2, {"Isometric", "Press SPACE to change projection", "", ""}, 0, 0},
	{2, {"Dimetic", "Press SPACE to change projection", "", ""}, 0, 0},
	{2, {"Trimetric", "Press SPACE to change projection", "", ""}, 0, 0},
	{2, {"Parallel, cabinet", "Press SPACE to change projection", "", ""}, 0, 0},
	{2, {"Perspective, normal", "Press SPACE to change projection", "", ""}, 0, 0},
	{2, {"Perspective, zoom", "Press SPACE to change projection", "", ""}, 0, 0},
	{2, {"Perspective, wide", "Press SPACE to save the screenshots", "", ""}, 0, 0},
	{1, {"Saving Screenshots. Please wait.","","",""}, 0, 0},
	{2, {"Screenshot Saved!","Press ESC to quit","",""}, 0, 0}
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
	alignas(16) glm::vec4 eyeDir;
};



// The vertices data structures
// Example
struct Vertex {
	glm::vec3 pos;
	glm::vec2 UV;
	glm::vec3 norm;
};

#include "modules/Scene.hpp"


class A03;
void SetProjections(A03 *A);


// MAIN ! 
class A03 : public BaseProject {
	private:
		glm::mat4 PrjM[8];

	public:
		void SetMatrix(int i, glm::mat4 M) {
			if((i >= 0) && (i < 8)) {
				PrjM[i] = M;
			}
		}

	protected:
	
	// Descriptor Layouts ["classes" of what will be passed to the shaders]
	DescriptorSetLayout DSL;

	// Vertex formats
	VertexDescriptor VD;

	// Pipelines [Shader couples]
	Pipeline P;

	// Models
	Model<Vertex> M1;
	// Descriptor sets
	DescriptorSet DS1;

	Scene SC;

	TextMaker txt;
	
	// Other application parameters
	int currScene = 0;
	float Ar;
	glm::vec3 Pos;
	glm::vec3 InitialPos;

	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "A03 - Projections";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.85f, 1.0f, 1.0f};
		
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
		M1.vertices = {{{-100.0,0.0f,-100.0}, {0.45f,0.1f}, {0.0f,1.0f,0.0f}},
					   {{-100.0,0.0f, 100.0}, {0.45f,0.1f}, {0.0f,1.0f,0.0f}},
					   {{ 100.0,0.0f,-100.0}, {0.45f,0.1f}, {0.0f,1.0f,0.0f}},
					   {{ 100.0,0.0f, 100.0}, {0.45f,0.1f}, {0.0f,1.0f,0.0f}}};
		M1.indices = {0, 1, 2,    1, 3, 2};
		M1.initMesh(this, &VD);


		// Load Scene
		SC.init(this, &VD, DSL, P, "models/scene.json");
		
		// updates the text
		txt.init(this, &outText);

		// Init local variables
		SetProjections(this);
		Pos = glm::vec3(SC.I[SC.InstanceIds["ge"]].Wm[3]);
		InitialPos = Pos;
	}
	
	// Here you create your pipelines and Descriptor Sets!
	void pipelinesAndDescriptorSetsInit() {
		// This creates a new pipeline (with the current surface), using its shaders
		P.create();

		DS1.init(this, &DSL, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, SC.T[SC.TextureIds["t0"]]},
					{2, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
				});

		// Here you define the data set
		SC.pipelinesAndDescriptorSetsInit(DSL);
		txt.pipelinesAndDescriptorSetsInit();
	}

	// Here you destroy your pipelines and Descriptor Sets!
	// All the object classes defined in Starter.hpp have a method .cleanup() for this purpose
	void pipelinesAndDescriptorSetsCleanup() {
		// Cleanup pipelines
		P.cleanup();
		DS1.cleanup();

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

		// Cleanup models
		M1.cleanup();

		SC.localCleanup();		
		txt.localCleanup();		
	}
	
	// Here it is the creation of the command buffer:
	// You send to the GPU all the objects you want to draw,
	// with their buffers and textures
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
		// binds the pipeline
		P.bind(commandBuffer);

		// binds the data set
		DS1.bind(commandBuffer, P, 0, currentImage);
					
		// binds the model
		M1.bind(commandBuffer);
		
		// record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(M1.indices.size()), 1, 0, 0, 0);

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
		
		const float ROT_SPEED = glm::radians(360.0f);
		const float MOVE_SPEED = 10.0f;
		int gelatoId = SC.InstanceIds["ge"];
		static float ang;
		static float lookAng = 0;
		static float DlookAng = 0;
		static int subpass = 0;
		static float subpassTimer = 0.0;

		m.y = 0;
		m = glm::vec3(glm::rotate(glm::mat4(1), DlookAng, glm::vec3(0,1,0)) * glm::vec4(m, 1));
		Pos += m * MOVE_SPEED * deltaT;
		
		lookAng -= r.y * ROT_SPEED * deltaT;
		lookAng = (lookAng < -3.1416) ? lookAng + 2*3.1416 : ((lookAng > 3.1416) ? lookAng - 2*3.1416 : lookAng);
		DlookAng = 3.1416/2 * round(2*lookAng/3.1416);
//std::cout << DlookAng;
		
		if((m.x != 0) || (m.z != 0)) {
			ang	= atan2(-m.z, m.x) + 3.1416/2.0;
		}
		SC.I[gelatoId].Wm = glm::translate(glm::mat4(1), Pos) * glm::rotate(glm::mat4(1), ang, glm::vec3(0,1,0));
		

		if(glfwGetKey(window, GLFW_KEY_SPACE)) {
			if(!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				if(currScene != 8) {
					currScene = (currScene+1) % outText.size();
				}
				if(currScene == 8) {
					if(subpass >= 8) {
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

		
		glm::mat4 M ;
		if(currScene < 8) {
			M = PrjM[currScene];
		} else if(currScene == 8) {
			Pos = InitialPos;
			SC.I[gelatoId].Wm = glm::translate(glm::mat4(1), Pos);
			ang = lookAng = DlookAng = 0;

			M = PrjM[subpass];
			subpassTimer += deltaT;
			if(subpassTimer > 1.0f) {
				subpassTimer = 0.0f;
				subpass++;
				std::cout << "Scene : " << currScene << " subpass: " << subpass << "\n";
				char buf[20];
				sprintf(buf, "A03_%d.png", subpass);
				saveScreenshot(buf, currentImage);
				if(subpass == 8) {
					currScene++;
					std::cout << "Scene : " << currScene << "\n";
					RebuildPipeline();
				}
			}
		} else {
			M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 500.0f);
			M[1][1] *= -1;			
		}

		
		glm::mat4 Mv =  glm::inverse(
							glm::translate(glm::mat4(1), Pos) *
							glm::rotate(glm::mat4(1), DlookAng, glm::vec3(0,1,0)) *
							glm::translate(glm::mat4(1), glm::vec3(0,2,8))
						);

		glm::mat4 ViewPrj =  M * Mv;
		UniformBufferObject ubo{};
		glm::mat4 baseTr = glm::mat4(1.0f);								
		// Here is where you actually update your uniforms

		// updates global uniforms
		GlobalUniformBufferObject gubo{};
		gubo.lightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
		gubo.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.eyePos = glm::vec3(glm::inverse(Mv) * glm::vec4(0,0,0,1));
		gubo.eyeDir = glm::vec4(0);
		if(((currScene > 4) && (currScene < 8)) || ((currScene == 8) && (subpass > 4))){
			gubo.eyeDir.w = 0.0;
		} else {
			gubo.eyeDir.w = 1.0;
		}

		for(int i = 0; i < SC.InstanceCount; i++) {
			ubo.mMat = SC.I[i].Wm * baseTr;
			ubo.mvpMat = ViewPrj * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

			SC.DS[i]->map(currentImage, &ubo, sizeof(ubo), 0);
			SC.DS[i]->map(currentImage, &gubo, sizeof(gubo), 2);
		}

		ubo.mMat = baseTr;
		ubo.mvpMat = ViewPrj * ubo.mMat;
		ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));
		DS1.map(currentImage, &ubo, sizeof(ubo), 0);
		DS1.map(currentImage, &gubo, sizeof(gubo), 2);
	}
};

#include "cameras.hpp"

// This is the main: probably you do not need to touch this!
int main() {
    A03 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}